/*
===========================================================================
sp_types.h -- struct definitions for EF1 singleplayer game module ABI

BACKGROUND
----------
The EF1 singleplayer campaign shipped as precompiled DLLs (efgamex86.dll
for the server-side game logic, efuix86.dll / efcgamex86.dll for the
client-side cgame/UI).  These DLLs were compiled against Ritual
Entertainment's modified Quake 3 engine, which used different struct
layouts for playerState_t, entityState_t, and snapshot_t than ioEF's
multiplayer-oriented layouts.

THE ABI BOUNDARY PROBLEM
------------------------
ioEF and the SP DLLs share memory across a C function-call boundary: the
engine passes pointers to playerState / entityState / snapshot structs,
and the DLL reads and writes fields by offset.  If the engine's struct
layout doesn't match what the DLL was compiled with, every field after
the first divergence will be read from or written to the wrong memory
location, causing silent data corruption, wrong animation frames, broken
physics, and crashes.

We cannot recompile the original SP DLLs (no source available for the
final retail builds), so the engine must present data in exactly the
binary layout those DLLs expect.  That is the purpose of every struct
in this file: to replicate the SP-side ABI so that the engine and the
precompiled SP modules agree on where every field lives in memory.

IMPORTANT MAINTENANCE NOTES
----------------------------
- These structs MUST match the layout the SP DLLs were compiled with.
  The authoritative reference is the Elite-Reinforce game source
  (game/q_shared.h and cgame/cg_public.h), which is a reconstruction
  of the original Ritual SP game source.
- Do NOT add or remove fields.  Do NOT reorder fields.  Do NOT change
  types.  Any of these will silently break the ABI.
- Padding and alignment matter.  The SP DLLs were compiled with MSVC's
  default packing rules (natural alignment, no #pragma pack).  If you
  change compilers, verify struct sizes with static_assert or offsetof.
===========================================================================
*/

#ifndef SP_TYPES_H
#define SP_TYPES_H

// When included from the engine (which already has its own q_shared.h),
// skip the EF q_shared.h include. The types we need (vec3_t, trajectory_t,
// qboolean, etc.) are already defined.
#ifndef __Q_SHARED_H
#include "q_shared.h"
#endif

// ============================================================================
// SP entityState_t
//
// This is the entity state struct as laid out by the SP game DLLs.  It
// carries per-entity data from the server game module to the cgame for
// rendering.
//
// DIFFERENCES FROM ioEF entityState_t:
//
// The SP version includes five fields that ioEF's multiplayer entityState_t
// does not have:
//
//   modelindex3     -- SP uses a third model slot for multi-part entities
//                      (e.g., some enemies have weapon/accessory sub-models)
//   legsAnimTimer   -- countdown timer for the legs animation blend;
//                      the SP animation system blends between animations
//                      over a timed window rather than snapping instantly
//   torsoAnimTimer  -- same as above, but for the upper-body animation
//   scale           -- per-entity scale factor; used by SP for cinematic
//                      effects and certain enemy types (e.g., scaled-up
//                      boss entities)
//   pushVec         -- directional push/knockback vector applied to the
//                      entity, used by SP's physics to show impact effects
//
// OFFSET IMPLICATIONS:
//
// Fields up to and including modelindex2 are at identical byte offsets in
// both the SP and ioEF layouts.  The insertion of modelindex3 between
// modelindex2 and clientNum shifts every subsequent field by 4 bytes.
// Then legsAnimTimer, torsoAnimTimer, scale, and pushVec (3 floats = 12
// bytes) accumulate further offset drift.  The total size difference is
// 28 bytes (7 extra int/float-sized fields: 1 + 1 + 1 + 1 + 3).
//
// This means you CANNOT cast an sp_entityState_t pointer to an ioEF
// entityState_t pointer and expect fields after modelindex2 to be correct.
// The server-side translation layer (sv_game_sp.c :: SV_SP_SyncToShared)
// copies individual fields to bridge this gap for the engine's internal
// use, while the cgame bridge (cl_cgame_sp.c) hands the raw SP layout
// directly to the SP cgame DLL.
// ============================================================================

