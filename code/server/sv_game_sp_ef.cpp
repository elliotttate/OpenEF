/*
===========================================================================
sv_game_sp_ef.cpp -- Bridge between OpenJK engine and EF1 SP game module

The EF1 SP game module uses different struct layouts for entityState_t,
playerState_t, gentity_t, and trace_t than the OpenJK engine.  This file
maintains shadow arrays in engine-native format and translates field-by-field
between the two layouts around every engine operation.

Architecture follows ioEF's sv_game_sp.c pattern.
===========================================================================
*/

#ifdef EF_MODE

#include "server.h"
#include "../../codeEF/qcommon/sp_types.h"

// Functions from sv_game.cpp and sv_world.cpp not declared in server.h
extern void SV_SetBrushModel( gentity_t *ent, const char *name );
extern void SV_AdjustAreaPortalState( gentity_t *ent, qboolean open );
extern qboolean SV_EntityContact( const vec3_t mins, const vec3_t maxs, const gentity_t *ent );

// ============================================================================
// Shadow arrays -- engine reads these instead of the game DLL's entities
// ============================================================================

gentity_t           sv_ef_entities[MAX_GENTITIES];
static playerState_t sv_ef_playerState;
static qboolean     sv_ef_initialized = qfalse;

// Cached game module pointers
static sp_gentity_t *sv_ef_ge_gentities = NULL;
static int           sv_ef_ge_gentitySize = 0;

// Forward declarations
static void SV_EF_EnsureInit( void );

// ============================================================================
// Helper: get SP entity by number
// ============================================================================

static sp_gentity_t *SV_EF_GetSPEntity( int entNum ) {
	return (sp_gentity_t *)( (byte *)sv_ef_ge_gentities + sv_ef_ge_gentitySize * entNum );
}

// ============================================================================
// Entity sync: SP -> Engine (before engine reads entity data)
// ============================================================================

void SV_EF_SyncToShared( int entNum ) {
	sp_gentity_t *sp = SV_EF_GetSPEntity( entNum );
	gentity_t *dst = &sv_ef_entities[entNum];
	sp_entityState_t *sps = &sp->s;
	entityState_t *des = &dst->s;

	// entityState_t field-by-field copy (layouts differ after modelindex2)
	des->number          = sps->number;
	des->eType           = sps->eType;
	des->eFlags          = sps->eFlags;
	des->pos             = sps->pos;
	des->apos            = sps->apos;
	des->time            = sps->time;
	des->time2           = sps->time2;
	VectorCopy( sps->origin, des->origin );
	VectorCopy( sps->origin2, des->origin2 );
	VectorCopy( sps->angles, des->angles );
	VectorCopy( sps->angles2, des->angles2 );
	des->otherEntityNum  = sps->otherEntityNum;
	des->otherEntityNum2 = sps->otherEntityNum2;
	des->groundEntityNum = sps->groundEntityNum;
	des->constantLight   = sps->constantLight;
	des->loopSound       = sps->loopSound;
	des->modelindex      = sps->modelindex;
	des->modelindex2     = sps->modelindex2;
	// Skip modelindex3 (SP-only)
	des->modelindex3     = sps->modelindex3;
	des->clientNum       = sps->clientNum;
	des->frame           = sps->frame;
	des->solid           = sps->solid;
	des->event           = sps->event;
	des->eventParm       = sps->eventParm;
	des->powerups        = sps->powerups;
	des->weapon          = sps->weapon;
	des->legsAnim        = sps->legsAnim;
	des->legsAnimTimer   = sps->legsAnimTimer;
	des->torsoAnim       = sps->torsoAnim;
	des->torsoAnimTimer  = sps->torsoAnimTimer;
	des->scale           = sps->scale;
	// pushVec, saberInFlight, saberActive, vehicleModel -- not synced (engine doesn't need them)

	// gentity_t engine-visible fields (after entityState_t)
	dst->client  = NULL; // Engine doesn't chase this pointer for SP
	dst->inuse   = sp->inuse;
	dst->linked  = sp->linked;
	dst->svFlags = sp->svFlags;
	dst->bmodel  = sp->bmodel;
	// Sanity check: if bmodel is set but modelindex is out of range, clear it
	if ( dst->bmodel && (des->modelindex < 0 || des->modelindex >= 512) ) {
		dst->bmodel = qfalse;
	}
	VectorCopy( sp->mins, dst->mins );
	VectorCopy( sp->maxs, dst->maxs );
	dst->contents = sp->contents;
	VectorCopy( sp->absmin, dst->absmin );
	VectorCopy( sp->absmax, dst->absmax );
	VectorCopy( sp->currentOrigin, dst->currentOrigin );
	VectorCopy( sp->currentAngles, dst->currentAngles );
	if ( sp->owner && sp->owner->s.number >= 0 && sp->owner->s.number < MAX_GENTITIES ) {
		dst->owner = &sv_ef_entities[sp->owner->s.number];
	} else {
		dst->owner = NULL;
	}
}

