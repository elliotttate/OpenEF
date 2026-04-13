// cg_gi_compat.cpp - Compatibility layer for cgame code that references gi.*
// When cgame is built as a separate DLL, it can't access the game module's gi global.
// This provides a stub gi with the functions/data the cgame actually uses.
// In combined DLL mode (SP_GAME), the real gi from g_main.cpp is used instead.

#ifdef SP_GAME
// Nothing needed - game module provides the real gi global
#else
#include "../game/g_shared.h"

// Stub S_Override array - cgame reads this for voice volume visualization
static int s_override_stub[MAX_CLIENTS + 1];

// Stub cvar function - wraps cgi_Cvar_Register
extern void cgi_Cvar_Register(vmCvar_t *vmCvar, const char *varName, const char *defaultValue, int flags);

static cvar_t cvar_stub_pool[64];
static int cvar_stub_count = 0;

static cvar_t *gi_cvar_stub(const char *var_name, const char *value, int flags) {
	// Return a static cvar_t that has the name and default value
	if (cvar_stub_count >= 64) cvar_stub_count = 0;
	cvar_t *cv = &cvar_stub_pool[cvar_stub_count++];
	memset(cv, 0, sizeof(*cv));
	Q_strncpyz(cv->string, value ? value : "", sizeof(cv->string));
	cv->value = atof(cv->string);
	cv->integer = atoi(cv->string);
	return cv;
}

// Stub trace function - wraps cgi_CM_BoxTrace
extern void cgi_CM_BoxTrace(trace_t *results, const vec3_t start, const vec3_t end,
	const vec3_t mins, const vec3_t maxs, clipHandle_t model, int brushmask);

static void gi_trace_stub(trace_t *results, const vec3_t start, const vec3_t mins,
	const vec3_t maxs, const vec3_t end, int passEntityNum, int contentmask) {
	cgi_CM_BoxTrace(results, start, end, mins, maxs, 0, contentmask);
}

// Global gi - only the fields that cgame code actually references
game_import_t gi = {0};

// Called early in cgame init to set up the gi stub
void CG_InitGiCompat(void) {
	gi.cvar = gi_cvar_stub;
	gi.trace = gi_trace_stub;
	gi.S_Override = s_override_stub;
}
#endif // !SP_GAME