typedef struct {
	int		number;			/* entity index in the game's entity array */
	int		eType;			/* entityType_t -- determines how cgame interprets this entity */
	int		eFlags;			/* EF_* bit flags (dead, firing, teleport, etc.) */

	trajectory_t	pos;	/* positional trajectory for interpolation */
	trajectory_t	apos;	/* angular trajectory for interpolation */

	int		time;			/* event timestamp or misc timer */
	int		time2;			/* secondary timer (e.g., for two-phase events) */

	vec3_t	origin;			/* current origin (also baked into pos.trBase) */
	vec3_t	origin2;		/* secondary origin (e.g., mover destination, beam endpoint) */

	vec3_t	angles;			/* current facing angles */
	vec3_t	angles2;		/* secondary angles (e.g., mover angular destination) */

	int		otherEntityNum;	 /* related entity (e.g., beam source, shotgun attacker) */
	int		otherEntityNum2; /* second related entity */

	int		groundEntityNum; /* ENTITYNUM_NONE when airborne */

	int		constantLight;	/* packed RGBA constant light: r + (g<<8) + (b<<16) + (intensity<<24) */
	int		loopSound;		/* handle to continuously looping sound */

	int		modelindex;		/* primary model (world brush model or MD3/MDR index) */
	int		modelindex2;	/* secondary model (e.g., held weapon model) */
	int		modelindex3;	/* SP-specific: third model slot for multi-part SP entities */
	int		clientNum;		/* owning client number (0..MAX_CLIENTS-1); offset shifted +4 from ioEF */
	int		frame;			/* model animation frame */

	int		solid;			/* bounding-box encoding for client-side prediction */

	int		event;			/* impulse event (muzzle flash, footstep, etc.) */
	int		eventParm;		/* parameter for the impulse event */

	int		powerups;		/* bit flags for active powerups */
	int		weapon;			/* weapon index, determines weapon/flash models */
	int		legsAnim;		/* lower-body animation index (mask off ANIM_TOGGLEBIT) */
	int		legsAnimTimer;	/* SP-specific: time remaining in legs anim blend (msec) */
	int		torsoAnim;		/* upper-body animation index (mask off ANIM_TOGGLEBIT) */
	int		torsoAnimTimer;	/* SP-specific: time remaining in torso anim blend (msec) */

	int		scale;			/* SP-specific: entity scale multiplier (fixed-point or int encoding) */

	vec3_t	pushVec;		/* SP-specific: knockback/push direction and magnitude */
} sp_entityState_t;

// ============================================================================
// SP playerState_t
//
// The player state is the largest and most frequently accessed struct at
// the engine/game boundary.  The SP layout diverges from ioEF's in many
// places, reflecting gameplay mechanics that exist only in singleplayer.
//
// KEY DIFFERENCES FROM ioEF playerState_t:
//
// Missing fields (present in ioEF, absent in SP):
//   introTime            -- ioEF MP uses this for holodoor intros; SP does
//                           not have this field at all, so every field after
//                           useTime is shifted relative to the ioEF layout
//   damageShieldCount    -- ioEF MP tracks personal shield damage separately;
//                           SP handles shields through the stats[] array
//   entityEventSequence  -- ioEF MP uses this for reliable event delivery;
//                           SP omits it (it was added after SP shipped)
//
// Added fields (present in SP, absent in ioEF):
//   leanofs              -- signed char: how far the player is leaning left
//                           or right.  The SP campaign has a lean mechanic
//                           (peek around corners) that doesn't exist in MP.
//   friction             -- short: per-frame friction override.  SP uses
//                           this for surfaces like ice or low-gravity areas
//                           where the default friction doesn't apply.
//   legsAnimTimer        -- timed blend for lower-body animation transitions
//   torsoAnimTimer       -- timed blend for upper-body animation transitions
//   scale                -- player model scale (used in cinematics)
//   borgAdaptHits[32]    -- per-weapon Borg adaptation tracking.  In the SP
//                           campaign, Borg enemies adapt to weapons that hit
//                           them repeatedly.  This array records how many
//                           times each weapon has struck an adapted Borg,
//                           affecting damage reduction.  (MAX_WEAPONS=32 in
//                           SP, vs MAX_WEAPONS=16 in ioEF.)
//   pushVec              -- directional push/knockback vector, used for
//                           physics effects like explosions pushing the player
//   leanStopDebounceTime -- unsigned char: debounce timer to prevent rapid
//                           lean on/off toggling
//
// Array size differences:
//   events[2] / eventParms[2]  -- SP uses MAX_PS_EVENTS=2; ioEF MP uses 4.
//                                 This shifts all fields after eventParms.
//   ammo[4]                    -- SP uses MAX_AMMO=4 (ammo types are pooled
//                                 differently than MP's per-weapon ammo).
//                                 ioEF uses ammo[MAX_WEAPONS=16].
//
// ALIGNMENT NOTES:
//   The leanofs (signed char) and friction (short) fields create alignment
//   padding that is critical to get right.  See the inline comments below
//   for the exact padding layout.  The original MSVC compiler inserted
//   padding to align each field to its natural boundary.
// ============================================================================

