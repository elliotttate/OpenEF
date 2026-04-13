// qfiles_q3bsp.h -- Q3/EF BSP format structs (version 46)
// These differ from JKA's BSP version 1 structs in qfiles.h:
// - drawVert_t has 1 lightmap instead of 4
// - dbrushside_t has no drawSurfNum
// - dsurface_t has 1 lightmapNum instead of 4, no lightmapStyles
// - dleaf_t layout matches q3
// - dgrid_t has 1 light instead of 4
#ifndef QFILES_Q3BSP_H
#define QFILES_Q3BSP_H

#define Q3_BSP_VERSION 46
#define Q3_HEADER_LUMPS 17  // Q3 has 17 lumps, JKA has 18 (added LUMP_LIGHTARRAY)

typedef struct {
	vec3_t		xyz;
	float		st[2];
	float		lightmap[2];  // Q3: single lightmap channel (JKA has [4][2])
	vec3_t		normal;
	byte		color[4];     // Q3: single color (JKA has [4][4])
} q3drawVert_t;

typedef struct {
	int			planeNum;
	int			shaderNum;
	// Q3 has no drawSurfNum field (JKA adds it)
} q3dbrushside_t;

typedef struct {
	int			shaderNum;
	int			fogNum;
	int			surfaceType;
	int			firstVert;
	int			numVerts;
	int			firstIndex;
	int			numIndexes;
	int			lightmapNum;     // Q3: single lightmap (JKA has [4])
	int			lightmapX, lightmapY;
	int			lightmapWidth, lightmapHeight;
	vec3_t		lightmapOrigin;
	vec3_t		lightmapVecs[3];
	int			patchWidth, patchHeight;
} q3dsurface_t;

typedef struct {
	byte		ambientLight[3]; // Q3: single light (JKA has [4][3])
	byte		directLight[3];
	byte		latLong[2];
} q3dgrid_t;

#endif