// ============================================================================
// Entity sync: Engine -> SP (after engine modifies entity)
// ============================================================================

void SV_EF_SyncFromShared( int entNum ) {
	gentity_t *src = &sv_ef_entities[entNum];
	sp_gentity_t *sp = SV_EF_GetSPEntity( entNum );

	// Only copy back fields the engine may have modified
	sp->linked = src->linked;
	VectorCopy( src->absmin, sp->absmin );
	VectorCopy( src->absmax, sp->absmax );
	// s.number may be set by engine during init
	sp->s.number = src->s.number;
}

// ============================================================================
// Bulk sync all active entities
// ============================================================================

void SV_EF_SyncAllEntities( void ) {
	SV_EF_EnsureInit();
	if ( !sv_ef_initialized || !sv_ef_ge_gentities ) return;

	int count = ge->num_entities;
	if ( count > MAX_GENTITIES ) count = MAX_GENTITIES;
	for ( int i = 0; i < count; i++ ) {
		sp_gentity_t *sp = SV_EF_GetSPEntity( i );
		if ( sp && sp->inuse ) {
			SV_EF_SyncToShared( i );
		}
	}
}

// ============================================================================
// Player state sync: SP -> Engine
// ============================================================================

void SV_EF_SyncPlayerState( void ) {
	if ( !sv_ef_ge_gentities ) return;

	sp_gentity_t *playerEnt = SV_EF_GetSPEntity( 0 );
	if ( !playerEnt->client ) return;

	// sp_playerState_t is at the address playerEnt->client points to
	sp_playerState_t *sp_ps = (sp_playerState_t *)playerEnt->client;
	playerState_t *ps = &sv_ef_playerState;

	memset( ps, 0, sizeof( *ps ) );

	ps->commandTime     = sp_ps->commandTime;
	ps->pm_type         = sp_ps->pm_type;
	ps->bobCycle        = sp_ps->bobCycle;
	ps->pm_flags        = sp_ps->pm_flags;
	ps->pm_time         = sp_ps->pm_time;
	VectorCopy( sp_ps->origin, ps->origin );
	VectorCopy( sp_ps->velocity, ps->velocity );
	ps->weaponTime      = sp_ps->weaponTime;
	ps->gravity         = sp_ps->gravity;
	ps->speed           = sp_ps->speed;
	ps->delta_angles[0] = sp_ps->delta_angles[0];
	ps->delta_angles[1] = sp_ps->delta_angles[1];
	ps->delta_angles[2] = sp_ps->delta_angles[2];
	ps->groundEntityNum = sp_ps->groundEntityNum;
	ps->legsAnim        = sp_ps->legsAnim;
	ps->legsAnimTimer   = sp_ps->legsAnimTimer;
	ps->torsoAnim       = sp_ps->torsoAnim;
	ps->torsoAnimTimer  = sp_ps->torsoAnimTimer;
	ps->movementDir     = sp_ps->movementDir;
	ps->eFlags          = sp_ps->eFlags;
	ps->eventSequence   = sp_ps->eventSequence;
	ps->events[0]       = sp_ps->events[0];
	ps->events[1]       = sp_ps->events[1];
	ps->eventParms[0]   = sp_ps->eventParms[0];
	ps->eventParms[1]   = sp_ps->eventParms[1];
	ps->externalEvent      = sp_ps->externalEvent;
	ps->externalEventParm  = sp_ps->externalEventParm;
	ps->externalEventTime  = sp_ps->externalEventTime;
	ps->clientNum       = sp_ps->clientNum;
	ps->weapon          = sp_ps->weapon;
	ps->weaponstate     = sp_ps->weaponstate;
	VectorCopy( sp_ps->viewangles, ps->viewangles );
	ps->viewheight      = sp_ps->viewheight;
	ps->damageEvent     = sp_ps->damageEvent;
	ps->damageYaw       = sp_ps->damageYaw;
	ps->damagePitch     = sp_ps->damagePitch;
	ps->damageCount     = sp_ps->damageCount;
	memcpy( ps->stats, sp_ps->stats, sizeof( ps->stats ) );
	memcpy( ps->persistant, sp_ps->persistant, sizeof( ps->persistant ) );
	memcpy( ps->powerups, sp_ps->powerups, sizeof( ps->powerups ) );
	// SP ammo[4] -> engine ammo[MAX_WEAPONS]. Copy 4, rest stays zero.
	memcpy( ps->ammo, sp_ps->ammo, sizeof( sp_ps->ammo ) );
	ps->ping            = sp_ps->ping;
}

// ============================================================================
// Initialize the shadow entity system
// ============================================================================