typedef struct {
	int			commandTime;	/* cmd->serverTime of last executed command */
	int			pm_type;		/* PM_NORMAL, PM_DEAD, PM_SPECTATOR, etc. */
	int			bobCycle;		/* view bobbing and footstep generation cycle */
	int			pm_flags;		/* PMF_DUCKED, PMF_JUMP_HELD, etc. */
	int			pm_time;		/* timer for pm_flags effects (e.g., waterjump) */

	vec3_t		origin;			/* player world position */
	vec3_t		velocity;		/* current velocity vector */
	int			weaponTime;		/* time until weapon can fire again (msec) */
	int			rechargeTime;	/* phaser recharge timer -- EF-specific weapon mechanic */
	short		useTime;		/* use-key debounce timer */
	// --- 2 bytes implicit padding here (MSVC aligns int to 4-byte boundary) ---
	int			gravity;		/* current gravity (can be overridden per-area) */
	signed char	leanofs;		/* SP-specific: lean offset, negative=left, positive=right */
	// --- 1 byte implicit padding here (MSVC aligns short to 2-byte boundary) ---
	short		friction;		/* SP-specific: per-frame friction override for special surfaces */
	// --- friction (2 bytes) + padding (1+1) ends exactly on a 4-byte boundary ---
	int			speed;			/* player movement speed */
	int			delta_angles[3]; /* added to usercmd angles for view direction after teleports/spawns */

	int			groundEntityNum; /* entity we're standing on, ENTITYNUM_NONE if airborne */
	int			legsAnim;		/* lower-body animation (mask off ANIM_TOGGLEBIT to get index) */
	int			legsAnimTimer;	/* SP-specific: timed blend for legs animation transitions */
	int			torsoAnim;		/* upper-body animation (mask off ANIM_TOGGLEBIT to get index) */
	int			torsoAnimTimer;	/* SP-specific: timed blend for torso animation transitions */
	int			scale;			/* SP-specific: player model scale (for cinematic effects) */
	int			movementDir;	/* 0-7 compass direction relative to view angle */

	int			eFlags;			/* entity flags copied to entityState_t->eFlags */

	int			eventSequence;	/* increments with each new pmove-generated event */
	int			events[2];		/* SP MAX_PS_EVENTS = 2 (ioEF MP uses 4) */
	int			eventParms[2];	/* parameters for the above events */

	int			externalEvent;		/* event set on the player by an external source */
	int			externalEventParm;	/* parameter for external event */
	int			externalEventTime;	/* SP-specific: timestamp of the external event */

	int			clientNum;		/* this player's client slot (0..MAX_CLIENTS-1) */
	int			weapon;			/* current weapon index */
	int			weaponstate;	/* WS_IDLE, WS_FIRING, WS_RELOADING, etc. */

	vec3_t		viewangles;		/* current view angles (for fixed/locked views) */
	int			viewheight;		/* eye offset above origin (changes when crouching) */

	/* Damage feedback -- when damageEvent changes, cgame latches the other parms
	   to drive screen-flash and directional-damage indicators */
	int			damageEvent;
	int			damageYaw;		/* direction the damage came from (yaw angle) */
	int			damagePitch;	/* direction the damage came from (pitch angle) */
	int			damageCount;	/* amount of damage taken (drives screen flash intensity) */
	/* Note: ioEF MP has damageShieldCount here; SP does not track it separately */

	int			stats[16];		/* MAX_STATS: health, armor, etc. */
	int			persistant[16];	/* MAX_PERSISTANT: stats surviving death (score, kills, etc.) */
	int			powerups[16];	/* MAX_POWERUPS: level.time when each powerup expires */
	int			ammo[4];		/* SP MAX_AMMO = 4: ammo is pooled by type, not per-weapon.
								   (ioEF uses ammo[16] indexed by weapon number) */
	int			borgAdaptHits[32]; /* SP-specific, MAX_WEAPONS = 32:
								   Tracks per-weapon adaptation state for Borg enemies.
								   When a weapon is used repeatedly against Borg, they
								   adapt and take reduced damage.  This array drives
								   that mechanic and the cgame's "weapon ineffective"
								   HUD indicator.  The index is the weapon number. */

	vec3_t		pushVec;		/* SP-specific: accumulated knockback/push direction */

	/* ---- fields below this line are NOT communicated over the network ---- */
	int			ping;			/* server to cgame info, used for scoreboard display */
	unsigned char	leanStopDebounceTime; /* SP-specific: prevents rapid lean toggling */
	/* Note: ioEF MP has entityEventSequence here; SP does not use it */
} sp_playerState_t;

// ============================================================================
// SP snapshot_t  (cgame API)
//
// WHY THIS EXISTS:
//
// The cgame DLL receives a snapshot_t pointer from the engine each frame
// via the CG_GETSNAPSHOT syscall.  It then reads the playerState and
// entity array by field offset.  Because sp_playerState_t and
// sp_entityState_t are different sizes from their ioEF counterparts,
// an ioEF-layout snapshot_t would place entities at the wrong byte
// offset -- the cgame would read garbage.
//
// For example, ioEF's snapshot_t has playerState_t at offset ~20, and
// entities[] starts at offset ~(20 + sizeof(playerState_t) + 4).  The
// SP cgame expects sp_playerState_t at that location, which is a
// different size, so entities[] would start at a completely different
// offset.  Every field read from entities onward would be wrong.
//
// This struct replicates the exact snapshot_t layout the SP cgame DLL
// expects, with the correct embedded types and extra fields.
//
// EXTRA FIELDS vs ioEF snapshot_t:
//
//   cmdNum                  -- the usercmd number associated with this
//                              snapshot; the SP cgame uses this for
//                              prediction reconciliation
//   numConfigstringChanges  -- count of configstrings that changed in
//   configstringNum            this snapshot; the SP cgame uses these
//                              to incrementally update its configstring
//                              cache without rescanning all strings
//
// The cl_cgame_sp.c bridge builds this struct manually each frame
// instead of going through ioEF's normal delta-compression snapshot
// path, because the delta compressor operates on ioEF-layout entities
// and would produce garbage when decompressed into SP-layout fields.
// ============================================================================

#define SP_MAX_ENTITIES_IN_SNAPSHOT	256