static void SV_EF_EnsureInit( void ) {
	if ( sv_ef_initialized ) return;
	if ( !ge || !ge->gentities || !ge->gentitySize ) return;

	Com_Printf("SV_EF: Shadow entity init: gentities=%p, gentitySize=%d\n",
		(void*)ge->gentities, ge->gentitySize);
	sv_ef_ge_gentities = (sp_gentity_t *)ge->gentities;
	sv_ef_ge_gentitySize = ge->gentitySize;

	memset( sv_ef_entities, 0, sizeof( sv_ef_entities ) );
	memset( &sv_ef_playerState, 0, sizeof( sv_ef_playerState ) );

	// Set entity 0 (player) client pointer for snapshot builder
	sv_ef_entities[0].client = &sv_ef_playerState;

	sv_ef_initialized = qtrue;
}

void SV_EF_InitShadowEntities( void ) {
	sv_ef_initialized = qfalse; // Force re-init
	SV_EF_EnsureInit();

	Com_Printf("SV_EF: Full sync starting, num_entities=%d\n", ge->num_entities);

	// Full sync now that all entities are spawned
	int count = ge->num_entities;
	if ( count > MAX_GENTITIES ) count = MAX_GENTITIES;
	for ( int i = 0; i < count; i++ ) {
		sp_gentity_t *sp = SV_EF_GetSPEntity( i );
		if ( sp && sp->inuse ) {
			SV_EF_SyncToShared( i );
		}
	}
	Com_Printf("SV_EF: Full sync complete\n");
	SV_EF_SyncPlayerState();
	Com_Printf("SV_EF: Player state synced\n");
}

// ============================================================================
// Wrapper functions for game import callbacks
// These translate between SP entity pointers and engine shadow entities
// ============================================================================

void SV_EF_LinkEntity( sp_gentity_t *sp_ent ) {
	SV_EF_EnsureInit();
	if ( !sv_ef_initialized ) return; // ge not ready yet
	int entNum = sp_ent->s.number;
	if ( entNum < 0 || entNum >= MAX_GENTITIES ) return;
	SV_EF_SyncToShared( entNum );
	SV_LinkEntity( &sv_ef_entities[entNum] );
	SV_EF_SyncFromShared( entNum );
}

void SV_EF_UnlinkEntity( sp_gentity_t *sp_ent ) {
	SV_EF_EnsureInit();
	if ( !sv_ef_initialized ) return;
	int entNum = sp_ent->s.number;
	if ( entNum < 0 || entNum >= MAX_GENTITIES ) return;
	SV_EF_SyncToShared( entNum );
	SV_UnlinkEntity( &sv_ef_entities[entNum] );
	SV_EF_SyncFromShared( entNum );
}

void SV_EF_SetBrushModel( sp_gentity_t *sp_ent, const char *name ) {
	SV_EF_EnsureInit();
	if ( !sv_ef_initialized ) return;
	int entNum = sp_ent->s.number;
	if ( entNum < 0 || entNum >= MAX_GENTITIES ) return;
	SV_EF_SyncToShared( entNum );
	SV_SetBrushModel( &sv_ef_entities[entNum], name );
	SV_EF_SyncFromShared( entNum );
	// SetBrushModel also sets s.modelindex, mins, maxs, contents
	sp_ent->s.modelindex = sv_ef_entities[entNum].s.modelindex;
	VectorCopy( sv_ef_entities[entNum].mins, sp_ent->mins );
	VectorCopy( sv_ef_entities[entNum].maxs, sp_ent->maxs );
	sp_ent->contents = sv_ef_entities[entNum].contents;
}

void SV_EF_AdjustAreaPortalState( sp_gentity_t *sp_ent, qboolean open ) {
	SV_EF_EnsureInit();
	int entNum = sp_ent->s.number;
	SV_EF_SyncToShared( entNum );
	SV_AdjustAreaPortalState( &sv_ef_entities[entNum], open );
}

qboolean SV_EF_EntityContact( const vec3_t mins, const vec3_t maxs, const sp_gentity_t *sp_ent ) {
	SV_EF_EnsureInit();
	int entNum = sp_ent->s.number;
	// const_cast needed because SyncToShared takes non-const
	SV_EF_SyncToShared( entNum );
	return SV_EntityContact( mins, maxs, &sv_ef_entities[entNum] );
}

int SV_EF_EntitiesInBox( const vec3_t mins, const vec3_t maxs, sp_gentity_t **sp_list, int maxcount ) {
	SV_EF_EnsureInit();
	gentity_t *engineList[MAX_GENTITIES];
	int count = SV_AreaEntities( mins, maxs, engineList, maxcount );

	// Translate engine entity pointers back to SP game entity pointers
	for ( int i = 0; i < count; i++ ) {
		int entNum = engineList[i]->s.number;
		sp_list[i] = SV_EF_GetSPEntity( entNum );
	}
	return count;
}

#endif // EF_MODE