typedef struct {
	int				snapFlags;			/* SNAPFLAG_RATE_DELAYED, etc. */
	int				ping;				/* round-trip time to server */

	int				serverTime;			/* server time this snapshot is valid for (msec) */

	byte			areamask[MAX_MAP_AREA_BYTES]; /* PVS area visibility bits */

	int				cmdNum;				/* SP-specific: usercmd number for prediction */

	sp_playerState_t	ps;				/* full player state in SP layout */

	int				numEntities;		/* count of entities in the snapshot */
	sp_entityState_t	entities[SP_MAX_ENTITIES_IN_SNAPSHOT]; /* entity states in SP layout */

	int				numConfigstringChanges;	/* SP-specific: how many configstrings changed */
	int				configstringNum;		/* SP-specific: starting configstring index */

	int				numServerCommands;		/* text-based server commands to execute */
	int				serverCommandSequence;	/* sequence number for server command tracking */
} sp_snapshot_t;

// ============================================================================
// SP gentity_t  (server-visible portion)
//
// CRITICAL: WHY THIS USES sp_entityState_t
//
// Every gentity_t in the Quake 3 engine family starts with an entityState_t
// as its first member ('s').  The engine iterates the game module's entity
// array using pointer arithmetic:
//
//     ent = (gentity_t *)( (byte *)gentities + gentitySize * index );
//     ent->s.number  ... ent->inuse ... ent->linked ...
//
// Because sp_entityState_t is LARGER than ioEF's entityState_t (by 28
// bytes), every field after 's' in the struct -- client, inuse, linked,
// svFlags, bmodel, mins, maxs, etc. -- sits at a different byte offset
// than it would in an ioEF gentity_t.
//
// If we declared 's' as the ioEF entityState_t here, then ent->client
// would read 28 bytes too early, ent->inuse would be wrong, and the
// engine would misidentify which entities are active, which are linked
// into the world, etc.  This would cause entities to vanish, collide
// incorrectly, or crash.
//
// SCOPE OF THIS STRUCT:
//
// We only define the engine-visible prefix of gentity_t here -- the
// fields the engine needs to read for collision, PVS checks, and entity
// management.  The SP game module has many more fields after 'owner'
// (AI state, script variables, etc.), but the engine never touches them.
// The gentitySize stride from the game module ensures we skip over
// those private fields correctly when iterating.
// ============================================================================

typedef struct sp_gentity_s sp_gentity_t;
struct sp_gentity_s {
	sp_entityState_t	s;		/* MUST be sp_entityState_t, not entityState_t --
								   see comment block above for why this matters */
	struct sp_gclient_s	*client; /* pointer to this entity's gclient_t, or NULL for non-players */
	qboolean		inuse;		/* qtrue if this entity slot is active */
	qboolean		linked;		/* qtrue if linked into the world (collision/PVS) */
	int			svFlags;		/* SVF_* flags (noclient, broadcast, singleclient, etc.) */
	qboolean		bmodel;		/* qtrue if this entity is a brush model (door, platform, etc.) */
	vec3_t			mins, maxs;	/* local bounding box (relative to origin) */
	int			contents;		/* CONTENTS_* flags for collision filtering */
	vec3_t			absmin, absmax; /* world-space bounding box, computed by trap_LinkEntity */
	vec3_t			currentOrigin;	/* actual current position (may differ from s.origin during interpolation) */
	vec3_t			currentAngles;	/* actual current angles */
	sp_gentity_t		*owner;	/* owning entity (e.g., projectile -> shooter) */
	/* Game-private fields follow in the SP DLL's actual gentity_t, but the
	   engine never accesses them.  The ge->gentitySize stride ensures correct
	   pointer arithmetic when iterating past this point. */
};

// ============================================================================
// SP usercmd_t
//
// The SP game module's usercmd_t has `int buttons` (4 bytes) instead of
// ioEF's `byte buttons` (1 byte).  This 3-byte difference shifts the
// weapon, angles, and movement fields to different offsets.  The engine
// must translate usercmd_t before passing it to ge->ClientThink/ClientBegin.
// ============================================================================

typedef struct {
	int			serverTime;
	int			buttons;		// int in SP, byte in ioEF
	byte		weapon;
	int			angles[3];
	signed char	forwardmove, rightmove, upmove;
} sp_usercmd_t;

#endif /* SP_TYPES_H */
