#include "mb_main.h"
#include "mb_data.h"
#include "display_funcs.h"

#include "segment_symbols.h"

#include "actors/b/header.h"
#include "actors/maker/header.h"
#include "behavior_data.h"

#include "actors/group0.h"
#include "actors/common0.h"
#include "actors/group14.h"
#include "actors/group17.h"

f32 MBCameraZoomTable[][2] = {
    {900.0f,50.0f},
    {900.0f,400.0f},
    {1500.0f,800.0f},
    {2000.0f,900.0f},
    {2100.0f,1500.0f},
};

void *mb_theme_segments[][4] = {
    {_group17_yay0SegmentRomStart, _group17_yay0SegmentRomEnd, _group17_geoSegmentRomStart, _group17_geoSegmentRomEnd}, //MB_GAME_VANILLA
    {_group14_yay0SegmentRomStart, _group14_yay0SegmentRomEnd, _group14_geoSegmentRomStart, _group14_geoSegmentRomEnd}, //MB_GAME_BTCM
};

LevelScript *mb_theme_model_scripts[] = {
    script_func_mb_theme_vanilla,
    script_func_mb_theme_btcm,
};


// lookup table for rotating blocks
u8 mb_rotated_dirs[4][6] = {
    {MB_DIRECTION_UP, MB_DIRECTION_DOWN, MB_DIRECTION_POS_X, MB_DIRECTION_NEG_X, MB_DIRECTION_POS_Z, MB_DIRECTION_NEG_Z},
    {MB_DIRECTION_UP, MB_DIRECTION_DOWN, MB_DIRECTION_NEG_Z, MB_DIRECTION_POS_Z, MB_DIRECTION_POS_X, MB_DIRECTION_NEG_X},
    {MB_DIRECTION_UP, MB_DIRECTION_DOWN, MB_DIRECTION_NEG_X, MB_DIRECTION_POS_X, MB_DIRECTION_NEG_Z, MB_DIRECTION_POS_Z},
    {MB_DIRECTION_UP, MB_DIRECTION_DOWN, MB_DIRECTION_POS_Z, MB_DIRECTION_NEG_Z, MB_DIRECTION_NEG_X, MB_DIRECTION_POS_X},
};

struct MBTerrainPoly mb_terrain_fullblock_quads[] = {
    {{{16, 16, 16}, {16, 16, 0}, {0, 16, 16},  {0, 16, 0}},  MB_DIRECTION_UP,    MB_FACESHAPE_FULL, MB_GROWTH_FULL, NULL}, // TOP
    {{{16, 0, 16},  {0, 0, 16},  {16, 0, 0},   {0, 0, 0}},   MB_DIRECTION_DOWN,  MB_FACESHAPE_FULL, 0, NULL}, // BOTTOM
    {{{16, 16, 16}, {16, 0, 16}, {16, 16, 0},  {16, 0, 0}},  MB_DIRECTION_POS_X, MB_FACESHAPE_FULL, MB_GROWTH_NORMAL_SIDE, NULL}, // LEFT
    {{{0, 16, 0},   {0, 0, 0},   {0, 16, 16},  {0, 0, 16}},  MB_DIRECTION_NEG_X, MB_FACESHAPE_FULL, MB_GROWTH_NORMAL_SIDE, NULL}, // RIGHT
    {{{0, 16, 16},  {0, 0, 16},  {16, 16, 16}, {16, 0, 16}}, MB_DIRECTION_POS_Z, MB_FACESHAPE_FULL, MB_GROWTH_NORMAL_SIDE, NULL}, // FRONT
    {{{16, 16, 0},  {16, 0, 0},  {0, 16, 0},   {0, 0, 0}},   MB_DIRECTION_NEG_Z, MB_FACESHAPE_FULL, MB_GROWTH_NORMAL_SIDE, NULL}, // BACK
};
struct MBTerrain mb_terrain_fullblock = {
    6,
    0,
    mb_terrain_fullblock_quads,
    NULL
};

s8 slope_decal_uvs1[3][2] = {{0, 0}, {0, 16}, {16, 16}};
s8 slope_decal_uvs2[3][2] = {{16, 16}, {16, 0}, {0, 16}};

struct MBTerrainPoly mb_terrain_slope_quads[] = {
    {{{16, 0, 16}, {16, 16, 0}, {0, 0, 16}, {0, 16, 0}}, MB_DIRECTION_UP,    MB_FACESHAPE_EMPTY_2, MB_GROWTH_FULL, NULL}, // TOP
    {{{16, 0, 16}, {0, 0, 16},  {16, 0, 0}, {0, 0, 0}},  MB_DIRECTION_DOWN,  MB_FACESHAPE_FULL, 0, NULL}, // BOTTOM
    {{{16, 16, 0}, {16, 0, 0},  {0, 16, 0}, {0, 0, 0}},  MB_DIRECTION_NEG_Z, MB_FACESHAPE_FULL, MB_GROWTH_NORMAL_SIDE, NULL}, // BACK
};

struct MBTerrainPoly mb_terrain_slope_tris[] = {
    {{{16, 0, 0}, {16, 16, 0}, {16, 0, 16}}, MB_DIRECTION_POS_X, MB_FACESHAPE_TRI_1, MB_GROWTH_SLOPE_SIDE_L, &slope_decal_uvs1}, // LEFT
    {{{0, 16, 0}, {0, 0, 0},   {0, 0, 16}},  MB_DIRECTION_NEG_X, MB_FACESHAPE_TRI_2, MB_GROWTH_SLOPE_SIDE_R, &slope_decal_uvs2}, // RIGHT
};

struct MBTerrain mb_terrain_slope = {
    3,
    2,
    mb_terrain_slope_quads,
    mb_terrain_slope_tris,
};

struct MBTerrainPoly mb_terrain_dslope_quads[] = {
    {{{16, 16, 16}, {16, 16, 0}, {0, 16, 16}, {0, 16, 0}}, MB_DIRECTION_UP,    MB_FACESHAPE_FULL, MB_GROWTH_FULL, NULL}, // TOP
    {{{16, 16, 0},  {16, 0, 0},  {0, 16, 0},  {0, 0, 0}},  MB_DIRECTION_NEG_Z, MB_FACESHAPE_FULL, MB_GROWTH_NORMAL_SIDE, NULL}, // BACK
    {{{16, 16, 16}, {0, 16, 16}, {16, 0, 0},  {0, 0, 0}},  MB_DIRECTION_DOWN,  MB_FACESHAPE_EMPTY_2, MB_GROWTH_NORMAL_SIDE, NULL}, // BOTTOM
};

struct MBTerrainPoly mb_terrain_dslope_tris[] = {
    {{{16, 0, 0}, {16, 16, 0}, {16, 16, 16}},  MB_DIRECTION_POS_X, MB_FACESHAPE_DOWNTRI_1, MB_GROWTH_NORMAL_SIDE, NULL}, // LEFT
    {{{0, 16, 0}, {0, 0, 0},   {0, 16, 16}},   MB_DIRECTION_NEG_X, MB_FACESHAPE_DOWNTRI_2, MB_GROWTH_NORMAL_SIDE, NULL}, // RIGHT
};

struct MBTerrain mb_terrain_dslope = {
    3,
    2,
    mb_terrain_dslope_quads,
    mb_terrain_dslope_tris,
};

struct MBTerrainPoly mb_terrain_corner_quads[] = {
    {{{16, 0, 16}, {0, 0, 16}, {16, 0, 0}, {0, 0, 0}}, MB_DIRECTION_DOWN, MB_FACESHAPE_FULL, 0, NULL}, // BOTTOM
};

struct MBTerrainPoly mb_terrain_corner_tris[] = {
    {{{0, 0, 16}, {16, 0, 16}, {0, 16, 0}},  MB_DIRECTION_UP,    MB_FACESHAPE_EMPTY_2, MB_GROWTH_FULL, NULL}, // TOP-FRONT
    {{{0, 16, 0}, {16, 0, 16}, {16, 0, 0}},  MB_DIRECTION_UP,    MB_FACESHAPE_EMPTY_0, MB_GROWTH_FULL, NULL}, // TOP-LEFT 
    {{{0, 16, 0}, {0, 0, 0},   {0, 0, 16}},  MB_DIRECTION_NEG_X, MB_FACESHAPE_TRI_2, MB_GROWTH_SLOPE_SIDE_R, &slope_decal_uvs2}, // RIGHT
    {{{0, 0, 0},  {0, 16, 0},  {16, 0, 0}},  MB_DIRECTION_NEG_Z, MB_FACESHAPE_TRI_1, MB_GROWTH_SLOPE_SIDE_L, &slope_decal_uvs1}, // BACK
};

struct MBTerrain mb_terrain_corner = {
    1,
    4,
    mb_terrain_corner_quads,
    mb_terrain_corner_tris,
};

struct MBTerrainPoly mb_terrain_dcorner_quads[] = {
    {{{16, 16, 16}, {16, 16, 0}, {0, 16, 16}, {0, 16, 0}}, MB_DIRECTION_UP, MB_FACESHAPE_FULL, MB_GROWTH_FULL, NULL}, // TOP
};

struct MBTerrainPoly mb_terrain_dcorner_tris[] = {
    {{{16, 16, 16}, {0, 16, 16}, {0, 0, 0}},    MB_DIRECTION_DOWN, MB_FACESHAPE_EMPTY_2, MB_GROWTH_NORMAL_SIDE, NULL}, // BOTTOM-FRONT
    {{{0, 0, 0},    {16, 16, 0}, {16, 16, 16}}, MB_DIRECTION_DOWN, MB_FACESHAPE_EMPTY_0, MB_GROWTH_NORMAL_SIDE, NULL}, // BOTTOM-LEFT 
    {{{0, 0, 0},    {0, 16, 16}, {0, 16, 0}},   MB_DIRECTION_NEG_X, MB_FACESHAPE_DOWNTRI_2, MB_GROWTH_NORMAL_SIDE, NULL}, // RIGHT
    {{{0, 0, 0},    {0, 16, 0},  {16, 16, 0}},  MB_DIRECTION_NEG_Z, MB_FACESHAPE_DOWNTRI_1, MB_GROWTH_NORMAL_SIDE, NULL}, // BACK
};

struct MBTerrain mb_terrain_dcorner = {
    1,
    4,
    mb_terrain_dcorner_quads,
    mb_terrain_dcorner_tris,
};


struct MBTerrainPoly mb_terrain_icorner_quads[] = {
    {{{16, 0, 16}, {0, 0, 16}, {16, 0, 0},  {0, 0, 0}},  MB_DIRECTION_DOWN,  MB_FACESHAPE_FULL, 0, NULL}, // BOTTOM
    {{{16, 16, 0}, {16, 0, 0}, {0, 16, 0},  {0, 0, 0}},  MB_DIRECTION_NEG_Z, MB_FACESHAPE_FULL, MB_GROWTH_NORMAL_SIDE, NULL}, // BACK
    {{{0, 16, 0},  {0, 0, 0},  {0, 16, 16}, {0, 0, 16}}, MB_DIRECTION_NEG_X, MB_FACESHAPE_FULL, MB_GROWTH_NORMAL_SIDE, NULL}, // RIGHT
};

struct MBTerrainPoly mb_terrain_icorner_tris[] = {
    {{{0, 16, 16}, {16, 0, 16}, {0, 16, 0}},  MB_DIRECTION_UP,    MB_FACESHAPE_EMPTY_0, MB_GROWTH_FULL, NULL}, // TOP-FRONT
    {{{0, 16, 0},  {16, 0, 16}, {16, 16, 0}}, MB_DIRECTION_UP,    MB_FACESHAPE_EMPTY_2, MB_GROWTH_FULL, NULL}, // TOP-LEFT
    {{{16, 0, 0},  {16, 16, 0}, {16, 0, 16}}, MB_DIRECTION_POS_X, MB_FACESHAPE_TRI_1, MB_GROWTH_SLOPE_SIDE_L, &slope_decal_uvs1}, // LEFT
    {{{0, 16, 16}, {0, 0, 16},  {16, 0, 16}}, MB_DIRECTION_POS_Z, MB_FACESHAPE_TRI_2, MB_GROWTH_SLOPE_SIDE_R, &slope_decal_uvs2}, // FRONT
};

struct MBTerrain mb_terrain_icorner = {
    3,
    4,
    mb_terrain_icorner_quads,
    mb_terrain_icorner_tris,
};

struct MBTerrainPoly mb_terrain_dicorner_quads[] = {
    {{{16, 16, 16}, {16, 16, 0}, {0, 16, 16}, {0, 16, 0}}, MB_DIRECTION_UP,    MB_FACESHAPE_FULL, MB_GROWTH_FULL, NULL}, // TOP
    {{{16, 16, 0},  {16, 0, 0},  {0, 16, 0},  {0, 0, 0}},  MB_DIRECTION_NEG_Z, MB_FACESHAPE_FULL, MB_GROWTH_NORMAL_SIDE, NULL}, // BACK
    {{{0, 16, 0},   {0, 0, 0},   {0, 16, 16}, {0, 0, 16}}, MB_DIRECTION_NEG_X, MB_FACESHAPE_FULL, MB_GROWTH_NORMAL_SIDE, NULL}, // RIGHT
};

struct MBTerrainPoly mb_terrain_dicorner_tris[] = {
    {{{16, 0, 0},   {16, 16, 0}, {16, 16, 16}}, MB_DIRECTION_POS_X, MB_FACESHAPE_DOWNTRI_1, MB_GROWTH_NORMAL_SIDE, NULL}, // LEFT
    {{{0, 16, 16},  {0, 0, 16},  {16, 16, 16}}, MB_DIRECTION_POS_Z, MB_FACESHAPE_DOWNTRI_2, MB_GROWTH_NORMAL_SIDE, NULL}, // FRONT
    {{{16, 16, 16}, {0, 0, 16},  {0, 0, 0}},    MB_DIRECTION_DOWN, MB_FACESHAPE_EMPTY_0, MB_GROWTH_UNDERSLOPE_CORNER, NULL}, // BOTTOM-FRONT
    {{{16, 16, 16}, {0, 0, 0},   {16, 0, 0}},   MB_DIRECTION_DOWN, MB_FACESHAPE_EMPTY_2, MB_GROWTH_UNDERSLOPE_CORNER, NULL}, // BOTTOM-LEFT
};

struct MBTerrain mb_terrain_dicorner = {
    3,
    4,
    mb_terrain_dicorner_quads,
    mb_terrain_dicorner_tris,
};

struct MBTerrainPoly mb_terrain_sslope_quads[] = {
    {{{16, 16, 0}, {16, 0, 0},  {0, 16, 0},  {0, 0, 0}},  MB_DIRECTION_NEG_Z, MB_FACESHAPE_FULL, MB_GROWTH_NORMAL_SIDE, NULL}, // BACK
    {{{0, 16, 0},  {0, 0, 0},   {0, 16, 16}, {0, 0, 16}}, MB_DIRECTION_NEG_X, MB_FACESHAPE_FULL, MB_GROWTH_NORMAL_SIDE, NULL}, // RIGHT
    {{{16, 16, 0}, {0, 16, 16}, {16, 0, 0},  {0, 0, 16}}, MB_DIRECTION_POS_Z, MB_FACESHAPE_EMPTY, MB_GROWTH_DIAGONAL_SIDE, NULL}, // DIAGONAL
};

struct MBTerrainPoly mb_terrain_sslope_tris[] = {
    {{{0, 16, 16}, {16, 16, 0}, {0, 16, 0}}, MB_DIRECTION_UP,   MB_FACESHAPE_TOPTRI, MB_GROWTH_FULL, NULL}, // TOP
    {{{16, 0, 0},  {0, 0, 16},  {0, 0, 0}},  MB_DIRECTION_DOWN, MB_FACESHAPE_TOPTRI, 0, NULL}, // BOTTOM
};

struct MBTerrain mb_terrain_sslope = {
    3,
    2,
    mb_terrain_sslope_quads,
    mb_terrain_sslope_tris,
};

s8 scorner_slope_tri_uvs[3][2] = {{16, 0}, {0, 0}, {8, 16}};

struct MBTerrainPoly mb_terrain_scorner_tris[] = {
    {{{0, 0, 16}, {16, 0, 0}, {0, 16, 0}},  MB_DIRECTION_UP,    MB_FACESHAPE_EMPTY_2, MB_GROWTH_FULL, &scorner_slope_tri_uvs}, // TOP
    {{{16, 0, 0},  {0, 0, 16},  {0, 0, 0}}, MB_DIRECTION_DOWN, MB_FACESHAPE_TOPTRI, 0, NULL}, // BOTTOM
    {{{0, 16, 0}, {0, 0, 0},   {0, 0, 16}}, MB_DIRECTION_NEG_X, MB_FACESHAPE_TRI_2, MB_GROWTH_SLOPE_SIDE_R, &slope_decal_uvs2}, // RIGHT
    {{{0, 0, 0},  {0, 16, 0},  {16, 0, 0}}, MB_DIRECTION_NEG_Z, MB_FACESHAPE_TRI_1, MB_GROWTH_SLOPE_SIDE_L, &slope_decal_uvs1}, // BACK
};

struct MBTerrain mb_terrain_scorner = {
    0,
    4,
    NULL,
    mb_terrain_scorner_tris,
};

s8 dscorner_slope_tri_uvs[3][2] = {{0, 16}, {16, 16}, {8, 0}};

struct MBTerrainPoly mb_terrain_dscorner_tris[] = {
    {{{16, 16, 0}, {0, 16, 16}, {0, 0, 0}},    MB_DIRECTION_DOWN, MB_FACESHAPE_EMPTY_2, MB_GROWTH_DIAGONAL_SIDE, &dscorner_slope_tri_uvs}, // BOTTOM
    {{{0, 16, 16}, {16, 16, 0}, {0, 16, 0}},   MB_DIRECTION_UP,   MB_FACESHAPE_TOPTRI, MB_GROWTH_FULL, NULL}, // TOP
    {{{0, 0, 0},    {0, 16, 16}, {0, 16, 0}},  MB_DIRECTION_NEG_X, MB_FACESHAPE_DOWNTRI_2, MB_GROWTH_NORMAL_SIDE, NULL}, // RIGHT
    {{{0, 0, 0},    {0, 16, 0},  {16, 16, 0}}, MB_DIRECTION_NEG_Z, MB_FACESHAPE_DOWNTRI_1, MB_GROWTH_NORMAL_SIDE, NULL}, // BACK
};

struct MBTerrain mb_terrain_dscorner = {
    0,
    4,
    NULL,
    mb_terrain_dscorner_tris,
};

s8 iscorner_slope_tri_uvs[3][2] = {{8, 16}, {0, 0}, {-8, 16}};

struct MBTerrainPoly mb_terrain_iscorner_quads[] = {
    {{{16, 0, 16}, {0, 0, 16}, {16, 0, 0},  {0, 0, 0}},  MB_DIRECTION_DOWN,  MB_FACESHAPE_FULL, 0, NULL}, // BOTTOM
    {{{16, 16, 0}, {16, 0, 0}, {0, 16, 0},  {0, 0, 0}},  MB_DIRECTION_NEG_Z, MB_FACESHAPE_FULL, MB_GROWTH_NORMAL_SIDE, NULL}, // BACK
    {{{0, 16, 0},  {0, 0, 0},  {0, 16, 16}, {0, 0, 16}}, MB_DIRECTION_NEG_X, MB_FACESHAPE_FULL, MB_GROWTH_NORMAL_SIDE, NULL}, // RIGHT
};

struct MBTerrainPoly mb_terrain_iscorner_tris[] = {
    {{{0, 16, 16}, {16, 16, 0}, {0, 16, 0}}, MB_DIRECTION_UP,   MB_FACESHAPE_TOPTRI, MB_GROWTH_FULL, NULL}, // TOP
    {{{0, 16, 16},  {16, 0, 16}, {16, 16, 0}}, MB_DIRECTION_UP,    MB_FACESHAPE_EMPTY_2, MB_GROWTH_FULL, &iscorner_slope_tri_uvs}, // TOP SLOPE
    {{{16, 0, 0},  {16, 16, 0}, {16, 0, 16}}, MB_DIRECTION_POS_X, MB_FACESHAPE_TRI_1, MB_GROWTH_SLOPE_SIDE_L, &slope_decal_uvs1}, // LEFT
    {{{0, 16, 16}, {0, 0, 16},  {16, 0, 16}}, MB_DIRECTION_POS_Z, MB_FACESHAPE_TRI_2, MB_GROWTH_SLOPE_SIDE_R, &slope_decal_uvs2}, // FRONT
};

struct MBTerrain mb_terrain_iscorner = {
    3,
    4,
    mb_terrain_iscorner_quads,
    mb_terrain_iscorner_tris,
};

s8 discorner_slope_tri_uvs[3][2] = {{8, 0}, {-8, 0}, {0, 16}};

struct MBTerrainPoly mb_terrain_discorner_quads[] = {
    {{{16, 16, 16}, {16, 16, 0}, {0, 16, 16}, {0, 16, 0}}, MB_DIRECTION_UP,    MB_FACESHAPE_FULL, MB_GROWTH_FULL, NULL}, // TOP
    {{{16, 16, 0},  {16, 0, 0},  {0, 16, 0},  {0, 0, 0}},  MB_DIRECTION_NEG_Z, MB_FACESHAPE_FULL, MB_GROWTH_NORMAL_SIDE, NULL}, // BACK
    {{{0, 16, 0},   {0, 0, 0},   {0, 16, 16}, {0, 0, 16}}, MB_DIRECTION_NEG_X, MB_FACESHAPE_FULL, MB_GROWTH_NORMAL_SIDE, NULL}, // RIGHT
};

struct MBTerrainPoly mb_terrain_discorner_tris[] = {
    {{{16, 0, 0},   {16, 16, 0}, {16, 16, 16}}, MB_DIRECTION_POS_X, MB_FACESHAPE_DOWNTRI_1, MB_GROWTH_NORMAL_SIDE, NULL}, // LEFT
    {{{0, 16, 16},  {0, 0, 16},  {16, 16, 16}}, MB_DIRECTION_POS_Z, MB_FACESHAPE_DOWNTRI_2, MB_GROWTH_NORMAL_SIDE, NULL}, // FRONT
    {{{16, 0, 0},  {0, 0, 16},  {0, 0, 0}},  MB_DIRECTION_DOWN, MB_FACESHAPE_TOPTRI, 0, NULL}, // BOTTOM
    {{{0, 0, 16},   {16, 0, 0}, {16, 16, 16}},   MB_DIRECTION_DOWN, MB_FACESHAPE_EMPTY_2, MB_GROWTH_UNDERSLOPE_CORNER, &discorner_slope_tri_uvs}, // BOTTOM SLOPE
};

struct MBTerrain mb_terrain_discorner = {
    3,
    4,
    mb_terrain_discorner_quads,
    mb_terrain_discorner_tris,
};

s8 bottomslab_decal_uvs1[4][2] = {{16, 16}, {16, 8}, {0, 16}, {0, 8}};

struct MBTerrainPoly mb_terrain_bottomslab_quads[] = {
    {{{16, 8, 16}, {16, 8, 0},  {0, 8, 16},  {0, 8, 0}},   MB_DIRECTION_UP,    MB_FACESHAPE_EMPTY, MB_GROWTH_FULL, NULL}, // TOP
    {{{16, 0, 16}, {0, 0, 16},  {16, 0, 0},  {0, 0, 0}},   MB_DIRECTION_DOWN,  MB_FACESHAPE_FULL, 0, NULL}, // BOTTOM
    {{{16, 8, 16}, {16, 0, 16}, {16, 8, 0},  {16, 0, 0}},  MB_DIRECTION_POS_X, MB_FACESHAPE_BOTTOMSLAB, MB_GROWTH_UNCONDITIONAL, &bottomslab_decal_uvs1}, // LEFT
    {{{0, 8, 0},   {0, 0, 0},   {0, 8, 16},  {0, 0, 16}},  MB_DIRECTION_NEG_X, MB_FACESHAPE_BOTTOMSLAB, MB_GROWTH_UNCONDITIONAL, &bottomslab_decal_uvs1}, // RIGHT
    {{{0, 8, 16},  {0, 0, 16},  {16, 8, 16}, {16, 0, 16}}, MB_DIRECTION_POS_Z, MB_FACESHAPE_BOTTOMSLAB, MB_GROWTH_UNCONDITIONAL, &bottomslab_decal_uvs1}, // FRONT
    {{{16, 8, 0},  {16, 0, 0},  {0, 8, 0},   {0, 0, 0}},   MB_DIRECTION_NEG_Z, MB_FACESHAPE_BOTTOMSLAB, MB_GROWTH_UNCONDITIONAL, &bottomslab_decal_uvs1}, // BACK
};

struct MBTerrain mb_terrain_bottomslab = {
    6,
    0,
    mb_terrain_bottomslab_quads,
    NULL
};

struct MBTerrainPoly mb_terrain_topslab_quads[] = {
    {{{16, 16, 16}, {16, 16, 0}, {0, 16, 16},  {0, 16, 0}},  MB_DIRECTION_UP,    MB_FACESHAPE_FULL, MB_GROWTH_FULL, NULL}, // TOP
    {{{16, 8, 16},  {0, 8, 16},  {16, 8, 0},   {0, 8, 0}},   MB_DIRECTION_DOWN,  MB_FACESHAPE_EMPTY, 0, NULL}, // BOTTOM
    {{{16, 16, 16}, {16, 8, 16}, {16, 16, 0},  {16, 8, 0}},  MB_DIRECTION_POS_X, MB_FACESHAPE_TOPSLAB, MB_GROWTH_NORMAL_SIDE, NULL}, // LEFT
    {{{0, 16, 0},   {0, 8, 0},   {0, 16, 16},  {0, 8, 16}},  MB_DIRECTION_NEG_X, MB_FACESHAPE_TOPSLAB, MB_GROWTH_NORMAL_SIDE, NULL}, // RIGHT
    {{{0, 16, 16},  {0, 8, 16},  {16, 16, 16}, {16, 8, 16}}, MB_DIRECTION_POS_Z, MB_FACESHAPE_TOPSLAB, MB_GROWTH_NORMAL_SIDE, NULL}, // FRONT
    {{{16, 16, 0},  {16, 8, 0},  {0, 16, 0},   {0, 8, 0}},   MB_DIRECTION_NEG_Z, MB_FACESHAPE_TOPSLAB, MB_GROWTH_NORMAL_SIDE, NULL}, // BACK
};

struct MBTerrain mb_terrain_topslab = {
    6,
    0,
    mb_terrain_topslab_quads,
    NULL
};

struct MBTerrainPoly mb_terrain_vslab_quads[] = {
    {{{16, 16, 8}, {16, 16, 0}, {0, 16, 8},  {0, 16, 0}}, MB_DIRECTION_UP,    MB_FACESHAPE_TOPHALF,    MB_GROWTH_FULL, NULL}, // TOP
    {{{16, 0, 8},  {0, 0, 8},   {16, 0, 0},  {0, 0, 0}},  MB_DIRECTION_DOWN,  MB_FACESHAPE_TOPHALF,    0, NULL}, // BOTTOM
    {{{16, 16, 8}, {16, 0, 8},  {16, 16, 0}, {16, 0, 0}}, MB_DIRECTION_POS_X, MB_FACESHAPE_HALFSIDE_1, MB_GROWTH_HALF_SIDE,   NULL}, // LEFT
    {{{0, 16, 0},  {0, 0, 0},   {0, 16, 8},  {0, 0, 8}},  MB_DIRECTION_NEG_X, MB_FACESHAPE_HALFSIDE_2, MB_GROWTH_HALF_SIDE,   NULL}, // RIGHT
    {{{0, 16, 8},  {0, 0, 8},   {16, 16, 8}, {16, 0, 8}}, MB_DIRECTION_POS_Z, MB_FACESHAPE_EMPTY,      MB_GROWTH_VSLAB_SIDE, NULL}, // FRONT
    {{{16, 16, 0}, {16, 0, 0},  {0, 16, 0},  {0, 0, 0}},  MB_DIRECTION_NEG_Z, MB_FACESHAPE_FULL,       MB_GROWTH_NORMAL_SIDE,   NULL}, // BACK
};
struct MBTerrain mb_terrain_vslab = {
    6,
    0,
    mb_terrain_vslab_quads,
    NULL
};

s8 gentle_slope_uvs[4][2] = {{0, 0}, {0, 16}, {16, 0}, {16, 16}};

s8 gentle_decal_tri_uvs1[3][2] = {{0, 8}, {0, 16}, {16, 16}};
s8 gentle_decal_tri_uvs2[3][2] = {{16, 16}, {16, 8}, {0, 16}};
s8 ugentle_decal_quad_uvs1[4][2] = {{16, 16}, {16, 8}, {0, 8}, {0, 0}};
s8 ugentle_decal_quad_uvs2[4][2] = {{16, 8}, {16, 0}, {0, 16}, {0, 8}};

struct MBTerrainPoly mb_terrain_ugentle_quads[] = {
    {{{16, 8, 16}, {16, 16, 0}, {0, 8, 16}, {0, 16, 0}}, MB_DIRECTION_UP,    MB_FACESHAPE_EMPTY_2, MB_GROWTH_FULL, &gentle_slope_uvs}, // TOP
    {{{16, 0, 16}, {0, 0, 16},  {16, 0, 0}, {0, 0, 0}},  MB_DIRECTION_DOWN,  MB_FACESHAPE_FULL, 0, NULL}, // BOTTOM
    {{{16, 16, 0}, {16, 0, 0},  {0, 16, 0}, {0, 0, 0}},  MB_DIRECTION_NEG_Z, MB_FACESHAPE_FULL, MB_GROWTH_NORMAL_SIDE, NULL}, // BACK
    {{{0, 8, 16},  {0, 0, 16},  {16, 8, 16}, {16, 0, 16}}, MB_DIRECTION_POS_Z, MB_FACESHAPE_BOTTOMSLAB, MB_GROWTH_UNCONDITIONAL, &bottomslab_decal_uvs1}, // FRONT
    {{{16, 8, 16}, {16, 0, 16}, {16, 8, 0},  {16, 0, 0}},  MB_DIRECTION_POS_X, MB_FACESHAPE_BOTTOMSLAB, MB_GROWTH_UNCONDITIONAL, &ugentle_decal_quad_uvs1}, // LEFT
    {{{0, 8, 0},   {0, 0, 0},   {0, 8, 16},  {0, 0, 16}},  MB_DIRECTION_NEG_X, MB_FACESHAPE_BOTTOMSLAB, MB_GROWTH_UNCONDITIONAL, &ugentle_decal_quad_uvs2}, // RIGHT
};

struct MBTerrainPoly mb_terrain_ugentle_tris[] = {
    {{{16, 8, 0}, {16, 16, 0}, {16, 8, 16}}, MB_DIRECTION_POS_X, MB_FACESHAPE_UPPERGENTLE_1, MB_GROWTH_UNCONDITIONAL, &gentle_decal_tri_uvs1}, // LEFT
    {{{0, 16, 0}, {0, 8, 0},   {0, 8, 16}},  MB_DIRECTION_NEG_X, MB_FACESHAPE_UPPERGENTLE_2, MB_GROWTH_UNCONDITIONAL, &gentle_decal_tri_uvs2}, // RIGHT
};

struct MBTerrain mb_terrain_ugentle = {
    6,
    2,
    mb_terrain_ugentle_quads,
    mb_terrain_ugentle_tris,
};

struct MBTerrainPoly mb_terrain_dugentle_quads[] = {
    {{{16, 16, 16}, {16, 16, 0}, {0, 16, 16},  {0, 16, 0}}, MB_DIRECTION_UP,    MB_FACESHAPE_FULL, MB_GROWTH_FULL, NULL}, // TOP
    {{{16, 16, 0},  {16, 0, 0},  {0, 16, 0},   {0, 0, 0}},  MB_DIRECTION_NEG_Z, MB_FACESHAPE_FULL, MB_GROWTH_NORMAL_SIDE, NULL}, // BACK
    {{{16, 0, 0},   {16, 8, 16}, {0, 0, 0},    {0, 8, 16}},  MB_DIRECTION_DOWN, MB_FACESHAPE_EMPTY_2, MB_GROWTH_NONE, &gentle_slope_uvs}, // BOTTOM
    {{{0, 16, 16},  {0, 8, 16},  {16, 16, 16}, {16, 8, 16}}, MB_DIRECTION_POS_Z, MB_FACESHAPE_TOPSLAB, MB_GROWTH_NORMAL_SIDE, NULL}, // FRONT
    {{{16, 16, 16}, {16, 8, 16}, {16, 16, 0},  {16, 8, 0}},  MB_DIRECTION_POS_X, MB_FACESHAPE_TOPSLAB, MB_GROWTH_NORMAL_SIDE, NULL}, // LEFT
    {{{0, 16, 0},   {0, 8, 0},   {0, 16, 16},  {0, 8, 16}},  MB_DIRECTION_NEG_X, MB_FACESHAPE_TOPSLAB, MB_GROWTH_NORMAL_SIDE, NULL}, // RIGHT
};

struct MBTerrainPoly mb_terrain_dugentle_tris[] = {
    {{{16, 0, 0}, {16, 8, 0}, {16, 8, 16}},  MB_DIRECTION_POS_X, MB_FACESHAPE_DOWNUPPERGENTLE_1, MB_GROWTH_NONE, NULL}, // LEFT
    {{{0, 8, 0}, {0, 0, 0},   {0, 8, 16}},   MB_DIRECTION_NEG_X, MB_FACESHAPE_DOWNUPPERGENTLE_2, MB_GROWTH_NONE, NULL}, // RIGHT
};

struct MBTerrain mb_terrain_dugentle = {
    6,
    2,
    mb_terrain_dugentle_quads,
    mb_terrain_dugentle_tris,
};

struct MBTerrainPoly mb_terrain_lgentle_quads[] = {
    {{{16, 0, 16}, {16, 8, 0}, {0, 0, 16}, {0, 8, 0}}, MB_DIRECTION_UP,    MB_FACESHAPE_EMPTY_2, MB_GROWTH_FULL, &gentle_slope_uvs}, // TOP
    {{{16, 0, 16}, {0, 0, 16},  {16, 0, 0}, {0, 0, 0}},  MB_DIRECTION_DOWN,  MB_FACESHAPE_FULL, 0, NULL}, // BOTTOM
    {{{16, 8, 0},  {16, 0, 0},  {0, 8, 0},   {0, 0, 0}},   MB_DIRECTION_NEG_Z, MB_FACESHAPE_BOTTOMSLAB, MB_GROWTH_UNCONDITIONAL, &bottomslab_decal_uvs1}, // BACK
};

struct MBTerrainPoly mb_terrain_lgentle_tris[] = {
    {{{16, 0, 0}, {16, 8, 0}, {16, 0, 16}}, MB_DIRECTION_POS_X, MB_FACESHAPE_LOWERGENTLE_1, MB_GROWTH_GENTLE_SIDE_L, &gentle_decal_tri_uvs1}, // LEFT
    {{{0, 8, 0}, {0, 0, 0},   {0, 0, 16}},  MB_DIRECTION_NEG_X, MB_FACESHAPE_LOWERGENTLE_2, MB_GROWTH_GENTLE_SIDE_R, &gentle_decal_tri_uvs2}, // RIGHT
};

struct MBTerrain mb_terrain_lgentle = {
    3,
    2,
    mb_terrain_lgentle_quads,
    mb_terrain_lgentle_tris,
};

s8 dlgentle_decal_uvs[4][2] = {{0, 16}, {16, 16}, {0, 8}, {16, 8}};

struct MBTerrainPoly mb_terrain_dlgentle_quads[] = {
    {{{16, 16, 16}, {16, 16, 0}, {0, 16, 16}, {0, 16, 0}}, MB_DIRECTION_UP,    MB_FACESHAPE_FULL, MB_GROWTH_FULL, NULL}, // TOP
    {{{16, 16, 0},  {16, 8, 0},  {0, 16, 0},   {0, 8, 0}}, MB_DIRECTION_NEG_Z, MB_FACESHAPE_TOPSLAB, MB_GROWTH_NORMAL_SIDE, NULL}, // BACK
    {{{16, 8, 0},   {16, 16, 16}, {0, 8, 0},    {0, 16, 16}},  MB_DIRECTION_DOWN, MB_FACESHAPE_EMPTY_2, MB_GROWTH_NORMAL_SIDE, &gentle_slope_uvs}, // BOTTOM
};

struct MBTerrainPoly mb_terrain_dlgentle_tris[] = {
    {{{16, 8, 0}, {16, 16, 0}, {16, 16, 16}},  MB_DIRECTION_POS_X, MB_FACESHAPE_DOWNLOWERGENTLE_1, MB_GROWTH_NORMAL_SIDE, NULL}, // LEFT
    {{{0, 16, 0}, {0, 8, 0},   {0, 16, 16}},   MB_DIRECTION_NEG_X, MB_FACESHAPE_DOWNLOWERGENTLE_2, MB_GROWTH_NORMAL_SIDE, NULL}, // RIGHT
};

struct MBTerrain mb_terrain_dlgentle = {
    3,
    2,
    mb_terrain_dlgentle_quads,
    mb_terrain_dlgentle_tris,
};


struct MBTerrainPoly mb_terrain_slopebelowdecal_quad = {
    {{0, 16, 16}, {0, 0, 16}, {16, 16, 16}, {16, 0, 16}}, MB_DIRECTION_POS_Z, MB_FACESHAPE_FULL, 0, NULL
};
struct MBTerrainPoly mb_terrain_slopebelowdecal_downtri1 = {
    {{16, 16, 16}, {0, 16, 16}, {16, 0, 16}}, MB_DIRECTION_POS_Z, MB_FACESHAPE_DOWNTRI_1, 0, NULL
};
struct MBTerrainPoly mb_terrain_slopebelowdecal_downtri2 = {
    {{0, 16, 16}, {0, 0, 16}, {16, 16, 16}}, MB_DIRECTION_POS_Z, MB_FACESHAPE_DOWNTRI_2, 0, NULL
};
struct MBTerrainPoly mb_terrain_slopebelowdecal_topslab = {
    {{0, 16, 16}, {0, 8, 16}, {16, 16, 16}, {16, 8, 16}}, MB_DIRECTION_POS_Z, MB_FACESHAPE_TOPSLAB, 0, NULL
};
struct MBTerrainPoly mb_terrain_slopebelowdecal_vslab_1 = {
    {{8, 16, 16}, {8, 0, 16}, {16, 16, 16}, {16, 0, 16}}, MB_DIRECTION_POS_Z, MB_FACESHAPE_HALFSIDE_1, 0, NULL
};
struct MBTerrainPoly mb_terrain_slopebelowdecal_vslab_2 = {
    {{0, 16, 16}, {0, 0, 16}, {8, 16, 16}, {8, 0, 16}}, MB_DIRECTION_POS_Z, MB_FACESHAPE_HALFSIDE_2, 0, NULL
};
struct MBTerrainPoly mb_terrain_slopebelowdecal_downlowergentle1 = {
    {{16, 16, 16}, {0, 16, 16}, {16, 8, 16}}, MB_DIRECTION_POS_Z, MB_FACESHAPE_DOWNLOWERGENTLE_1, 0, NULL
};
struct MBTerrainPoly mb_terrain_slopebelowdecal_downlowergentle2 = {
    {{0, 16, 16}, {0, 8, 16}, {16, 16, 16}}, MB_DIRECTION_POS_Z, MB_FACESHAPE_DOWNLOWERGENTLE_2, 0, NULL
};

void *slope_decal_below_surfs[] = {
    &mb_terrain_slopebelowdecal_quad,
    &mb_terrain_slopebelowdecal_downtri1,
    &mb_terrain_slopebelowdecal_downtri2,
    &mb_terrain_slopebelowdecal_topslab,
    &mb_terrain_slopebelowdecal_vslab_1,
    &mb_terrain_slopebelowdecal_vslab_2,
    &mb_terrain_slopebelowdecal_downlowergentle1,
    &mb_terrain_slopebelowdecal_downlowergentle2,
};

// Shape of fence

s8 fence_uvs[4][2] = {{32, 16},  {32, 0},  {0, 16},  {0, 0}};
struct MBTerrainPoly mb_terrain_fence_quad[] = {
    {{{0, 8, 0}, {0, 0, 0}, {16, 8, 0}, {16, 0, 0}}, MB_DIRECTION_POS_Z, MB_FACESHAPE_EMPTY, 0, &fence_uvs}, // FRONT (towards tile)
    {{{16, 8, 0}, {16, 0, 0}, {0, 8, 0}, {0, 0, 0}}, MB_DIRECTION_NEG_Z, MB_FACESHAPE_BOTTOMSLAB, 0, &fence_uvs}, // BACK (away from tile)
};

struct MBTerrainPoly mb_terrain_fence_col_quads[] = {
    {{{16, 7, 1},  {0, 7, 1},   {16, 0, 1}, {0, 0, 1}}, MB_DIRECTION_POS_Z, MB_FACESHAPE_EMPTY, 0, NULL}, // FRONT (towards tile)
    {{{16, 7, -1}, {16, 0, -1}, {0, 7, -1}, {0, 0, -1}}, MB_DIRECTION_NEG_Z, MB_FACESHAPE_BOTTOMSLAB, 0, NULL}, // BACK (away from tile)

    {{{16, 8, 0},  {0, 8, 0},   {16, 7, 1}, {0, 7, 1}}, MB_DIRECTION_UP, MB_FACESHAPE_EMPTY, 0, NULL}, // FRONT (towards tile)
    {{{16, 8, 0},  {16, 7, -1}, {0, 8, 0},  {0, 7, -1}}, MB_DIRECTION_UP, MB_FACESHAPE_EMPTY, 0, NULL}, // BACK (away from tile)
};

struct MBTerrain mb_terrain_fence = {
    2,
    0,
    mb_terrain_fence_quad,
    NULL,
};

struct MBTerrain mb_terrain_fence_col = {
    4,
    0,
    mb_terrain_fence_col_quads,
    NULL,
};


s8 pole_uvs1[4][2] = {{16, 32}, {16, 0}, {12, 32}, {12, 0}};
s8 pole_uvs2[4][2] = {{12, 32}, {12, 0}, {8, 32}, {8, 0}};
s8 pole_uvs3[4][2] = {{8, 32}, {8, 0}, {4, 32}, {4, 0}};
s8 pole_uvs4[4][2] = {{4, 32}, {4, 0}, {0, 32}, {0, 0}};
struct MBTerrainPoly mb_terrain_pole_quads[] = {
    {{{8, 16, 9}, {8, 0, 9}, {9, 16, 8}, {9, 0, 8}}, MB_DIRECTION_POS_Z, MB_FACESHAPE_EMPTY, 0, &pole_uvs1}, // Diagonal 1
    {{{9, 16, 8}, {9, 0, 8}, {8, 16, 7}, {8, 0, 7}}, MB_DIRECTION_POS_X, MB_FACESHAPE_EMPTY, 0, &pole_uvs2}, // Diagonal 2
    {{{8, 16, 7}, {8, 0, 7}, {7, 16, 8}, {7, 0, 8}}, MB_DIRECTION_NEG_Z, MB_FACESHAPE_EMPTY, 0, &pole_uvs3}, // Diagonal 3
    {{{7, 16, 8}, {7, 0, 8}, {8, 16, 9}, {8, 0, 9}}, MB_DIRECTION_NEG_X, MB_FACESHAPE_EMPTY, 0, &pole_uvs4}, // Diagonal 4

    {{{8, 16, 9}, {9, 16, 8}, {7, 16, 8}, {8, 16, 7}}, MB_DIRECTION_UP,   MB_FACESHAPE_POLETOP, 0, NULL}, // Top
    {{{8, 0, 9},  {7, 0, 8},  {9, 0, 8},  {8, 0, 7}},  MB_DIRECTION_DOWN, MB_FACESHAPE_POLETOP, 0, NULL}, // Bottom
};

struct MBTerrain mb_terrain_pole = {
    6,
    0,
    mb_terrain_pole_quads,
    NULL,
};

struct MBTerrainPoly mb_terrain_bars_connected_quads[] = {
    {{{7, 16, 16}, {7, 16, 9}, {7, 0, 16}, {7, 0, 9}}, MB_DIRECTION_NEG_X, MB_FACESHAPE_EMPTY, 0, NULL}, // Left
    {{{9, 16, 9}, {9, 16, 16}, {9, 0, 9}, {9, 0, 16}}, MB_DIRECTION_POS_X, MB_FACESHAPE_EMPTY, 0, NULL}, // Right

    {{{7, 16, 16}, {9, 16, 16}, {7, 16, 9}, {9, 16, 9}}, MB_DIRECTION_UP, MB_FACESHAPE_EMPTY, 0, NULL}, // Top
    {{{9, 0, 16}, {7, 0, 16}, {9, 0, 9}, {7, 0, 9}}, MB_DIRECTION_DOWN, MB_FACESHAPE_EMPTY, 0, NULL}, // Bottom
};

struct MBTerrainPoly mb_terrain_bars_unconnected_quad[] = {
    {{{9, 16, 9}, {7, 16, 9}, {9, 0, 9}, {7, 0, 9}}, MB_DIRECTION_POS_Z, MB_FACESHAPE_EMPTY, 0, NULL}, // Back
};

struct MBTerrainPoly mb_terrain_bars_center_quads[] = {
    {{{7, 16, 9}, {9, 16, 9}, {7, 16, 7}, {9, 16, 7}}, MB_DIRECTION_UP, MB_FACESHAPE_EMPTY, 0, NULL},
    {{{9, 0, 9}, {7, 0, 9}, {9, 0, 7}, {7, 0, 7}}, MB_DIRECTION_DOWN, MB_FACESHAPE_EMPTY, 0, NULL},
};

// Shapes of water tiles
struct MBTerrainPoly mb_terrain_shallowwater_quads[] = {
    {{{16, 14, 16}, {16, 14, 0}, {0, 14, 16}, {0, 14, 0}}, MB_DIRECTION_UP, MB_FACESHAPE_EMPTY, 0, NULL}, // TOP
    {{{16, 0, 16},  {0, 0, 16},  {16, 0, 0},  {0, 0, 0}},  MB_DIRECTION_DOWN, MB_FACESHAPE_FULL, 0, NULL}, // BOTTOM
    {{{16, 14, 16}, {16, 0, 16}, {16, 14, 0}, {16, 0, 0}}, MB_DIRECTION_POS_X, MB_FACESHAPE_FULL, 0, NULL}, // LEFT
    {{{0, 14, 16},  {0, 14, 0},  {0, 0, 16},  {0, 0, 0}},  MB_DIRECTION_NEG_X, MB_FACESHAPE_FULL, 0, NULL}, // RIGHT
    {{{16, 14, 16}, {0, 14, 16}, {16, 0, 16}, {0, 0, 16}}, MB_DIRECTION_POS_Z, MB_FACESHAPE_FULL, 0, NULL}, // FRONT
    {{{16, 14, 0},  {16, 0, 0},  {0, 14, 0},  {0, 0, 0}},  MB_DIRECTION_NEG_Z, MB_FACESHAPE_FULL, 0, NULL}, // BACK
};

struct MBTerrainPoly mb_terrain_topwater_quads[] = {
    {{{16, 16, 16}, {16, 16, 0}, {0, 16, 16}, {0, 16, 0}}, MB_DIRECTION_UP, MB_FACESHAPE_FULL, 0, NULL}, // TOP
    {{{16, 0, 16},  {0, 0, 16},  {16, 0, 0},  {0, 0, 0}},  MB_DIRECTION_DOWN, MB_FACESHAPE_FULL, 0, NULL}, // BOTTOM
    {{{16, 16, 16}, {16, 14, 16}, {16, 16, 0}, {16, 14, 0}}, MB_DIRECTION_POS_X, MB_FACESHAPE_FULL, 0, NULL}, // LEFT
    {{{0, 16, 16},  {0, 16, 0},  {0, 14, 16},  {0, 14, 0}},  MB_DIRECTION_NEG_X, MB_FACESHAPE_FULL, 0, NULL}, // RIGHT
    {{{16, 16, 16}, {0, 16, 16}, {16, 14, 16}, {0, 14, 16}}, MB_DIRECTION_POS_Z, MB_FACESHAPE_FULL, 0, NULL}, // FRONT
    {{{16, 16, 0},  {16, 14, 0},  {0, 16, 0},  {0, 14, 0}}, MB_DIRECTION_NEG_Z, MB_FACESHAPE_FULL, 0, NULL}, // BACK
};

struct MBTerrainPoly *mb_terrain_water_quadlists[] = {
    mb_terrain_shallowwater_quads,
    mb_terrain_fullblock_quads,
    mb_terrain_topwater_quads,
};

struct MBTerrainInfo mb_terrain_info_list[] = {
    {NULL, NULL, NULL},
    {NULL, NULL, NULL},
    {"Slope", mat_b_btn_slope, &mb_terrain_slope},
    {NULL, NULL, &mb_terrain_dslope},
    {"Slab", mat_b_btn_slabtile, &mb_terrain_bottomslab},
    {NULL, NULL, &mb_terrain_topslab},
    {"Outer Corner", mat_b_btn_corner, &mb_terrain_corner},
    {NULL, NULL, &mb_terrain_dcorner},
    {"Inner Corner", mat_b_btn_icorner, &mb_terrain_icorner},
    {NULL, NULL, &mb_terrain_dicorner},
    {"Sloped Corner", mat_b_btn_triangle, &mb_terrain_scorner},
    {NULL, NULL, &mb_terrain_dscorner},
    {"Inverted Sloped Corner", mat_b_btn_isc, &mb_terrain_iscorner},
    {NULL, NULL, &mb_terrain_discorner},
    {"Upper Gentle Slope", mat_b_btn_ugs, &mb_terrain_ugentle},
    {NULL, NULL, &mb_terrain_dugentle},
    {"Lower Gentle Slope", mat_b_btn_lgs, &mb_terrain_lgentle},
    {NULL, NULL, &mb_terrain_dlgentle},

    {"Tile", mat_b_btn_tile, &mb_terrain_fullblock},
    {"Vertical Slope", mat_b_btn_sideslope, &mb_terrain_sslope},
    {"Vertical Slab", mat_b_btn_vslab, &mb_terrain_vslab},
    {"Cull Marker", mat_b_btn_cull, NULL},
    {"Intangible Tile", mat_b_btn_troll, &mb_terrain_fullblock},
    {"Fence", mat_b_btn_fence, NULL},
    {"Pole", mat_b_btn_pole, NULL},
    {"Iron Mesh", mat_b_btn_bars, NULL},
    {"Water", mat_b_btn_water, NULL},
};

extern Gfx mat_maker_MakerMCWater[];
extern Gfx mat_maker_MakerMCLava[];
extern Gfx mat_maker_MakerMCFlowingLava[];

struct MBMaterial mb_mat_table[] = {
    // Terrain
    {mat_maker_MakerGrass,        MAT_OPAQUE, FALSE, SURFACE_GRASS,        "Grass"},       // MB_MAT_GRASS
    {mat_maker_MakerVanillaGrass, MAT_OPAQUE, FALSE, SURFACE_GRASS,        "Grass (Old)"}, // MB_MAT_GRASS_OLD
    {mat_maker_MakerCartoonGrass, MAT_OPAQUE, FALSE, SURFACE_GRASS,        "Smooth Grass"},// MB_MAT_CARTOON_GRASS
    {mat_maker_MakerDarkGrass,    MAT_OPAQUE, FALSE, SURFACE_GRASS,        "Dark Grass"},  // MB_MAT_DARK_GRASS
    {mat_maker_MakerHGrass,       MAT_OPAQUE, FALSE, SURFACE_GRASS,        "Cave Grass"},  // MB_MAT_HMC_GRASS
    {mat_maker_MakerOrangeGrass,  MAT_OPAQUE, FALSE, SURFACE_GRASS,        "Orange Grass"},// MB_MAT_ORANGE_GRASS
    {mat_maker_MakerRedGrass,     MAT_OPAQUE, FALSE, SURFACE_GRASS,        "Red Grass"},   // MB_MAT_RED_GRASS
    {mat_maker_MakerPurpleGrass,  MAT_OPAQUE, FALSE, SURFACE_GRASS,        "Purple Grass"},// MB_MAT_PURPLE_GRASS
    {mat_maker_MakerSand,         MAT_OPAQUE, FALSE, SURFACE_SAND,         "Sand"},        // MB_MAT_SAND
    {mat_maker_MakerJRBSand,      MAT_OPAQUE, FALSE, SURFACE_SAND,         "Ocean Sand"},  // MB_MAT_JRB_SAND
    {mat_maker_MakerSnow,         MAT_OPAQUE, FALSE, SURFACE_SNOW,         "Snow"},        // MB_MAT_SNOW
    {mat_maker_MakerVanillaSnow,  MAT_OPAQUE, FALSE, SURFACE_SNOW,         "Snow (Old)"},  // MB_MAT_SNOW_OLD
    {mat_maker_MakerDirt,         MAT_OPAQUE, FALSE, SURFACE_NOT_SLIPPERY, "Dirt"},        // MB_MAT_DIRT
    {mat_maker_MakerSandDirt,     MAT_OPAQUE, TRUE,  SURFACE_NOT_SLIPPERY, "Sandy Dirt"},  // MB_MAT_SANDDIRT
    {mat_maker_MakerLightDirt,    MAT_OPAQUE, FALSE, SURFACE_NOT_SLIPPERY, "Light Dirt"},  // MB_MAT_LIGHTDIRT
    {mat_maker_MakerHDirt,        MAT_OPAQUE, FALSE, SURFACE_NOT_SLIPPERY, "Cave Dirt"},   // MB_MAT_HMC_DIRT
    {mat_maker_MakerRockyDirt,    MAT_OPAQUE, FALSE, SURFACE_NOT_SLIPPERY, "Rocky Dirt"},  // MB_MAT_ROCKY_DIRT
    {mat_maker_MakerVanillaDirt,  MAT_OPAQUE, FALSE, SURFACE_NOT_SLIPPERY,  "Dirt (Old)"}, // MB_MAT_DIRT_OLD
    {mat_maker_MakerWavyDirt,     MAT_OPAQUE, TRUE,  SURFACE_NOT_SLIPPERY, "Wavy Dirt"},   // MB_MAT_WAVY_DIRT
    {mat_maker_MakerWavyDirtBlue, MAT_OPAQUE, TRUE,  SURFACE_NOT_SLIPPERY, "Wavy Dirt (Blue)"}, // MB_MAT_WAVY_DIRT_BLUE
    {mat_maker_MakerSnowDirt,     MAT_OPAQUE, FALSE, SURFACE_NOT_SLIPPERY, "Snowy Dirt"},  // MB_MAT_SNOWDIRT
    {mat_maker_MakerPurpleDirt,   MAT_OPAQUE, TRUE,  SURFACE_NOT_SLIPPERY, "Purple Dirt"}, // MB_MAT_PURPLE_DIRT
    {mat_maker_MakerHLakewall,    MAT_OPAQUE, TRUE,  SURFACE_NOT_SLIPPERY, "Clay"},        // MB_MAT_HMC_LAKEGRASS

    // Stone
    {mat_maker_MakerStoneSide,     MAT_OPAQUE, FALSE, SURFACE_DEFAULT,      "White Stone"},     // MB_MAT_STONE
    {mat_maker_MakerHStone,        MAT_OPAQUE, FALSE, SURFACE_NOT_SLIPPERY, "Cave Stone"},      // MB_MAT_HMC_STONE
    {mat_maker_MakerHMazefloor,    MAT_OPAQUE, FALSE, SURFACE_DEFAULT,      "Beige Stone"},     // MB_MAT_HMC_MAZEFLOOR
    {mat_maker_MakerCCMWall,       MAT_OPAQUE, FALSE, SURFACE_NOT_SLIPPERY, "Green Stone"},     // MB_MAT_CCM_ROCK
    {mat_maker_MakerMountainFloor, MAT_OPAQUE, FALSE, SURFACE_NOT_SLIPPERY, "Mountain Stone"},  // MB_MAT_TTM_FLOOR
    {mat_maker_MakerMountainRock,  MAT_OPAQUE, FALSE, SURFACE_NOT_SLIPPERY, "Mountain Rock"},   // MB_MAT_TTM_ROCK
    {mat_maker_MakerStone,         MAT_OPAQUE, FALSE, SURFACE_NOT_SLIPPERY, "White Rock"},      // MB_MAT_COBBLESTONE
    {mat_maker_MakerJRBWall,       MAT_OPAQUE, FALSE, SURFACE_NOT_SLIPPERY, "Green Rock"},      // MB_MAT_JRB_WALL
    {mat_maker_MakerBlackStone,    MAT_OPAQUE, FALSE, SURFACE_DEFAULT,      "Black Rock"},      // MB_MAT_GABBRO
    {mat_maker_MakerRHRStone,      MAT_OPAQUE, FALSE, SURFACE_NOT_SLIPPERY, "Scorched Rock"},   // MB_MAT_RHR_STONE
    {mat_maker_MakerLavaRock,      MAT_OPAQUE, FALSE, SURFACE_NOT_SLIPPERY, "Volcanic Rocks"},  // MB_MAT_LAVA_ROCKS
    {mat_maker_MakerVolcanicRock,  MAT_OPAQUE, TRUE,  SURFACE_DEFAULT,      "Volcanic Wall"},   // MB_MAT_VOLCANO_WALL
    {mat_maker_MakerRHRBasalt,     MAT_OPAQUE, TRUE,  SURFACE_NOT_SLIPPERY, "Basalt"},          // MB_MAT_RHR_BASALT
    {mat_maker_MakerObsidian,      MAT_OPAQUE, FALSE, SURFACE_NOT_SLIPPERY, "Obsidian"},        // MB_MAT_OBSIDIAN
    {mat_maker_MakerCastleCobble,  MAT_OPAQUE, FALSE, SURFACE_NOT_SLIPPERY, "Plum Concrete"},   // MB_MAT_CASTLE_STONE
    {mat_maker_MakerJRBUnderwater, MAT_OPAQUE, FALSE, SURFACE_NOT_SLIPPERY, "Ocean Floor"},     // MB_MAT_JRB_UNDERWATER
    {mat_maker_MakerSnowRock,      MAT_OPAQUE, TRUE,  SURFACE_DEFAULT,      "Snowy Rock"},      // MB_MAT_SNOW_ROCK
    {mat_maker_MakerIcyRock,       MAT_OPAQUE, TRUE,  SURFACE_VERY_SLIPPERY, "Icy Rock"},       // MB_MAT_ICY_ROCK
    {mat_maker_MakerDStone,        MAT_OPAQUE, FALSE, SURFACE_NOT_SLIPPERY, "Cobblestone (1)"}, // MB_MAT_DESERT_STONE
    {mat_maker_MakerRHRObsidian,   MAT_OPAQUE, FALSE, SURFACE_NOT_SLIPPERY, "Cobblestone (2)"}, // MB_MAT_RHR_OBSIDIAN
    {mat_maker_MakerJRBStone,      MAT_OPAQUE, FALSE, SURFACE_NOT_SLIPPERY, "Cobblestone (3)"}, // MB_MAT_JRB_STONE

    // Bricks
    {mat_maker_MakerBricks,           MAT_OPAQUE, TRUE,  SURFACE_DEFAULT,      "Stone Bricks"},     // MB_MAT_BRICKS
    {mat_maker_MakerDBrick,           MAT_OPAQUE, TRUE,  SURFACE_DEFAULT,      "Desert Bricks"},    // MB_MAT_DESERT_BRICKS
    {mat_maker_MakerRHRBrick,         MAT_OPAQUE, TRUE,  SURFACE_NOT_SLIPPERY, "Scorched Bricks"},  // MB_MAT_RHR_BRICK
    {mat_maker_MakerHBrick,           MAT_OPAQUE, TRUE,  SURFACE_DEFAULT,      "Cave Bricks"},      // MB_MAT_HMC_BRICK
    {mat_maker_MakerLightBrownBricks, MAT_OPAQUE, TRUE,  SURFACE_DEFAULT,    "Fortress Bricks"},  // MB_MAT_LIGHTBROWN_BRICK
    {mat_maker_MakerBrownBricks,      MAT_OPAQUE, TRUE,  SURFACE_DEFAULT,      "Brown Bricks (1)"}, // MB_MAT_WDW_BRICK
    {mat_maker_MakerTTMBricks,        MAT_OPAQUE, TRUE,  SURFACE_DEFAULT,      "Brown Bricks (2)"}, // MB_MAT_TTM_BRICK
    {mat_maker_MakerCBrick,           MAT_OPAQUE, TRUE,  SURFACE_NOT_SLIPPERY, "Castle Bricks"},    // MB_MAT_C_BRICK
    {mat_maker_MakerBBHBricks,        MAT_OPAQUE, TRUE,  SURFACE_NOT_SLIPPERY, "Red Bricks (1)"},   // MB_MAT_BBH_BRICKS
    {mat_maker_MakerRoofBricks,       MAT_OPAQUE, TRUE,  SURFACE_DEFAULT,      "Red Bricks (2)"},   // MB_MAT_ROOF_BRICKS
    {mat_maker_MakerCOutsideBricks,   MAT_OPAQUE, TRUE,  SURFACE_DEFAULT,      "White Bricks (1)"}, // MB_MAT_C_OUTSIDE_BRICK
    {mat_maker_MakerSnowBricks,       MAT_OPAQUE, TRUE,  SURFACE_DEFAULT,      "White Bricks (2)"}, // MB_MAT_SNOW_BRICKS
    {mat_maker_MakerJRBBricks,        MAT_OPAQUE, TRUE,  SURFACE_DEFAULT,      "Ocean Bricks"},     // MB_MAT_JRB_BRICKS 
    {mat_maker_MakerSnowTileSide,     MAT_OPAQUE, TRUE,  SURFACE_DEFAULT,      "Blue Bricks"},      // MB_MAT_SNOW_TILE_SIDE
    {mat_maker_MakerTileBricks,       MAT_OPAQUE, FALSE, SURFACE_NOT_SLIPPERY, "Mixed Bricks"},     // MB_MAT_TILESBRICKS

    // Tiling
    {mat_maker_MakerTiles,          MAT_OPAQUE, FALSE, SURFACE_NOT_SLIPPERY, "Checkered Tiling"}, // MB_MAT_TILES
    {mat_maker_MakerCTiles,         MAT_OPAQUE, FALSE, SURFACE_NOT_SLIPPERY, "Castle Tiling"},    // MB_MAT_C_TILES
    {mat_maker_MakerDTiles,         MAT_OPAQUE, FALSE, SURFACE_NOT_SLIPPERY, "Desert Tiling"},    // MB_MAT_DESERT_TILES
    {mat_maker_MakerVBlueTiles,     MAT_OPAQUE, FALSE, SURFACE_NOT_SLIPPERY, "Blue Tiling (1)"},  // MB_MAT_VP_BLUETILES
    {mat_maker_MakerSnowTiles,      MAT_OPAQUE, FALSE, SURFACE_NOT_SLIPPERY, "Blue Tiling (2)"},  // MB_MAT_SNOW_TILES
    {mat_maker_MakerJRBTileTop,     MAT_OPAQUE, FALSE, SURFACE_NOT_SLIPPERY, "Ocean Tiling (1)"}, // MB_MAT_JRB_TILETOP
    {mat_maker_MakerJRBTileSide,    MAT_OPAQUE, TRUE,  SURFACE_DEFAULT,      "Ocean Tiling (2)"}, // MB_MAT_JRB_TILESIDE
    {mat_maker_MakerHTileTop,       MAT_OPAQUE, FALSE, SURFACE_NOT_SLIPPERY, "Dark Tiling"},     // MB_MAT_HMC_TILES
    {mat_maker_MakerGraniteTiles,   MAT_OPAQUE, FALSE, SURFACE_NOT_SLIPPERY, "Granite Tiling"},   // MB_MAT_GRANITE_TILES
    {mat_maker_MakerRHRTiles,       MAT_OPAQUE, FALSE, SURFACE_NOT_SLIPPERY, "Black Tiling"},    // MB_MAT_RHR_TILES
    {mat_maker_MakerVTiles,         MAT_OPAQUE, TRUE,  SURFACE_DEFAULT,      "Grey Tiling"},     // MB_MAT_VP_TILES
    {mat_maker_MakerDiamondPattern, MAT_OPAQUE, TRUE,  SURFACE_NOT_SLIPPERY, "Diamond Pattern"},  // MB_MAT_DIAMOND_PATTERN
    {mat_maker_MakerCStone,         MAT_OPAQUE, FALSE, SURFACE_NOT_SLIPPERY, "Hex Tiling"},       // MB_MAT_C_STONETOP
    {mat_maker_MakerSnowBrickTiles, MAT_OPAQUE, FALSE, SURFACE_DEFAULT,      "Diamond Tiling"},   // MB_MAT_SNOW_BRICK_TILES

    // Cut Stone
    {mat_maker_MakerDStoneBlock,     MAT_OPAQUE, TRUE, SURFACE_NOT_SLIPPERY, "Block"},           // MB_MAT_DESERT_BLOCK
    {mat_maker_MakerVBlock,          MAT_OPAQUE, TRUE, SURFACE_DEFAULT,      "Smooth Block"},    // MB_MAT_VP_BLOCK
    {mat_maker_MakerBBHWall,         MAT_OPAQUE, TRUE, SURFACE_DEFAULT,      "Mansion Wall"},    // MB_MAT_BBH_STONE
    {mat_maker_MakerBBHStonePattern, MAT_OPAQUE, TRUE, SURFACE_DEFAULT,      "Chiseled Wall"},   // MB_MAT_BBH_STONE_PATTERN
    {mat_maker_MakerLauncherTex,     MAT_OPAQUE, TRUE, SURFACE_DEFAULT,      "Patterned Block"}, // MB_MAT_PATTERNED_BLOCK
    {mat_maker_MakerHTileSide,       MAT_OPAQUE, TRUE, SURFACE_DEFAULT,      "Black Slabs"},     // MB_MAT_HMC_SLAB
    {mat_maker_MakerRHRPattern,      MAT_OPAQUE, TRUE, SURFACE_DEFAULT,      "Chiseled Block"},  // MB_MAT_RHR_BLOCK
    {mat_maker_MakerGraniteBlock,    MAT_OPAQUE, TRUE, SURFACE_NOT_SLIPPERY, "Granite Block"},   // MB_MAT_GRANITE_BLOCK
    {mat_maker_MakerCStoneSide,      MAT_OPAQUE, TRUE, SURFACE_DEFAULT,      "Stone Slab"},      // MB_MAT_C_STONESIDE
    {mat_maker_MakerCPillar,         MAT_OPAQUE, TRUE, SURFACE_DEFAULT,      "Castle Pillar"},   // MB_MAT_C_PILLAR
    {mat_maker_MakerBBHPillar,       MAT_OPAQUE, TRUE, SURFACE_DEFAULT,      "Mansion Pillar"},  // MB_MAT_BBH_PILLAR
    {mat_maker_MakerRHRPillar,       MAT_OPAQUE, TRUE, SURFACE_NOT_SLIPPERY, "Scorched Pillar"}, // MB_MAT_RHR_PILLAR

    // Wood
    {mat_maker_MakerWood,             MAT_OPAQUE, TRUE,  SURFACE_CREAKWOOD,    "Planks (1)"},    // MB_MAT_WOOD
    {mat_maker_MakerBBHWoodWall,      MAT_OPAQUE, TRUE,  SURFACE_CREAKWOOD,    "Planks (2)"},    // MB_MAT_BBH_WOOD_WALL
    {mat_maker_MakerBBHWoodFloor,     MAT_OPAQUE, TRUE,  SURFACE_CREAKWOOD,    "Dark Planks"},   // MB_MAT_BBH_WOOD_FLOOR
    {mat_maker_MakerCWood,            MAT_OPAQUE, TRUE,  SURFACE_CREAKWOOD,    "Castle Planks"}, // MB_MAT_C_WOOD
    {mat_maker_MakerJRBWood,          MAT_OPAQUE, TRUE,  SURFACE_CREAKWOOD,    "Docks"},         // MB_MAT_JRB_WOOD
    {mat_maker_MakerJRBShipSide,      MAT_OPAQUE, TRUE,  SURFACE_CREAKWOOD,    "Ship Planks"},   // MB_MAT_JRB_SHIPSIDE
    {mat_maker_MakerJRBShipTop,       MAT_OPAQUE, TRUE,  SURFACE_CREAKWOOD,    "Ship Decking"},  // MB_MAT_JRB_SHIPTOP
    {mat_maker_MakerBBHHauntedPlanks, MAT_OPAQUE, TRUE,  SURFACE_CREAKWOOD,    "Spooky Planks"}, // MB_MAT_BBH_HAUNTED_PLANKS
    {mat_maker_MakerBBHRoof,          MAT_OPAQUE, FALSE, SURFACE_CREAKWOOD,    "Mansion Roof"},  // MB_MAT_BBH_ROOF
    {mat_maker_MakerSolidWood,        MAT_OPAQUE, FALSE, SURFACE_NOT_SLIPPERY, "Wood (Old)"},    // MB_MAT_SOLID_WOOD
    {mat_maker_MakerRHRWood,          MAT_OPAQUE, TRUE,  SURFACE_NOT_SLIPPERY, "Scorched Wood"}, // MB_MAT_RHR_WOOD

    // Metal
    {mat_maker_MakerBBHMetal,        MAT_OPAQUE, FALSE, SURFACE_NOT_SLIPPERY, "Metal Flooring"},   // MB_MAT_BBH_METAL
    {mat_maker_MakerJRBMetalSide,    MAT_OPAQUE, TRUE,  SURFACE_DEFAULT,      "Metal Sheet"},      // MB_MAT_JRB_METALSIDE
    {mat_maker_MakerJRBMetal,        MAT_OPAQUE, TRUE,  SURFACE_DEFAULT,      "Metal Plating"},    // MB_MAT_JRB_METAL
    {mat_maker_MakerCBasementWall,   MAT_OPAQUE, TRUE,  SURFACE_DEFAULT,      "Basement Plating"}, // MB_MAT_C_BASEMENTWALL
    {mat_maker_MakerDTiles2,         MAT_OPAQUE, TRUE,  SURFACE_DEFAULT,      "Desert Plating"},   // MB_MAT_DESERT_TILES2
    {mat_maker_MakerVRustyBlock,     MAT_OPAQUE, TRUE,  SURFACE_DEFAULT,      "Rusted Block"},     // MB_MAT_VP_RUSTYBLOCK

    // Other
    {mat_maker_MakerCCarpet,      MAT_OPAQUE, FALSE, SURFACE_GRASS,         "Carpet"},         // MB_MAT_C_CARPET
    {mat_maker_MakerCWall,        MAT_OPAQUE, TRUE,  SURFACE_DEFAULT,       "Castle Wall"},    // MB_MAT_C_WALL
    {mat_maker_MakerRoof,         MAT_OPAQUE, TRUE,  SURFACE_NOT_SLIPPERY,  "Roof"},           // MB_MAT_ROOF
    {mat_maker_MakerCRoof,        MAT_OPAQUE, TRUE,  SURFACE_NOT_SLIPPERY,  "Castle Roof"},    // MB_MAT_C_ROOF
    {mat_maker_MakerSnowRoof,     MAT_OPAQUE, TRUE,  SURFACE_VERY_SLIPPERY, "Blue Roof"},      // MB_MAT_SNOW_ROOF
    {mat_maker_MakerBBHWindow,    MAT_OPAQUE, TRUE,  SURFACE_DEFAULT,       "Window"},         // MB_MAT_BBH_WINDOW
    {mat_maker_MakerHLight,       MAT_OPAQUE, TRUE,  SURFACE_DEFAULT,       "Lantern"},        // MB_MAT_HMC_LIGHT
    {mat_maker_MakerCaution,      MAT_OPAQUE, FALSE, SURFACE_DEFAULT,       "Hazard Stripes"}, // MB_MAT_VP_CAUTION
    {mat_maker_MakerRRBlocks,     MAT_OPAQUE, TRUE,  SURFACE_DEFAULT,       "Rainbow Blocks"},  // MB_MAT_RR_BLOCKS
    {mat_maker_MakerStuddedFloor, MAT_OPAQUE, FALSE, SURFACE_NOT_SLIPPERY, "Studded Tile"},  // MB_MAT_STUDDED_TILE
    {mat_maker_MakerYellowBlock,  MAT_OPAQUE, TRUE,  SURFACE_DEFAULT,     "Yellow Block"},   // MB_MAT_TTC_BLOCK
    {mat_maker_MakerTTCSide,      MAT_OPAQUE, TRUE,  SURFACE_DEFAULT,       "Clock Platform"}, // MB_MAT_TTC_SIDE
    {mat_maker_MakerTTCWall,      MAT_OPAQUE, TRUE,  SURFACE_DEFAULT,       "Clock Exterior"}, // MB_MAT_TTC_WALL
    {mat_maker_MakerFlowerTop,    MAT_OPAQUE, FALSE, SURFACE_GRASS,         "Flowers"},        // MB_MAT_FLOWERS

    // Hazards
    {mat_maker_MakerLava,        MAT_OPAQUE, TRUE,  SURFACE_BURNING_BUBBLES,   "Lava"},           // MB_MAT_LAVA
    {mat_maker_MakerVanillaLava, MAT_OPAQUE, TRUE,  SURFACE_BURNING_BUBBLES,  "Lava (Old)"},     // MB_MAT_LAVA_OLD
    {mat_maker_MakerServerAcid,  MAT_OPAQUE, TRUE,  SURFACE_BURNING_ICE,       "Server Acid"},    // MB_MAT_SERVER_ACID
    {mat_maker_MakerBurningIce,  MAT_OPAQUE, TRUE,  SURFACE_BURNING_ICE,       "Hazard Ice"},     // MB_MAT_BURNING_ICE
    {mat_maker_MakerQuicksand,   MAT_OPAQUE, TRUE,  SURFACE_INSTANT_QUICKSAND, "Quicksand"},      // MB_MAT_QUICKSAND
    {mat_maker_MakerSlowsand,    MAT_OPAQUE, FALSE, SURFACE_DEEP_QUICKSAND,    "Slow Quicksand"}, // MB_MAT_DESERT_SLOWSAND
    {mat_maker_MakerVoid,        MAT_OPAQUE, TRUE,  SURFACE_INSTANT_QUICKSAND, "Cosmic Void"},           // MB_MAT_VP_VOID

    // Seethrough
    {mat_maker_MakerRHRMesh, MAT_CUTOUT,      TRUE,  SURFACE_HANGABLE_MESH, "Mesh"},         // MB_MAT_RHR_MESH
    {mat_maker_MakerVPMesh,  MAT_CUTOUT,      TRUE,  SURFACE_HANGABLE_MESH, "Fine Mesh"},    // MB_MAT_VP_MESH
    {mat_maker_MakerHMesh,   MAT_CUTOUT,      TRUE,  SURFACE_HANGABLE_MESH, "Red Grille"},   // MB_MAT_HMC_MESH
    {mat_maker_MakerBBHMesh, MAT_CUTOUT,      TRUE,  SURFACE_HANGABLE_MESH, "Red Mesh"},     // MB_MAT_BBH_MESH
    {mat_maker_MakerGratePink, MAT_CUTOUT,    TRUE,  SURFACE_HANGABLE_MESH, "Pink Mesh"},    // MB_MAT_PINK_MESH
    {mat_maker_MakerTTCGrate, MAT_CUTOUT,     TRUE,  SURFACE_HANGABLE_MESH, "Clock Grille"}, // MB_MAT_TTC_MESH
    {mat_maker_MakerIce,     MAT_TRANSPARENT, FALSE, SURFACE_ICE,           "Ice"},          // MB_MAT_ICE
    {mat_maker_MakerCrystal, MAT_TRANSPARENT, FALSE, SURFACE_CRYSTAL,       "Crystal"},      // MB_MAT_CRYSTAL
    {mat_maker_MakerScreen,  MAT_DECAL,       TRUE,  SURFACE_DEFAULT,       "Screen"},       // MB_MAT_VP_SCREEN

    // Retro
    {mat_maker_MakerRetroGround,     MAT_OPAQUE, TRUE, SURFACE_NOT_SLIPPERY, NULL}, // MB_MAT_RETRO_GROUND
    {mat_maker_MakerRetroBrick,      MAT_OPAQUE, TRUE, SURFACE_DEFAULT,      NULL}, // MB_MAT_RETRO_BRICKS
    {mat_maker_MakerRetroTreeTop,    MAT_OPAQUE, TRUE, SURFACE_GRASS,        NULL}, // MB_MAT_RETRO_TREETOP
    {mat_maker_MakerRetroTree,       MAT_OPAQUE, TRUE, SURFACE_DEFAULT,      NULL}, // MB_MAT_RETRO_TREEPLAT
    {mat_maker_MakerRetroBlock,      MAT_OPAQUE, TRUE, SURFACE_NOT_SLIPPERY, NULL}, // MB_MAT_RETRO_BLOCK
    {mat_maker_MakerRetroBGround,    MAT_OPAQUE, TRUE, SURFACE_NOT_SLIPPERY, NULL}, // MB_MAT_RETRO_BLUEGROUND
    {mat_maker_MakerRetroBBrick,     MAT_OPAQUE, TRUE, SURFACE_DEFAULT,      NULL}, // MB_MAT_RETRO_BLUEBRICKS
    {mat_maker_MakerRetroBBlock,     MAT_OPAQUE, TRUE, SURFACE_NOT_SLIPPERY, NULL}, // MB_MAT_RETRO_BLUEBLOCK
    {mat_maker_MakerRetroWBrick,     MAT_OPAQUE, TRUE, SURFACE_DEFAULT,      NULL}, // MB_MAT_RETRO_WHITEBRICK
    {mat_maker_MakerRetroLava,       MAT_OPAQUE, TRUE, SURFACE_BURNING,      NULL}, // MB_MAT_RETRO_LAVA
    {mat_maker_MakerRetroUnderwater, MAT_OPAQUE, TRUE, SURFACE_NOT_SLIPPERY, NULL}, // MB_MAT_RETRO_UNDERWATERGROUND

    // Minecraft
    {mat_maker_MakerMCDirt,        MAT_OPAQUE, TRUE, SURFACE_NOT_SLIPPERY, NULL}, // MB_MAT_MC_DIRT
    {mat_maker_MakerMCGrassTop,    MAT_OPAQUE, TRUE, SURFACE_GRASS,        NULL}, // MB_MAT_MC_GRASS
    {mat_maker_MakerMCCobblestone, MAT_OPAQUE, TRUE, SURFACE_NOT_SLIPPERY, NULL}, // MB_MAT_MC_COBBLESTONE
    {mat_maker_MakerMCStone,       MAT_OPAQUE, TRUE, SURFACE_DEFAULT,      NULL}, // MB_MAT_MC_STONE
    {mat_maker_MakerMCOakLogTop,   MAT_OPAQUE, TRUE, SURFACE_CREAKWOOD,    NULL}, // MB_MAT_MC_OAK_LOG_TOP
    {mat_maker_MakerMCOakLogSide,  MAT_OPAQUE, TRUE, SURFACE_CREAKWOOD,    NULL}, // MB_MAT_MC_OAK_LOG_SIDE
    {mat_maker_MakerMCOakLeaves,   MAT_CUTOUT, TRUE, SURFACE_GRASS,        NULL}, // MB_MAT_MC_OAK_LEAVES
    {mat_maker_MakerMCWoodPlanks,  MAT_OPAQUE, TRUE, SURFACE_CREAKWOOD,    NULL}, // MB_MAT_MC_WOOD_PLANKS
    {mat_maker_MakerMCSand,        MAT_OPAQUE, TRUE, SURFACE_SAND,         NULL}, // MB_MAT_MC_SAND
    {mat_maker_MakerMCBricks,      MAT_OPAQUE, TRUE, SURFACE_DEFAULT,      NULL}, // MB_MAT_MC_BRICKS
    {mat_maker_MakerMCLava,        MAT_OPAQUE, TRUE, SURFACE_BURNING,      NULL}, // MB_MAT_MC_LAVA
    {mat_maker_MakerMCFlowingLava, MAT_OPAQUE, TRUE, SURFACE_BURNING,      NULL}, // MB_MAT_MC_FLOWING_LAVA
    {mat_maker_MakerMCGlass,       MAT_CUTOUT_NOCULL, TRUE, SURFACE_VANISH_CAP_WALLS, NULL}, // MB_MAT_MC_GLASS
};

u32 mb_render_mode_table[] = {
    /* MAT_OPAQUE */        G_RM_ZB_OPA_SURF    | G_RM_ZB_OPA_SURF2,
    /* MAT_DECAL */         G_RM_TEX_EDGE_DECAL | G_RM_TEX_EDGE_DECAL2,
    /* MAT_CUTOUT */        G_RM_AA_ZB_TEX_EDGE | G_RM_AA_ZB_TEX_EDGE2,
    /* MAT_CUTOUT_NOCULL */ G_RM_AA_ZB_TEX_EDGE | G_RM_AA_ZB_TEX_EDGE2,
    /* MAT_TRANSPARENT */   G_RM_AA_ZB_XLU_SURF | G_RM_AA_ZB_XLU_SURF2,
    /* MAT_SCREEN */        G_RM_VPLEX_SCREEN   | G_RM_VPLEX_SCREEN2,
};

enum mb_fences {
    MB_FENCE_NORMAL,
    MB_FENCE_WOOD2,
    MB_FENCE_DESERT,
    MB_FENCE_BARBED,
    MB_FENCE_RHR,
    MB_FENCE_HMC,
    MB_FENCE_CASTLE,
    MB_FENCE_VIRTUAPLEX,
    MB_FENCE_BBH,
    MB_FENCE_JRB,
    MB_FENCE_SNOW2,
    MB_FENCE_SNOW,
    MB_FENCE_RETRO,
    MB_FENCE_MC,
};

Gfx *mb_fence_texs[] = {
    mat_maker_MakerFence,
    mat_maker_MakerWoodenFence,
    mat_maker_MakerDFence,
    mat_maker_MakerBarbedWire,
    mat_maker_MakerRHRFence,
    mat_maker_MakerHFence,
    mat_maker_MakerCFence,
    mat_maker_MakerVPFence,
    mat_maker_MakerBBHFence,
    mat_maker_MakerJRBFence,
    mat_maker_MakerSnowFence2,
    mat_maker_MakerSnowFence,
    mat_maker_MakerRetroFence,
    mat_maker_MakerMCFence,
};

enum mb_bars {
    MB_BAR_GENERIC,
    MB_BAR_RHR,
    MB_BAR_VP,
    MB_BAR_HMC,
    MB_BAR_BBH,
    MB_BAR_LLL,
    MB_BAR_TTC,
    MB_BAR_DESERT,
    MB_BAR_BOB,
    MB_BAR_RETRO,
    MB_BAR_MC,
};
Gfx *mb_bar_texs[][2] = {
    {mat_maker_MakerIronBars,   mat_maker_MakerIronBarsTop},
    {mat_maker_MakerRHRMesh,    mat_maker_MakerChainTop},
    {mat_maker_MakerVPMesh,     mat_maker_MakerChainTop},
    {mat_maker_MakerHMesh,      mat_maker_MakerHMCTop},
    {mat_maker_MakerBBHMesh,    mat_maker_MakerHMCTop},
    {mat_maker_MakerGratePink,  mat_maker_MakerChainTopPink},
    {mat_maker_MakerTTCGrate,   mat_maker_MakerTTCGrateTop},
    {mat_maker_MakerDMesh,      mat_maker_MakerDesertMeshTop},
    {mat_maker_MakerBOBBars,    mat_maker_MakerBOBBarsTop},
    {mat_maker_MakerRetroMesh,  mat_maker_MakerRetroMeshTop},
    {mat_maker_MakerMCIronBars, mat_maker_MakerMCIronBarsTop},
};

enum mb_water {
    MB_WATER_DEFAULT,
    MB_WATER_GREEN,
    MB_WATER_RETRO,
    MB_WATER_MC,
};

Gfx *mb_water_texs[] = {
    mat_maker_MakerWater,
    mat_maker_MakerGreenWater,
    mat_maker_MakerRetroWater,
    mat_maker_MakerMCWater,
};

struct MBTheme mb_theme_table[] = {
    // GENERIC
    {
        {
            {MB_MAT_DIRT,        MB_MAT_GRASS,     "Grass"},
            {MB_MAT_BRICKS,      MB_MAT_BRICKS,    "Bricks"},
            {MB_MAT_COBBLESTONE, MB_MAT_STONE,     "Rock"},
            {MB_MAT_TILESBRICKS, MB_MAT_TILES,     "Tiling"},
            {MB_MAT_ROOF,        MB_MAT_ROOF,      "Roof"},
            {MB_MAT_WOOD,        MB_MAT_WOOD,      "Wood"},
            {MB_MAT_SANDDIRT,    MB_MAT_SAND,      "Sand"},
            {MB_MAT_SNOWDIRT,    MB_MAT_SNOW,      "Snow"},
            {MB_MAT_LAVA,        MB_MAT_LAVA,      "Lava"},
            {MB_MAT_QUICKSAND,   MB_MAT_QUICKSAND, "Quicksand"},
        },
        MB_FENCE_NORMAL, MB_MAT_STONE, MB_BAR_GENERIC, MB_WATER_DEFAULT
    },
    // DESERT
    {
        {
            {MB_MAT_SANDDIRT,        MB_MAT_SAND,            "Sand"},
            {MB_MAT_DESERT_BRICKS,   MB_MAT_DESERT_BRICKS,   "Bricks"},
            {MB_MAT_DESERT_STONE,    MB_MAT_DESERT_STONE,    "Cobblestone"},
            {MB_MAT_DESERT_TILES,    MB_MAT_DESERT_TILES,    "Tiling"},
            {MB_MAT_DESERT_BLOCK,    MB_MAT_DESERT_BLOCK,    "Stone Block"},
            {MB_MAT_DESERT_SLOWSAND, MB_MAT_DESERT_SLOWSAND, "Slow Quicksand"},
            {MB_MAT_DESERT_BRICKS,   MB_MAT_DESERT_TILES2,   "Plating"},
            {MB_MAT_DIRT,            MB_MAT_GRASS,           "Grass"},
            {MB_MAT_LAVA,            MB_MAT_LAVA,            "Lava"},
            {MB_MAT_QUICKSAND,       MB_MAT_QUICKSAND,       "Quicksand"},
        },
        MB_FENCE_DESERT, MB_MAT_DESERT_TILES2, MB_BAR_DESERT, MB_WATER_GREEN
    },
    // LAVA
    {
        {
            {MB_MAT_RHR_STONE,   MB_MAT_RHR_OBSIDIAN, "Rock"},
            {MB_MAT_RHR_BRICK,   MB_MAT_RHR_OBSIDIAN, "Bricks"},
            {MB_MAT_RHR_BASALT,  MB_MAT_RHR_BASALT,   "Basalt"},
            {MB_MAT_RHR_TILES,   MB_MAT_RHR_TILES,    "Tiling"},
            {MB_MAT_RHR_BLOCK,   MB_MAT_RHR_BLOCK,    "Stone Block"},
            {MB_MAT_RHR_WOOD,    MB_MAT_RHR_WOOD,     "Wood"},
            {MB_MAT_RHR_PILLAR,  MB_MAT_RHR_TILES,    "Pillar"},
            {MB_MAT_RHR_MESH,    MB_MAT_RHR_MESH,     "Mesh"},
            {MB_MAT_LAVA,        MB_MAT_LAVA,         "Lava"},
            {MB_MAT_SERVER_ACID, MB_MAT_SERVER_ACID,  "Server Acid"},
        },
        MB_FENCE_RHR, MB_MAT_RHR_PILLAR, MB_BAR_RHR, MB_WATER_DEFAULT
    },
    // CAVE
    {
        {
            {MB_MAT_HMC_DIRT,      MB_MAT_HMC_GRASS,     "Grass"},
            {MB_MAT_HMC_BRICK,     MB_MAT_HMC_MAZEFLOOR, "Bricks"},
            {MB_MAT_HMC_STONE,     MB_MAT_HMC_STONE,     "Rock"},
            {MB_MAT_HMC_SLAB,      MB_MAT_HMC_TILES,     "Tiling"},
            {MB_MAT_HMC_BRICK,     MB_MAT_HMC_GRASS,     "Grassy Bricks"},
            {MB_MAT_HMC_LAKEGRASS, MB_MAT_HMC_GRASS,     "Lake Wall"},
            {MB_MAT_HMC_LIGHT,     MB_MAT_HMC_LIGHT,     "Light"},
            {MB_MAT_HMC_MESH,      MB_MAT_HMC_MESH,      "Grille"},
            {MB_MAT_LAVA,          MB_MAT_LAVA,          "Lava"},
            {MB_MAT_QUICKSAND,     MB_MAT_QUICKSAND,     "Quicksand"},
        },
        MB_FENCE_HMC, MB_MAT_HMC_LAKEGRASS, MB_BAR_HMC, MB_WATER_GREEN
    },
    // CASTLE
    {
        {
            {MB_MAT_C_WOOD,         MB_MAT_C_TILES,        "Tiling"},
            {MB_MAT_C_BRICK,        MB_MAT_C_TILES,        "Tiling (Bricks)"},
            {MB_MAT_C_STONESIDE,    MB_MAT_C_STONETOP,     "Tiling (Stone)"},
            {MB_MAT_C_WOOD,         MB_MAT_C_CARPET,       "Carpet"},
            {MB_MAT_C_ROOF,         MB_MAT_C_ROOF,         "Roof"},
            {MB_MAT_C_WALL,         MB_MAT_C_WALL,         "Castle Wall"},
            {MB_MAT_C_PILLAR,       MB_MAT_C_STONETOP,     "Pillar"},
            {MB_MAT_C_BASEMENTWALL, MB_MAT_C_BASEMENTWALL, "Basement Wall"},
            {MB_MAT_LAVA,           MB_MAT_LAVA,           "Lava"},
            {MB_MAT_C_OUTSIDEBRICK, MB_MAT_C_OUTSIDEBRICK, "Castle Bricks"},
        },
        MB_FENCE_CASTLE, MB_MAT_C_STONESIDE, MB_BAR_VP, MB_WATER_DEFAULT
    },
    // VIRTUAPLEX
    {
        {
            {MB_MAT_VP_BLOCK,      MB_MAT_VP_BLOCK,      "Block"},
            {MB_MAT_VP_TILES,      MB_MAT_VP_TILES,      "Tiling"},
            {MB_MAT_DIRT,          MB_MAT_GRASS,         "Grass"},
            {MB_MAT_VP_TILES,      MB_MAT_VP_BLUETILES,  "Blue Tiling"},
            {MB_MAT_VP_RUSTYBLOCK, MB_MAT_VP_RUSTYBLOCK, "Rusted Block"},
            {MB_MAT_VP_SCREEN,     MB_MAT_VP_SCREEN,     "Screen"},
            {MB_MAT_VP_CAUTION,    MB_MAT_VP_CAUTION,    "Hazard Stripes"},
            {MB_MAT_VP_BLOCK,      MB_MAT_SNOW,          "Snowy Block"},
            {MB_MAT_LAVA,          MB_MAT_LAVA,          "Lava"},
            {MB_MAT_VP_VOID,       MB_MAT_VP_VOID,       "Cosmic Void"},
        },
        MB_FENCE_VIRTUAPLEX, MB_MAT_VP_CAUTION, MB_BAR_VP, MB_WATER_DEFAULT
    },
    // SNOW
    {
        {
            {MB_MAT_SNOWDIRT,       MB_MAT_SNOW,             "Snow"},
            {MB_MAT_SNOW_BRICKS,    MB_MAT_SNOW_BRICK_TILES, "Bricks"},
            {MB_MAT_SNOW_ROCK,      MB_MAT_SNOW_ROCK,        "Rock"},
            {MB_MAT_SNOW_TILE_SIDE, MB_MAT_SNOW_TILES,       "Tiling"},
            {MB_MAT_SNOW_ROOF,      MB_MAT_SNOW_ROOF,        "Roof"},
            {MB_MAT_WOOD,           MB_MAT_WOOD,             "Wood"},
            {MB_MAT_CRYSTAL,        MB_MAT_CRYSTAL,          "Crystal"},
            {MB_MAT_ICE,            MB_MAT_ICE,              "Ice"},
            {MB_MAT_BURNING_ICE,    MB_MAT_BURNING_ICE,      "Hazard Ice"},
            {MB_MAT_LAVA,           MB_MAT_LAVA,             "Lava"},
        },
        MB_FENCE_SNOW, MB_MAT_SNOW_TILE_SIDE, MB_BAR_GENERIC, MB_WATER_DEFAULT
    },
    // BBH
    {
        {
            {MB_MAT_BBH_BRICKS,         MB_MAT_BBH_STONE,          "Stone Floor"},
            {MB_MAT_BBH_HAUNTED_PLANKS, MB_MAT_BBH_HAUNTED_PLANKS, "Haunted Planks"},
            {MB_MAT_BBH_STONE_PATTERN,  MB_MAT_BBH_WOOD_FLOOR,     "Wood Floor"},
            {MB_MAT_BBH_BRICKS,         MB_MAT_BBH_METAL,          "Metal Floor"},
            {MB_MAT_BBH_ROOF,           MB_MAT_BBH_ROOF,           "Roof"},
            {MB_MAT_BBH_WOOD_WALL,      MB_MAT_BBH_WOOD_WALL,      "Wood"},
            {MB_MAT_BBH_STONE,          MB_MAT_BBH_STONE,          "Wall"},
            {MB_MAT_BBH_PILLAR,         MB_MAT_BBH_STONE,          "Pillar"},
            {MB_MAT_LAVA,               MB_MAT_LAVA,               "Lava"},
            {MB_MAT_BBH_WINDOW,         MB_MAT_BBH_WINDOW,         "Window"},
        },
        MB_FENCE_BBH, MB_MAT_BBH_BRICKS, MB_BAR_BBH, MB_WATER_DEFAULT
    },
    // JRB
    {
        {
            {MB_MAT_JRB_STONE,      MB_MAT_JRB_SAND,       "Sand"},
            {MB_MAT_JRB_BRICKS,     MB_MAT_JRB_BRICKS,     "Bricks"},
            {MB_MAT_JRB_UNDERWATER, MB_MAT_JRB_UNDERWATER, "Ocean Floor"},
            {MB_MAT_JRB_TILESIDE,   MB_MAT_JRB_TILETOP,    "Tiles"},
            {MB_MAT_JRB_SHIPSIDE,   MB_MAT_JRB_SHIPTOP,    "Wood (Ship)"},
            {MB_MAT_JRB_METAL,      MB_MAT_JRB_WOOD,       "Wood (Docks)"},
            {MB_MAT_JRB_METALSIDE,  MB_MAT_JRB_METAL,      "Metal"},
            {MB_MAT_HMC_MESH,       MB_MAT_HMC_MESH,       "Grille"},
            {MB_MAT_JRB_WALL,       MB_MAT_JRB_WALL,       "Wall"},
            {MB_MAT_QUICKSAND,      MB_MAT_QUICKSAND,      "Quicksand"},
        },
        MB_FENCE_JRB, MB_MAT_VP_CAUTION, MB_BAR_HMC, MB_WATER_DEFAULT
    },
    // RETRO
    {
        {
            {MB_MAT_RETRO_GROUND,           MB_MAT_RETRO_GROUND,           "Ground"},
            {MB_MAT_RETRO_BRICKS,           MB_MAT_RETRO_BRICKS,           "Bricks"},
            {MB_MAT_RETRO_TREEPLAT,         MB_MAT_RETRO_TREETOP,          "Treetop"},
            {MB_MAT_RETRO_BLOCK,            MB_MAT_RETRO_BLOCK,            "Block"},
            {MB_MAT_RETRO_BLUEGROUND,       MB_MAT_RETRO_BLUEGROUND,       "Blue Ground"},
            {MB_MAT_RETRO_BLUEBRICKS,       MB_MAT_RETRO_BLUEBRICKS,       "Blue Bricks"},
            {MB_MAT_RETRO_BLUEBLOCK,        MB_MAT_RETRO_BLUEBLOCK,        "Blue Block"},
            {MB_MAT_RETRO_WHITEBRICK,       MB_MAT_RETRO_WHITEBRICK,       "White Bricks"},
            {MB_MAT_RETRO_LAVA,             MB_MAT_RETRO_LAVA,             "Lava"},
            {MB_MAT_RETRO_UNDERWATERGROUND, MB_MAT_RETRO_UNDERWATERGROUND, "Underwater Tile"},
        },
        MB_FENCE_RETRO, MB_MAT_RETRO_BRICKS, MB_BAR_RETRO, MB_WATER_RETRO
    },
    // CUSTOM
    {
        {
            {0, 0, "Tile 1"},
            {0, 0, "Tile 2"},
            {0, 0, "Tile 3"},
            {0, 0, "Tile 4"},
            {0, 0, "Tile 5"},
            {0, 0, "Tile 6"},
            {0, 0, "Tile 7"},
            {0, 0, "Tile 8"},
            {0, 0, "Tile 9"},
            {0, 0, "Tile 10"},
        },
        0, 0, 0, 0
    },
    // MINECRAFT
    {
        {
            {MB_MAT_MC_DIRT,         MB_MAT_MC_GRASS,       "Grass"},
            {MB_MAT_MC_COBBLESTONE,  MB_MAT_MC_COBBLESTONE, "Cobblestone"},
            {MB_MAT_MC_STONE,        MB_MAT_MC_STONE,       "Stone"},
            {MB_MAT_MC_OAK_LOG_SIDE, MB_MAT_MC_OAK_LOG_TOP, "Oak Log"},
            {MB_MAT_MC_OAK_LEAVES,   MB_MAT_MC_OAK_LEAVES,  "Oak Leaves"},
            {MB_MAT_MC_WOOD_PLANKS,  MB_MAT_MC_WOOD_PLANKS, "Oak Planks"},
            {MB_MAT_MC_SAND,         MB_MAT_MC_SAND,        "Sand"},
            {MB_MAT_MC_BRICKS,       MB_MAT_MC_BRICKS,      "Bricks"},
            {MB_MAT_MC_FLOWING_LAVA, MB_MAT_MC_LAVA,        "Lava"},
            {MB_MAT_MC_GLASS,        MB_MAT_MC_GLASS,       "Glass"},
        },
        MB_FENCE_MC, MB_MAT_MC_OAK_LOG_SIDE, MB_BAR_MC, MB_WATER_MC
    },
};

struct MBCustomTheme mb_default_custom = {
    {
        MB_MAT_DIRT,
        MB_MAT_BRICKS,
        MB_MAT_COBBLESTONE,
        MB_MAT_TILESBRICKS,
        MB_MAT_ROOF,
        MB_MAT_WOOD,
        MB_MAT_SANDDIRT,
        MB_MAT_SNOWDIRT,
        MB_MAT_LAVA,
        MB_MAT_QUICKSAND,
    },
    {
        MB_MAT_GRASS,
        MB_MAT_TILES,
        MB_MAT_STONE,
        MB_MAT_TILES,
        MB_MAT_ROOF,
        MB_MAT_WOOD,
        MB_MAT_SAND,
        MB_MAT_SNOW,
        MB_MAT_LAVA,
        MB_MAT_QUICKSAND,
    },
    {1, 0, 1, 1, 0, 0, 1, 1, 0, 0},
    MB_FENCE_NORMAL, MB_MAT_STONE, MB_BAR_GENERIC, MB_WATER_DEFAULT,
};
struct MBCustomTheme mb_curr_custom_theme;


struct MBExclamationBoxContents sExclamationBoxContents_btcm[] = {
    { 0, MODEL_MARIOS_ROCKET_BOOTS,  bhvWingCap,           0, TRUE,  0},
    { 0, MODEL_MARIOS_VANETAL_CAP,   bhvVanishCap,         2, TRUE,  0},
    { 0, MODEL_KOOPA_SHELL,          bhvKoopaShell,        3, TRUE,  0},
    { 0, MODEL_GREEN_COIN,           bhvGreenGetsSpawned,  4, FALSE, 3},
    { 0, MODEL_YELLOW_COIN,          bhvSingleCoinGetsSpawned,  4, FALSE, 1},
    { 0, MODEL_NONE,                 bhvThreeCoinsSpawn,   4, FALSE, 3},
    { 0, MODEL_NONE,                 bhvTenCoinsSpawn,     4, FALSE, 10},
};

struct MBExclamationBoxContents sExclamationBoxContents_vanilla[] = {
    { 0, MODEL_V_MARIOS_WING_CAP,  bhvWingCap,         0, TRUE,  0},
    { 6, MODEL_V_MARIOS_METAL_CAP, bhvMetalCap,        1, TRUE,  0},
    { 0, MODEL_MARIOS_CAP,         bhvVanishCap,       2, TRUE,  0},
    { 0, MODEL_KOOPA_SHELL,        bhvKoopaShell,      3, TRUE,  0},
    { 0, MODEL_YELLOW_COIN,        bhvSingleCoinGetsSpawned, 3, FALSE, 1},
    { 0, MODEL_NONE,               bhvThreeCoinsSpawn, 3, FALSE, 3},
    { 0, MODEL_NONE,               bhvTenCoinsSpawn,   3, FALSE, 10},
};

/*  Object Type                  Name                       Button GFX              Behavior           Y Offset     Model                      Flags                                  Coins/Objs/Scale  Anims   Display Func    Sound*/
struct MBObjectInfo mb_object_type_list[] = {
/* OBJECT_TYPE_SETTINGS */      {"Level Settings",          mat_b_btn_settings},
/* OBJECT_TYPE_1 */             {" ",                       mat_b_btn_settings}, // previously screenshot
/* OBJECT_TYPE_STAR */          {"Normal",                  mat_b_btn_star,         bhvStar,           TILE_SIZE/2, MODEL_STAR,                OBJ_TYPE_STAR,           OBJ_OCCUPY_INNER, 0, 0, 1.0f, NULL, df_star, SOUND_MENU_STAR_SOUND | SOUND_VIBRATO},
/* OBJECT_TYPE_RED_COIN_STAR */ {"Red Coins",               mat_b_btn_rcs,          bhvHiddenRedCoinStar, TILE_SIZE/2, MODEL_TRANSPARENT_STAR, OBJ_TYPE_STAR,           OBJ_OCCUPY_INNER, 0, 1, 1.0f, NULL, df_reds_marker, SOUND_MENU_STAR_SOUND | SOUND_VIBRATO},
/* OBJECT_TYPE_GOOMBA */        {"Normal",                  mat_b_btn_goomba,       bhvGoomba,         0,           MODEL_GOOMBA,              OBJ_TYPE_IMBUABLE,       OBJ_OCCUPY_INNER, 1, 0, 1.5f, goomba_seg8_anims_0801DA4C, NULL, SOUND_OBJ_GOOMBA_PREVIEW},
/* OBJECT_TYPE_BIG_GOOMBA */    {"Huge",                    mat_b_btn_goomba_b,     bhvBigGoomba,      0,           MODEL_GOOMBA,              OBJ_TYPE_IMBUABLE,       OBJ_OCCUPY_INNER, 5, 0, 3.5f, goomba_seg8_anims_0801DA4C, NULL, SOUND_OBJ_GOOMBA_PREVIEW},
/* OBJECT_TYPE_TINY_GOOMBA */   {"Tiny",                    mat_b_btn_goomba_s,     bhvTinyGoomba,     0,           MODEL_GOOMBA,              OBJ_TYPE_IMBUABLE,       OBJ_OCCUPY_INNER, 1, 0, 0.5f, goomba_seg8_anims_0801DA4C, NULL, SOUND_OBJ_GOOMBA_PREVIEW},
/* OBJECT_TYPE_PIRANHA_PLANT */ {"Normal",                  mat_b_btn_plant,        bhvPiranhaPlant,   0,           MODEL_MAKER_PLANT,         OBJ_TYPE_IMBUABLE,       OBJ_OCCUPY_INNER, 5, 1, 1.0f, piranha_plant_seg6_anims_0601C31C, df_piranha, SOUND_OBJ2_PIRANHA_PLANT_BITE},
/* OBJECT_TYPE_BIG_PIRANHA_PLANT */  {"Huge",               mat_b_btn_plant_b,      bhvFirePiranhaPlantBig, 0,      MODEL_MAKER_PLANT,         OBJ_TYPE_IMBUABLE,       OBJ_OCCUPY_INNER, 2, 0, 2.0f, piranha_plant_seg6_anims_0601C31C, NULL, SOUND_OBJ2_PIRANHA_PLANT_BITE},
/* OBJECT_TYPE_TINY_PIRANHA_PLANT */ {"Tiny",               mat_b_btn_plant_s,      bhvFirePiranhaPlant, 0,         MODEL_MAKER_PLANT,         OBJ_TYPE_IMBUABLE,       OBJ_OCCUPY_INNER, 1, 0, 0.5f, piranha_plant_seg6_anims_0601C31C, NULL, SOUND_OBJ2_PIRANHA_PLANT_BITE},
/* OBJECT_TYPE_KOOPA */         {"Koopa",                   mat_b_btn_kuppa,        bhvScaredKoopa,    0,           MODEL_KOOPA_WITH_SHELL,    OBJ_TYPE_IMBUABLE,       OBJ_OCCUPY_INNER, 5, 0, 1.5f, koopa_seg6_anims_06011364, df_koopa, SOUND_OBJ_KOOPA_TALK},
/* OBJECT_TYPE_COIN */          {"Yellow Coin",             mat_b_btn_coin,         bhvYellowCoin,     0,           MODEL_YELLOW_COIN,         OBJ_TYPE_BILLBOARD | OBJ_TYPE_IMBUABLE_TRIGGER, OBJ_OCCUPY_INNER, 1, 0, 1.0f, NULL, NULL, SOUND_GENERAL_COIN | SOUND_VIBRATO},
/* OBJECT_TYPE_GREEN_COIN */    {"Green Coin",              mat_b_btn_greencoin,    bhvGreenCoin,      0,           MODEL_GREEN_COIN,          OBJ_TYPE_BILLBOARD | OBJ_TYPE_IMBUABLE_TRIGGER, OBJ_OCCUPY_INNER, 3, 0, 1.0f, NULL, NULL, SOUND_GENERAL_COIN_MULTI},
/* OBJECT_TYPE_RED_COIN */      {"Red Coin",                mat_b_btn_redcoin,      bhvRedCoin,        0,           MODEL_RED_COIN,            OBJ_TYPE_BILLBOARD,      OBJ_OCCUPY_INNER, 2, 0, 1.0f, NULL, NULL, SOUND_MENU_COLLECT_RED_COIN},
/* OBJECT_TYPE_BLUE_COIN */     {"Coin",                    mat_b_btn_bluecoin,     bhvHiddenBlueCoin, 0,           MODEL_BLUE_COIN,           OBJ_TYPE_BILLBOARD | OBJ_TYPE_IMBUABLE_TRIGGER, OBJ_OCCUPY_INNER, 5, 0, 1.0f, NULL, NULL, SOUND_GENERAL_COIN_MULTI},
/* OBJECT_TYPE_BLUE_COIN_SWITCH */ {"Switch",               mat_b_btn_bcs,          bhvBlueCoinSwitch, 0,           MODEL_BLUE_COIN_SWITCH,    OBJ_TYPE_IMBUABLE_TRIGGER, OBJ_OCCUPY_INNER, 0, 0, 3.0f, NULL, NULL, SOUND_GENERAL2_PURPLE_SWITCH},
/* OBJECT_TYPE_NOTEBLOCK */     {"Noteblock",               mat_b_btn_noteblock,    bhvNoteblock,      TILE_SIZE/2, MODEL_NOTEBLOCK,           0,                       OBJ_OCCUPY_INNER, 0, 0, 1.27f,NULL, NULL, SOUND_GENERAL_CRAZY_BOX_BOING_SLOW},
/* OBJECT_TYPE_BOBOMB */        {"Bob-omb",                 mat_b_btn_bobomb,       bhvBobomb,         0,           MODEL_BLACK_BOBOMB,        OBJ_TYPE_IMBUABLE,       OBJ_OCCUPY_INNER, 1, 0, 1.0f, bobomb_anims, NULL, SOUND_GENERAL2_QUIET_EXPLOSION},
/* OBJECT_TYPE_CHUCKYA */       {"Chuckya",                 mat_b_btn_chuckya,      bhvChuckya,        0,           MODEL_CHUCKYA,             OBJ_TYPE_IMBUABLE,       OBJ_OCCUPY_INNER, 5, 0, 2.0f, chuckya_seg8_anims_0800C070, df_chuckya, SOUND_OBJ_CHUCKYA_PREVIEW},
/* OBJECT_TYPE_BULLY */         {"Bully",                   mat_b_btn_bully,        bhvSmallBully,     0,           MODEL_BULLY,               OBJ_TYPE_IMBUABLE,       OBJ_OCCUPY_INNER, 1, 0, 1.0f, bully_seg5_anims_0500470C, NULL, SOUND_OBJ2_SMALL_BULLY_ATTACKED},
/* OBJECT_TYPE_CHILL_BULLY */   {"Chill Bully",             mat_b_btn_chillbully,   bhvSmallBully,     0,           MODEL_CHILL_BULLY,         OBJ_TYPE_IMBUABLE,       OBJ_OCCUPY_INNER, 1, 0, 1.0f, bully_seg5_anims_0500470C, NULL, SOUND_OBJ2_SMALL_BULLY_ATTACKED},
/* OBJECT_TYPE_BULLET_BILL */   {"Bullet Bill",             mat_b_btn_bill,         bhvBulletBillCannon, TILE_SIZE/2, MODEL_BILL_MAKER_2,      0,                       OBJ_OCCUPY_FULL,  0, 1, 1.0f, NULL, NULL, SOUND_OBJ_POUNDING_CANNON},
/* OBJECT_TYPE_HEAVE_HO */      {"Heave-Ho",                mat_b_btn_heaveho,      bhvHeaveHo,        0,           MODEL_MAKER_HEAVEHO,       OBJ_TYPE_IMBUABLE | OBJ_TYPE_IMBUABLE_COINS, OBJ_OCCUPY_INNER, 0, 0, 2.0f, heave_ho_seg5_anims_0501534C, NULL, SOUND_OBJ_HEAVEHO_PREVIEW},
/* OBJECT_TYPE_MOTOS */         {"Motos",                   mat_b_btn_motos,        bhvMotos,          0,           MODEL_MAKER_MOTOS,         OBJ_TYPE_IMBUABLE,       OBJ_OCCUPY_INNER, 5, 0, 2.0f, motos_anime, NULL, SOUND_OBJ_HEAVEHO_PREVIEW},
/* OBJECT_TYPE_TREE */          {"Tree",                    mat_b_btn_tree,         bhvTree,           0,           MODEL_MAKER_TREE_1,        OBJ_TYPE_BILLBOARD,      OBJ_OCCUPY_INNER, 0, 0, 1.0f, NULL, df_tree, SOUND_ACTION_CLIMB_UP_TREE | SOUND_VIBRATO},
/* OBJECT_TYPE_EXCL_BOX */      {"Item Box",                mat_b_btn_excla,        bhvExclamationBox, TILE_SIZE/2, MODEL_EXCLAMATION_BOX,     OBJ_TYPE_IMBUABLE,       OBJ_OCCUPY_INNER, 0, 0, 2.0f, NULL, df_exbox, SOUND_GENERAL_BOX_PREVIEW},
/* OBJECT_TYPE_SPAWN */         {"Mario Spawn",             mat_b_btn_spawn,        bhvSpawn,          TILE_SIZE/2, MODEL_SPAWN,               0,                       OBJ_OCCUPY_INNER, 0, 0, 1.0f, NULL, df_hide_during_screenshot, SOUND_MENU_STAR_SOUND_LETS_A_GO},
/* OBJECT_TYPE_REX */           {"Rex",                     mat_b_btn_rex,          bhvRex,            0,           MODEL_REX,                 OBJ_TYPE_IMBUABLE,       OBJ_OCCUPY_INNER, 2, 0, 1.5f, Rex_anims, NULL, SOUND_OBJ_GOOMBA_PREVIEW},
/* OBJECT_TYPE_PODOBOO */       {"Podoboo",                 mat_b_btn_podoboo,      bhvPodoboo,        TILE_SIZE/2, MODEL_PODOBOO,             0,                       OBJ_OCCUPY_INNER, 0, 0, 1.0f, NULL, df_podoboo, SOUND_OBJ_FLAME_BLOWN},
/* OBJECT_TYPE_CRABLET */       {"Crablet",                 mat_b_btn_crablet,      bhvCrablet,        0,           MODEL_MAKER_CRABLET,       OBJ_TYPE_IMBUABLE,       OBJ_OCCUPY_INNER, 4, 0, 1.0f, crab_anims_anims, NULL, SOUND_OBJ2_SCUTTLEBUG_ALERT},
/* OBJECT_TYPE_HAMMER_BRO */    {"Normal",                  mat_b_btn_hammerbro,    bhvHammerBro,      0,           MODEL_HAMMER_BRO,          OBJ_TYPE_IMBUABLE,       OBJ_OCCUPY_INNER, 6, 0, 1.0f, Hammerbro_anims, df_hammerbro, SOUND_OBJ_KOOPA_DAMAGE},
/* OBJECT_TYPE_FIRE_BRO */      {"Fire Bro",                mat_b_btn_firebro,      bhvFireBro,        0,           MODEL_HAMMER_BRO,          OBJ_TYPE_IMBUABLE,       OBJ_OCCUPY_INNER, 6, 0, 1.0f, Hammerbro_anims, df_firebro, SOUND_OBJ_KOOPA_DAMAGE},
/* OBJECT_TYPE_CHICKEN */       {"Chicken",                 mat_b_btn_chicken,      bhvChicken,        TILE_SIZE/2, MODEL_MAKER_CHICKEN,       OBJ_TYPE_IMBUABLE,       OBJ_OCCUPY_INNER, 2, 0, 1.0f, chicken_anims, NULL, SOUND_OBJ_BOO_LAUGH_SHORT},
/* OBJECT_TYPE_PHANTASM */      {"Cosmic Phantasm",         mat_b_btn_phantasm,     bhvPhantasm,       0,           MODEL_MARIO,               OBJ_TYPE_IMBUABLE,       OBJ_OCCUPY_INNER, 5, 9, 1.0f, &evil_mario_anims[2], NULL, SOUND_ACTION_METAL_STEP | SOUND_VIBRATO},
/* OBJECT_TYPE_WARP_PIPE */     {"Warp Pipe",               mat_b_btn_pipe,         bhvStaticObject,   0,           MODEL_MAKER_PIPE,          0,                       OBJ_OCCUPY_INNER, 0, 0, 1.0f, NULL, NULL, SOUND_MENU_ENTER_PIPE | SOUND_VIBRATO},
/* OBJECT_TYPE_BADGE */         {"Badge",                   mat_b_btn_badge,        bhvBadge,          TILE_SIZE/2, MODEL_BADGE,               OBJ_TYPE_BILLBOARD | OBJ_TYPE_IMBUABLE_TRIGGER, OBJ_OCCUPY_INNER, 0, 0, 1.0f, NULL, NULL, SOUND_GENERAL2_PURPLE_SWITCH},
/* OBJECT_TYPE_KING_BOBOMB */   {"King Bob-omb",            mat_b_btn_boss_kb,      bhvKingBobomb,     0,           MODEL_KINGBOMB_MAKER,      OBJ_TYPE_IMBUABLE,       OBJ_OCCUPY_FULL,  0, 0, 1.0f, king_bobomb_seg5_anims_0500FE30, df_kingbomb, SOUND_OBJ_KING_BOBOMB_POUNDING1_HIGHPRIO},
/* OBJECT_TYPE_KING_WHOMP */    {"King Whomp",              mat_b_btn_boss_whomp,   bhvWhompKingBoss,  0,           MODEL_WHOMP_MAKER,         OBJ_TYPE_IMBUABLE,       OBJ_OCCUPY_FULL,  0, 1, 2.0f, whomp_seg6_anims_06020A04, NULL, SOUND_OBJ_WHOMP},
/* OBJECT_TYPE_BIG_BOO */       {"Big Boo",                 mat_b_btn_boss_boo,     bhvBalconyBigBoo,  TILE_SIZE,   MODEL_MAKER_BOO,           OBJ_TYPE_IMBUABLE | OBJ_TYPE_IMBUABLE_COINS, OBJ_OCCUPY_FULL,  0, 0, 3.0f, NULL, df_boo, SOUND_OBJ_BOO_LAUGH_LONG},
/* OBJECT_TYPE_BIG_BULLY */     {"Big Bully",               mat_b_btn_boss_bully,   bhvBigBully,       0,           MODEL_BULLY,               OBJ_TYPE_IMBUABLE,       OBJ_OCCUPY_FULL,  0, 0, 2.0f, bully_seg5_anims_0500470C, NULL, SOUND_OBJ2_LARGE_BULLY_ATTACKED},
/* OBJECT_TYPE_BIG_CHILL_BULLY */ {"Big Chill Bully",       mat_b_btn_boss_chillbully,bhvBigBully,     0,           MODEL_CHILL_BULLY,         OBJ_TYPE_IMBUABLE,       OBJ_OCCUPY_FULL,  0, 0, 2.0f, bully_seg5_anims_0500470C, NULL, SOUND_OBJ2_LARGE_BULLY_ATTACKED},
/* OBJECT_TYPE_WIGGLER */       {"Wiggler",                 mat_b_btn_boss_wiggler, bhvWigglerHead,    0,           MODEL_WIGGLER_HEAD,        OBJ_TYPE_IMBUABLE,       OBJ_OCCUPY_FULL,  0, 5, 4.0f, wiggler_seg5_anims_0500EC8C, df_wiggler, SOUND_OBJ_WIGGLER_TALK},
/* OBJECT_TYPE_BOWSER */        {"Bowser",                  mat_b_btn_boss_bowser,  bhvBowser,         0,           MODEL_MAKER_BOWSER,        OBJ_TYPE_IMBUABLE,       OBJ_OCCUPY_FULL,  0, 24, 1.0f, bowser_anims, df_booser, SOUND_OBJ2_BOWSER_ROAR},
/* OBJECT_TYPE_PLATFORM_TRACK */ {"Activated",              mat_b_btn_checker,      bhvPlatformOnTrack, TILE_SIZE/2, MODEL_CHECKERBOARD_PLATFORM, OBJ_TYPE_TRAJECTORY,  OBJ_OCCUPY_INNER, 0, 0, 1.0f, NULL, NULL, SOUND_ENV_ELEVATOR1 | SOUND_VIBRATO},
/* OBJECT_TYPE_PLATFORM_LOOPING */ {"Looping",              mat_b_btn_checker,      bhvLoopingPlatform, TILE_SIZE/2, MODEL_LOOPINGP,           OBJ_TYPE_TRAJECTORY,     OBJ_OCCUPY_INNER, 0, 0, 1.0f, NULL, NULL, SOUND_ENV_ELEVATOR1 | SOUND_VIBRATO},
/* OBJECT_TYPE_BOWLING_BALL */  {"Bowling Ball",            mat_b_btn_bball,        bhvBobBowlingBallSpawner, TILE_SIZE/2, MODEL_BOWLING_BALL, OBJ_TYPE_BILLBOARD | OBJ_TYPE_TRAJECTORY, OBJ_OCCUPY_INNER, 0, 0, 1.0f, NULL, NULL, SOUND_GENERAL_QUIET_POUND1 | SOUND_VIBRATO},
/* OBJECT_TYPE_KOOPA_THE_QUICK */ {"Koopa the Quick",       mat_b_btn_kuppaq,       bhvKoopa,          0,           MODEL_KOOPA_WITH_SHELL,    OBJ_TYPE_TRAJECTORY | OBJ_TYPE_STAR, OBJ_OCCUPY_FULL, 0, 1, 3.0f, koopa_seg6_anims_06011364, df_ktq, SOUND_OBJ_KOOPA_TALK},
/* OBJECT_TYPE_PURPLE_SWITCH */ {"Purple Switch",           mat_b_btn_purpleswitch, bhvFloorSwitchHiddenObjects, 0, MODEL_PURPLE_SWITCH,       0,                       OBJ_OCCUPY_INNER, 0, 0, 1.28f,NULL, NULL, SOUND_GENERAL2_PURPLE_SWITCH},
/* OBJECT_TYPE_TIMED_BOX */     {"Breakable",               mat_b_btn_tbox,         bhvHiddenObject,   0,           MODEL_BREAKABLE_BOX,       0,                       OBJ_OCCUPY_OUTER, 0, 0, 1.0f, NULL, df_timedbox, SOUND_GENERAL2_SWITCH_TICK_FAST},
/* OBJECT_TYPE_RECOVERY_HEART */ {"Recovery Heart",         mat_b_btn_heart,        bhvRecoveryHeart,  TILE_SIZE/2, MODEL_HEART,               0,                       OBJ_OCCUPY_INNER, 0, 0, 1.0f, NULL, df_heart, SOUND_GENERAL_HEART_SPIN},
/* OBJECT_TYPE_TEST_MARIO */    {"Save & Test",             mat_b_btn_check,        NULL,              0,           MODEL_MARIO,               0,                       OBJ_OCCUPY_FULL,  0, 0, 1.0f, mario_btcm_anims, NULL, 0},
/* OBJECT_TYPE_THWOMP */        {"Thwomp",                  mat_b_btn_thwomp,       bhvThwomp,         0,           MODEL_THWOMP_MAKER,        0,                       OBJ_OCCUPY_FULL,  0, 2, 1.5f, NULL, NULL, SOUND_OBJ_THWOMP},
/* OBJECT_TYPE_WHOMP */         {"Whomp",                   mat_b_btn_whomp,        bhvSmallWhomp,     0,           MODEL_WHOMP_MAKER,         OBJ_TYPE_IMBUABLE,       OBJ_OCCUPY_FULL, 10, 1, 1.0f, whomp_seg6_anims_06020A04, NULL, SOUND_OBJ_WHOMP},
/* OBJECT_TYPE_GRINDEL */       {"Grindel",                 mat_b_btn_grindel,      bhvGrindel,        0,           MODEL_MAKER_GRINDEL,       0,                       OBJ_OCCUPY_FULL,  0, 2, 1.0f, NULL, df_grindel, SOUND_OBJ_KING_BOBOMB_JUMP},
/* OBJECT_TYPE_LAKITU */        {"Lakitu",                  mat_b_btn_lakitu,       bhvEnemyLakitu,    TILE_SIZE/2, MODEL_LAKITU_MAKER,        OBJ_TYPE_IMBUABLE,       OBJ_OCCUPY_INNER, 5, 5, 1.0f, lakitu_enemy_seg5_anims_050144D4, df_lakitu, SOUND_OBJ_EVIL_LAKITU_THROW},
/* OBJECT_TYPE_FLY_GUY */       {"Fly Guy",                 mat_b_btn_flyguy,       bhvFlyGuy,     TILE_SIZE/2, MODEL_FLYGUY,              OBJ_TYPE_IMBUABLE,       OBJ_OCCUPY_INNER, 2, 0, 1.5f, flyguy_seg8_anims_08011A64, df_flyguy, SOUND_OBJ_KOOPA_FLYGUY_DEATH},
/* OBJECT_TYPE_SNUFIT */        {"Snufit",                  mat_b_btn_snufit,       bhvSnufit,         TILE_SIZE/2, MODEL_MAKER_SNUFIT,        OBJ_TYPE_IMBUABLE,       OBJ_OCCUPY_INNER, 2, 0, 1.0f, NULL, df_snufit, SOUND_OBJ_SNUFIT_SHOOT},
/* OBJECT_TYPE_AMP */           {"Amp",                     mat_b_btn_amp,          bhvCirclingAmp,    TILE_SIZE/2, MODEL_AMP,                 0,                       OBJ_OCCUPY_OUTER, 0, 0, 1.0f, amp_anims, df_circling_amp, SOUND_AIR_AMP_PREVIEW},
/* OBJECT_TYPE_BOO */           {"Boo",                     mat_b_btn_boo,          bhvBoo,            TILE_SIZE/2, MODEL_MAKER_BOO,           OBJ_TYPE_IMBUABLE | OBJ_TYPE_IMBUABLE_COINS,  OBJ_OCCUPY_INNER, 0, 0, 1.0f, NULL, df_boo, SOUND_OBJ_BOO_LAUGH_LONG},
/* OBJECT_TYPE_MR_I */          {"Mr. I",                   mat_b_btn_mri,          bhvMrI,            0,           MODEL_MAKER_MRI,           OBJ_TYPE_IMBUABLE | OBJ_TYPE_BILLBOARD, OBJ_OCCUPY_INNER, 5, 1, 1.0f, NULL, df_mri, SOUND_OBJ_MRI_SHOOT},
/* OBJECT_TYPE_SCUTTLEBUG */    {"Scuttlebug",              mat_b_btn_scuttlebug,   bhvScuttlebug,     0,           MODEL_MAKER_SCUTTLEBUG,    OBJ_TYPE_IMBUABLE,       OBJ_OCCUPY_INNER, 3, 0, 1.0f, scuttlebug_seg6_anims_06015064, NULL, SOUND_OBJ2_SCUTTLEBUG_ALERT},
/* OBJECT_TYPE_BOWSER_BOMB */   {"Bowser Bomb",             mat_b_btn_bbomb,        bhvBowserBomb,     TILE_SIZE/2, MODEL_MAKER_BOWSER_BOMB,   0,                       OBJ_OCCUPY_INNER, 0, 0, 1.0f, NULL, NULL, SOUND_GENERAL2_QUIET_EXPLOSION},
/* OBJECT_TYPE_FIRE_SPINNER */  {"Fire Spinner",            mat_b_btn_firebar,      bhvLllRotatingBlockWithFireBars, 0, MODEL_MAKER_FIREBAR,   0,                       OBJ_OCCUPY_INNER, 0, 0, 1.0f, NULL, df_fire_spinner, SOUND_OBJ_FLAME_BLOWN},
/* OBJECT_TYPE_COIN_FORMATION */ {"Coin Formation",         mat_b_btn_cformation,   bhvCoinFormation,  0,           MODEL_NONE,                0,                       OBJ_OCCUPY_FULL,  0, 0, 1.0f, NULL, df_coin_formation, SOUND_GENERAL_COIN_MULTI},
/* OBJECT_TYPE_RED_FLAME */     {"Red",                     mat_b_btn_fire_red,     bhvFlame,          90,          MODEL_RED_FLAME,           OBJ_TYPE_BILLBOARD,      OBJ_OCCUPY_INNER, 0, 0, 7.0f, NULL, df_flame, SOUND_OBJ_FLAME_BLOWN},
/* OBJECT_TYPE_BLUE_FLAME */    {"Blue",                    mat_b_btn_fire_blue,    bhvFlame,          90,          MODEL_BLUE_FLAME,          OBJ_TYPE_BILLBOARD,      OBJ_OCCUPY_INNER, 0, 0, 7.0f, NULL, df_flame, SOUND_OBJ_FLAME_BLOWN},
/* OBJECT_TYPE_FIRE_SPITTER */  {"Fire Spitter",            mat_b_btn_firespitter,  bhvFireSpitter,    TILE_SIZE/2, MODEL_BOWLING_BALL,        OBJ_TYPE_BILLBOARD,      OBJ_OCCUPY_INNER, 0, 0, 0.2f, NULL, NULL, SOUND_OBJ_FLAME_BLOWN},
/* OBJECT_TYPE_FLAMETHROWER */  {"Flamethrower",            mat_b_btn_flamethrower, bhvFlamethrower,   TILE_SIZE/2, MODEL_MAKER_FLAMETHROWER,  0,                       OBJ_OCCUPY_FULL,  0, 5, 1.0f, NULL, NULL, SOUND_OBJ_FLAME_BLOWN},
/* OBJECT_TYPE_SPINDRIFT */     {"Spindrift",               mat_b_btn_spindrift,    bhvSpindrift,      0,           MODEL_MAKER_SPINDRIFT,     OBJ_TYPE_IMBUABLE,       OBJ_OCCUPY_INNER, 3, 0, 1.0f, spindrift_seg5_anims_05002D68, NULL, SOUND_ACTION_TWIRL},
/* OBJECT_TYPE_MR_BLIZZARD */   {"Mr. Blizzard",            mat_b_btn_blizzard,     bhvMrBlizzard,     0,           MODEL_MAKER_BLIZZARD,      OBJ_TYPE_IMBUABLE,       OBJ_OCCUPY_INNER, 3, 0, 1.0f, snowman_seg5_anims_0500D118, df_blizzard, SOUND_OBJ2_SCUTTLEBUG_ALERT},
/* OBJECT_TYPE_MONEYBAG */      {"Moneybag",                mat_b_btn_moneybag,     bhvMoneybagHidden, 0,           MODEL_MONEYBAG,            OBJ_TYPE_IMBUABLE | OBJ_TYPE_IMBUABLE_COINS, OBJ_OCCUPY_INNER, 0, 1, 1.0f, moneybag_seg6_anims_06005E5C, df_moneybag, SOUND_GENERAL_MONEYBAG_BOING_LOWPRIO},
/* OBJECT_TYPE_SKEETER */       {"Skeeter",                 mat_b_btn_skeeter,      bhvSkeeter,        0,           MODEL_MAKER_SKEETER,       OBJ_TYPE_IMBUABLE,       OBJ_OCCUPY_INNER, 3, 0, 1.0f, skeeter_seg6_anims_06007DE0, df_skeeter, SOUND_OBJ_SKEETER_WALK},
/* OBJECT_TYPE_POKEY */         {"Pokey",                   mat_b_btn_pokey,        bhvPokey,          0,           MODEL_MAKER_POKEY_HEAD,    OBJ_TYPE_IMBUABLE | OBJ_TYPE_BILLBOARD, OBJ_OCCUPY_INNER, 5, 5, 3.0f, NULL, df_pokey, SOUND_OBJ_POKEY_DEATH},
/* OBJECT_TYPE_BBOX_SMALL*/     {"Throwable Box",           mat_b_btn_sbox,         bhvBreakableBoxSmall, 0,        MODEL_BREAKABLE_BOX_SMALL, 0,                       OBJ_OCCUPY_INNER, 3, 0, 0.31f,NULL, df_corkbox, SOUND_GENERAL_BOX_PREVIEW},
/* OBJECT_TYPE_BBOX_NORMAL*/    {"Breakable Box",           mat_b_btn_corkbox,      bhvBreakableBox,   0,           MODEL_BREAKABLE_BOX,       OBJ_TYPE_IMBUABLE | OBJ_TYPE_IMBUABLE_COINS, OBJ_OCCUPY_FULL, 0, 0, 1.0f, NULL, df_corkbox, SOUND_GENERAL_BOX_PREVIEW},
/* OBJECT_TYPE_BBOX_CRAZY*/     {"Crazy Box",               mat_b_btn_cbox,         bhvJumpingBox,     0,           MODEL_BREAKABLE_BOX_SMALL, 0,                       OBJ_OCCUPY_INNER, 5, 0, 0.38f,NULL, df_crazybox, SOUND_GENERAL_CRAZY_BOX_BOING_SLOW},
/* OBJECT_TYPE_DIAMOND*/        {"Water Diamond",           mat_b_btn_water_diamond,bhvWaterLevelDiamond,0,         MODEL_MAKER_DIAMOND,       0,                       OBJ_OCCUPY_INNER, 0, 0, 1.0f, NULL, NULL, SOUND_GENERAL_WATER_LEVEL_TRIG},
/* OBJECT_TYPE_SIGN */          {"Bill Board",              mat_b_btn_sign,         bhvMessagePanel,   0,           MODEL_WOODEN_SIGNPOST,     OBJ_TYPE_HAS_DIALOG,     OBJ_OCCUPY_INNER, 0, 0, 1.0f, NULL, NULL, SOUND_ACTION_READ_SIGN},
/* OBJECT_TYPE_BUDDY */         {"Bob-omb Buddy",           mat_b_btn_buddy,        bhvBobombBuddy,    0,           MODEL_BOBOMB_BUDDY,        OBJ_TYPE_HAS_DIALOG,     OBJ_OCCUPY_INNER, 0, 0, 1.0f, bobomb_anims, NULL, SOUND_OBJ_BOBOMB_BUDDY_TALK},
/* OBJECT_TYPE_BUTTON */        {"On-Off Button",           mat_b_btn_switch,       bhvOnOffButton,    0,           MODEL_MAKER_BUTTON,        0,                       OBJ_OCCUPY_INNER, 0, 0, 1.0f, NULL, df_button, SOUND_GENERAL_BUTTON_PRESS},
/* OBJECT_TYPE_ON_OFF_BLOCK */  {"On-Off Block",            mat_b_btn_block,        bhvOnOffBlock,     0,           MODEL_MAKER_BLOCK_OFF,     0,                       OBJ_OCCUPY_OUTER, 0, 0, 1.0f, NULL, df_block, SOUND_GENERAL_BUTTON_PRESS},
/* OBJECT_TYPE_WOODPLAT */      {"Wooden Platform",         mat_b_btn_woodplat,     bhvWoodPlat,       0,           MODEL_MAKER_WOODPLAT,      0,                       OBJ_OCCUPY_FULL,  0, 1, 1.0f, NULL, df_woodplat, SOUND_ACTION_TERRAIN_STEP + (SOUND_TERRAIN_SPOOKY << 16)},
/* OBJECT_TYPE_RFBOX */         {"Reinforced Box",          mat_b_btn_rfbox,        bhvBreakableBoxRF, 0,           MODEL_MAKER_RFBOX,         OBJ_TYPE_IMBUABLE | OBJ_TYPE_IMBUABLE_COINS, OBJ_OCCUPY_FULL, 0, 0, 1.0f, NULL, NULL, SOUND_OBJ_BULLY_METAL},
/* OBJECT_TYPE_CULL_PREVIEW */  {"",                        mat_b_btn_cull,         bhvStaticObject,   TILE_SIZE/2, MODEL_CULL_MARKER,         OBJ_TYPE_BILLBOARD,      OBJ_OCCUPY_INNER, 0, 0, 1.f,  NULL, NULL, 0},
/* OBJECT_TYPE_SHOWRUNNER */    {"Showrunner",              mat_b_btn_showrunner,   bhvShowrunner,     0,           MODEL_MAKER_SHOWRUNNER,    OBJ_TYPE_IMBUABLE,       OBJ_OCCUPY_FULL,  50,39,1.0f, showrunner_anims, NULL, SOUND_OBJ_MRI_SHOOT},
/* OBJECT_TYPE_CROWBAR */       {"Crowbar",                 mat_b_btn_pipebar,      bhvCrowbarPower,   TILE_SIZE/2, MODEL_MAKER_CROWBAR,       0,                       OBJ_OCCUPY_INNER, 0, 0, 1.0f, NULL, df_crowbar, SOUND_MENU_EXIT_PIPE},
/* OBJECT_TYPE_MASK    */       {"Bullet Bill Mask",        mat_b_btn_mask,         bhvBMask,          TILE_SIZE/2, MODEL_MAKER_MASK,          0,                       OBJ_OCCUPY_INNER, 0, 0, 1.0f, NULL, df_mask, SOUND_MENU_EXIT_PIPE},
/* OBJECT_TYPE_TOAD */          {"Toad",                    mat_b_btn_toad,         bhvMakerToad,      0,           MODEL_MAKER_TOAD,          OBJ_TYPE_HAS_DIALOG,     OBJ_OCCUPY_INNER, 0, 0, 1.0f, toad_seg6_anims_0600FB58, df_toad, SOUND_ACTION_READ_SIGN},
/* OBJECT_TYPE_TUXIE */         {"Tuxie",                   mat_b_btn_tuxie,        bhvMakerTuxie,     0,           MODEL_MAKER_TUXIE,         OBJ_TYPE_HAS_DIALOG,     OBJ_OCCUPY_INNER, 0, 0, 1.0f, penguin_seg5_anims_05008B74, NULL, SOUND_OBJ2_BABY_PENGUIN_YELL},
/* OBJECT_TYPE_UKIKI */         {"Ukiki",                   mat_b_btn_ukiki,        bhvMakerUkiki,     0,           MODEL_MAKER_UKIKI,         OBJ_TYPE_HAS_DIALOG,     OBJ_OCCUPY_INNER, 0, 0, 1.0f, ukiki_seg5_anims_05015784, df_ukiki, SOUND_OBJ_UKIKI_CHATTER_SHORT},
/* OBJECT_TYPE_MOLEMAN */       {"Moleman",                 mat_b_btn_moleman,      bhvMoleman,        65,          MODEL_MAKER_MOLEMAN,       OBJ_TYPE_HAS_DIALOG,     OBJ_OCCUPY_INNER, 0, 0, 1.0f, moleman3_anims, NULL, SOUND_ACTION_READ_SIGN},
/* OBJECT_TYPE_COBIE */         {"Cobie",                   mat_b_btn_cobie,        bhvCobie,          0,           MODEL_MAKER_COBIE,         OBJ_TYPE_HAS_DIALOG,     OBJ_OCCUPY_INNER, 0, 0, 1.0f, cobie2_anims, NULL, SOUND_ACTION_READ_SIGN},
/* OBJECT_TYPE_CONVEYOR */      {"Conveyor",                mat_b_btn_conveyor,     bhvConveyor,       0,           MODEL_MAKER_CONVEYOR_HALF, 0,                       OBJ_OCCUPY_FULL,  0, 0, 1.0f, NULL, df_conveyor, SOUND_OBJ_HEAVEHO_PREVIEW},
/* OBJECT_TYPE_TIMED_BLOCK */   {"Inverted",                mat_b_btn_tblock,       bhvTimedBlock,     0,           MODEL_MAKER_BLOCK_OFF,     0,                       OBJ_OCCUPY_OUTER, 0, 0, 1.0f, NULL, df_timedblock, SOUND_GENERAL2_SWITCH_TICK_FAST},
/* OBJECT_TYPE_TRIGGER */       {"Star Trigger",            mat_b_btn_trigger,      bhvStaticObject,   TILE_SIZE/2, MODEL_MAKER_IMBUE_TRIGGER, OBJ_TYPE_BILLBOARD,      OBJ_OCCUPY_INNER, 0, 0, 1.0f, NULL, NULL, SOUND_MENU_COLLECT_SECRET},
/* OBJECT_TYPE_TRIGGER_STAR */  {"Star Triggers",           mat_b_btn_triggerstar,  bhvHiddenStar,     TILE_SIZE/2, MODEL_TRANSPARENT_STAR,    OBJ_TYPE_STAR,           OBJ_OCCUPY_INNER, 0, 0, 1.0f, NULL, df_star, SOUND_MENU_STAR_SOUND | SOUND_VIBRATO},
};

//behparam2 strings
char *txt_bp_box[] = {
    "Rocket Boots",
    "Vanetal Cap",
    "Koopa Shell",
    "Green Coin",
    "One Coin",
    "Three Coins",
    "Ten Coins",
};

char *txt_bp_vbox[] = {
    "Wing Cap",
    "Metal Cap",
    "Vanish Cap",
    "Koopa Shell",
    "One Coin",
    "Three Coins",
    "Ten Coins",
};

char *txt_bp_tree[] = {
    "Bubbly",
    "Palm",
    "Spiky",
    "Snowy",
    "Farm",
    "Dead",
};

char *txt_badges[] = {
    "Lava Boost",
    "Fall Damage",
    "Defense",
    "One Hit",
    "Gills",
    "Fins",
    "HP Regen",
    "Mana Regen",
    "Greed",
    "Double Time",
    "Magnet",
    "Burn",
    "Squish",
    "Feather",
    "Weight",
    "Sticky",
    "Fast Foot",
    "Heal Plus",
    "Bottomless",
    "Slow Fall",
    "Brittle Burden",
    "Withering Burden",
};

char *txt_coin_formation[] = {
    "Line",
    "Vertical Line",
    "Ring",
    "Vertical Ring",
    "Arrow",
};

char *txt_onoff[] = {
    "Red",
    "Blue"
};

char *txt_woodplat[] = {
    "Thin",
    "Full"
};

char *txt_fire_spinner[] = {
    "Length: 2",
    "Length: 3",
    "Length: 4",
    "Length: 5",
    "Length: 6",
};

char *txt_conveyor[] = {
    "Thin",
    "Full",
    "Upwards",
    "Downwards",
    "Thin (On-Off)",
    "Full (On-Off)",
    "Upwards (On-Off)",
    "Downwards (On-Off)",
};

u8 mb_star_idlist[] = {OBJECT_TYPE_STAR, OBJECT_TYPE_RED_COIN_STAR, OBJECT_TYPE_TRIGGER_STAR};
u8 mb_goomba_idlist[] = {OBJECT_TYPE_GOOMBA, OBJECT_TYPE_BIG_GOOMBA, OBJECT_TYPE_TINY_GOOMBA};
u8 mb_piranha_idlist[] = {OBJECT_TYPE_PIRANHA_PLANT, OBJECT_TYPE_BIG_PIRANHA_PLANT, OBJECT_TYPE_TINY_PIRANHA_PLANT};
u8 mb_bluecoin_idlist[] = {OBJECT_TYPE_BLUE_COIN, OBJECT_TYPE_BLUE_COIN_SWITCH};
u8 mb_hammer_idlist[] = {OBJECT_TYPE_HAMMER_BRO, OBJECT_TYPE_FIRE_BRO};
u8 mb_whomp_idlist[] = {OBJECT_TYPE_WHOMP, OBJECT_TYPE_KING_WHOMP};
u8 mb_bully_idlist[] = {OBJECT_TYPE_BULLY, OBJECT_TYPE_CHILL_BULLY, OBJECT_TYPE_BIG_BULLY, OBJECT_TYPE_BIG_CHILL_BULLY};
u8 mb_boo_idlist[] = {OBJECT_TYPE_BOO, OBJECT_TYPE_BIG_BOO};
u8 mb_plat_idlist[] = {OBJECT_TYPE_PLATFORM_TRACK, OBJECT_TYPE_PLATFORM_LOOPING};
u8 mb_thwomp_idlist[] = {OBJECT_TYPE_THWOMP, OBJECT_TYPE_GRINDEL};
u8 mb_flame_idlist[] = {OBJECT_TYPE_RED_FLAME, OBJECT_TYPE_BLUE_FLAME};
u8 mb_npc_idlist[] = {OBJECT_TYPE_SIGN, OBJECT_TYPE_BUDDY, OBJECT_TYPE_TOAD, OBJECT_TYPE_TUXIE, OBJECT_TYPE_UKIKI};
u8 mb_npccm_idlist[] = {OBJECT_TYPE_SIGN, OBJECT_TYPE_BUDDY, OBJECT_TYPE_MOLEMAN, OBJECT_TYPE_COBIE};
u8 mb_power_idlist[] = {OBJECT_TYPE_CROWBAR, OBJECT_TYPE_MASK};
u8 mb_timedbox_idlist[] = {OBJECT_TYPE_TIMED_BOX, OBJECT_TYPE_TIMEDBLOCK};
u8 mb_smallbox_idlist[] = {OBJECT_TYPE_BBOX_SMALL, OBJECT_TYPE_BBOX_CRAZY};

struct MBUIButton mb_ui_buttons[] = {
/* MB_BUTTON_SETTINGS */ {MB_PM_ACTION,  FALSE, 0, OBJECT_TYPE_SETTINGS,    NULL},
/* MB_BUTTON_TEST */     {MB_PM_ACTION,  FALSE, 0, OBJECT_TYPE_TEST_MARIO,  NULL},
/* MB_BUTTON_TERRAIN */  {MB_PM_TILE, FALSE, 0, TILE_TYPE_BLOCK,         NULL},
/* MB_BUTTON_SLOPE */    {MB_PM_TILE, FALSE, 0, TILE_TYPE_SLOPE,         NULL},
/* MB_BUTTON_TROLL */    {MB_PM_TILE, FALSE, 0, TILE_TYPE_TROLL,         NULL},
/* MB_BUTTON_STAR */     {MB_PM_OBJ,  TRUE,  3, mb_star_idlist,        "Power Star"},
/* MB_BUTTON_GOOMBA */   {MB_PM_OBJ,  TRUE,  3, mb_goomba_idlist,      "Goomba"},
/* MB_BUTTON_PIRANHA */  {MB_PM_OBJ,  TRUE,  3, mb_piranha_idlist,     "Piranha Plant"},
/* MB_BUTTON_KOOPA */    {MB_PM_OBJ,  FALSE, 0, OBJECT_TYPE_KOOPA,       NULL},
/* MB_BUTTON_COIN */     {MB_PM_OBJ,  FALSE, 0, OBJECT_TYPE_COIN,        NULL},
/* MB_BUTTON_BLANK */    {MB_PM_TILE, FALSE, 0, TILE_TYPE_BLOCK,         NULL},
/* MB_BUTTON_GCOIN */    {MB_PM_OBJ,  FALSE, 0, OBJECT_TYPE_GREEN_COIN,  NULL},
/* MB_BUTTON_CORNER */   {MB_PM_TILE, FALSE, 0, TILE_TYPE_CORNER,        NULL},
/* MB_BUTTON_ICORNER */  {MB_PM_TILE, FALSE, 0, TILE_TYPE_ICORNER,       NULL},
/* MB_BUTTON_RCOIN */    {MB_PM_OBJ,  FALSE, 0, OBJECT_TYPE_RED_COIN,    NULL},
/* MB_BUTTON_BCOIN */    {MB_PM_OBJ,  TRUE,  2, mb_bluecoin_idlist,    "Blue Coins"},
/* MB_BUTTON_NOTEBLOCK */{MB_PM_OBJ,  FALSE, 0, OBJECT_TYPE_NOTEBLOCK,   NULL},
/* MB_BUTTON_CULL */     {MB_PM_TILE, FALSE, 0, TILE_TYPE_CULL,          NULL},
/* MB_BUTTON_BOBOMB */   {MB_PM_OBJ,  FALSE, 0, OBJECT_TYPE_BOBOMB,      NULL},
/* MB_BUTTON_CHUCKYA */  {MB_PM_OBJ,  FALSE, 0, OBJECT_TYPE_CHUCKYA,     NULL},
/* MB_BUTTON_BULLY */    {MB_PM_OBJ,  TRUE,  4, mb_bully_idlist,       "Bully"},
/* MB_BUTTON_BULLET */   {MB_PM_OBJ,  FALSE, 0, OBJECT_TYPE_BULLET_BILL, NULL},
/* MB_BUTTON_HEAVEHO */  {MB_PM_OBJ,  FALSE, 0, OBJECT_TYPE_HEAVE_HO,    NULL},
/* MB_BUTTON_MOTOS */    {MB_PM_OBJ,  FALSE, 0, OBJECT_TYPE_MOTOS,       NULL},
/* MB_BUTTON_TREE */     {MB_PM_OBJ,  FALSE, 6, OBJECT_TYPE_TREE,        txt_bp_tree},
/* MB_BUTTON_EXCLA */    {MB_PM_OBJ,  FALSE, 7, OBJECT_TYPE_EXCL_BOX,    txt_bp_box},
/* MB_BUTTON_SPAWN */    {MB_PM_OBJ,  FALSE, 0, OBJECT_TYPE_MARIO_SPAWN, NULL},
/* MB_BUTTON_REX */      {MB_PM_OBJ,  FALSE, 0, OBJECT_TYPE_REX,         NULL},
/* MB_BUTTON_PODOBOO */  {MB_PM_OBJ,  FALSE, 0, OBJECT_TYPE_PODOBOO,     NULL},
/* MB_BUTTON_CRABLET */  {MB_PM_OBJ,  FALSE, 0, OBJECT_TYPE_CRABLET,     NULL},
/* MB_BUTTON_HAMMER_BRO */{MB_PM_OBJ, TRUE,  2, mb_hammer_idlist,      "Hammer Bro"},
/* MB_BUTTON_CHICKEN */  {MB_PM_OBJ,  FALSE, 0, OBJECT_TYPE_CHICKEN,     NULL},
/* MB_BUTTON_PHANTASM */ {MB_PM_OBJ,  FALSE, 0, OBJECT_TYPE_PHANTASM,    NULL},
/* MB_BUTTON_PIPE */     {MB_PM_OBJ,  FALSE, 0, OBJECT_TYPE_WARP_PIPE,   NULL},
/* MB_BUTTON_BADGE */    {MB_PM_OBJ,  FALSE, 22, OBJECT_TYPE_BADGE,      txt_badges},
/* MB_BUTTON_WATER */    {MB_PM_TILE,FALSE, 0, TILE_TYPE_WATER,         NULL},
/* MB_BUTTON_FENCE */    {MB_PM_TILE, FALSE, 0, TILE_TYPE_FENCE,         NULL},
/* MB_BUTTON_KING_BOBOMB */ {MB_PM_OBJ, FALSE, 0, OBJECT_TYPE_KING_BOBOMB, NULL},
/* MB_BUTTON_WIGGLER */  {MB_PM_OBJ,  FALSE, 0, OBJECT_TYPE_WIGGLER,     NULL},
/* MB_BUTTON_BOWSER */   {MB_PM_OBJ,  FALSE, 0, OBJECT_TYPE_BOWSER,      NULL},
/* MB_BUTTON_MPLAT */    {MB_PM_OBJ,  TRUE,  2, mb_plat_idlist,        "Moving Platform"},
/* MB_BUTTON_BBALL */    {MB_PM_OBJ,  FALSE, 0, OBJECT_TYPE_BOWLING_BALL, NULL},
/* MB_BUTTON_KTQ */      {MB_PM_OBJ,  FALSE, 0, OBJECT_TYPE_KOOPA_THE_QUICK, NULL},
/* MB_BUTTON_SSLOPE */   {MB_PM_TILE, FALSE, 0, TILE_TYPE_SSLOPE,        NULL},
/* MB_BUTTON_SLAB */     {MB_PM_TILE, FALSE, 0, TILE_TYPE_SLAB,          NULL},
/* MB_BUTTON_PURPLE_SWITCH */{MB_PM_OBJ, FALSE, 0, OBJECT_TYPE_PURPLE_SWITCH, NULL},
/* MB_BUTTON_TIMED_BOX */ {MB_PM_OBJ,  TRUE, 2, mb_timedbox_idlist,   "Timed Box"},
/* MB_BUTTON_HEART */    {MB_PM_OBJ,  FALSE, 0, OBJECT_TYPE_RECOVERY_HEART, NULL},
/* MB_BUTTON_FORMATION */{MB_PM_OBJ,  FALSE, 5, OBJECT_TYPE_COIN_FORMATION, txt_coin_formation},
/* MB_BUTTON_VSLAB */    {MB_PM_TILE, FALSE, 0, TILE_TYPE_SSLAB,         NULL},
/* MB_BUTTON_SCORNER */  {MB_PM_TILE, FALSE, 0, TILE_TYPE_SCORNER,       NULL},
/* MB_BUTTON_UGENTLE */  {MB_PM_TILE, FALSE, 0, TILE_TYPE_UGENTLE,       NULL},
/* MB_BUTTON_LGENTLE */  {MB_PM_TILE, FALSE, 0, TILE_TYPE_LGENTLE,       NULL},
/* MB_BUTTON_BARS */     {MB_PM_TILE, FALSE, 0, TILE_TYPE_BARS,          NULL},
/* MB_BUTTON_THWOMP */   {MB_PM_OBJ,  TRUE,  2, mb_thwomp_idlist,      "Thwomp"},
/* MB_BUTTON_WHOMP */    {MB_PM_OBJ,  TRUE,  2, mb_whomp_idlist,       "Whomp"},
/* MB_BUTTON_POLE */     {MB_PM_TILE, FALSE, 0, TILE_TYPE_POLE,          NULL},
/* MB_BUTTON_VEXCLA */   {MB_PM_OBJ,  FALSE, 7, OBJECT_TYPE_EXCL_BOX,    txt_bp_vbox},
/* MB_BUTTON_LAKITU */   {MB_PM_OBJ,  FALSE, 0, OBJECT_TYPE_LAKITU,      NULL},
/* MB_BUTTON_FLYGUY */   {MB_PM_OBJ,  FALSE, 0, OBJECT_TYPE_FLY_GUY,     NULL},
/* MB_BUTTON_SNUFIT */   {MB_PM_OBJ,  FALSE, 0, OBJECT_TYPE_SNUFIT,      NULL},
/* MB_BUTTON_AMP */      {MB_PM_OBJ,  FALSE, 0, OBJECT_TYPE_AMP,         NULL},
/* MB_BUTTON_BOO */      {MB_PM_OBJ,  TRUE,  2, mb_boo_idlist,         "Boo"},
/* MB_BUTTON_MR_I */     {MB_PM_OBJ,  FALSE, 0, OBJECT_TYPE_MR_I,        NULL},
/* MB_BUTTON_SCUTTLEBUG */{MB_PM_OBJ, FALSE, 0, OBJECT_TYPE_SCUTTLEBUG,  NULL},
/* MB_BUTTON_SPINDRIFT */{MB_PM_OBJ,  FALSE, 0, OBJECT_TYPE_SPINDRIFT,   NULL},
/* MB_BUTTON_BLIZZARD */ {MB_PM_OBJ,  FALSE, 0, OBJECT_TYPE_MR_BLIZZARD, NULL},
/* MB_BUTTON_MONEYBAG */ {MB_PM_OBJ,  FALSE, 0, OBJECT_TYPE_MONEYBAG,    NULL},
/* MB_BUTTON_SKEETER */  {MB_PM_OBJ,  FALSE, 0, OBJECT_TYPE_SKEETER,     NULL},
/* MB_BUTTON_POKEY */    {MB_PM_OBJ,  FALSE, 0, OBJECT_TYPE_POKEY,       NULL},
/* MB_BUTTON_MINE */     {MB_PM_OBJ,  FALSE, 0, OBJECT_TYPE_BOWSER_BOMB, NULL},
/* MB_BUTTON_FIRE */     {MB_PM_OBJ,  TRUE,  2, mb_flame_idlist,       "Flame"},
/* MB_BUTTON_FLAMETHROWER */{MB_PM_OBJ, FALSE, 0, OBJECT_TYPE_FLAMETHROWER, NULL},
/* MB_BUTTON_FIRE_SPITTER */{MB_PM_OBJ, FALSE, 0, OBJECT_TYPE_FIRE_SPITTER, NULL},
/* MB_BUTTON_FIRE_SPINNER */{MB_PM_OBJ, FALSE, 5, OBJECT_TYPE_FIRE_SPINNER, txt_fire_spinner},
/* MB_BUTTON_BREAKABLE */{MB_PM_OBJ, FALSE, 0, OBJECT_TYPE_BBOX_NORMAL, NULL},
/* MB_BUTTON_SMALL_BOX */{MB_PM_OBJ, TRUE, 2, mb_smallbox_idlist, "Boxes"},
/* MB_BUTTON_DIAMOND */  {MB_PM_OBJ, FALSE, 0, OBJECT_TYPE_DIAMOND, NULL},
/* MB_BUTTON_NPC */      {MB_PM_OBJ, TRUE, 5, mb_npc_idlist, "NPC"},
/* MB_BUTTON_NPCCM */    {MB_PM_OBJ, TRUE, 4, mb_npccm_idlist, "NPC"},
/* MB_BUTTON_BUTTON */   {MB_PM_OBJ, FALSE, 2, OBJECT_TYPE_BUTTON, txt_onoff},
/* MB_BUTTON_BLOCK */    {MB_PM_OBJ, FALSE, 2, OBJECT_TYPE_ON_OFF_BLOCK, txt_onoff},
/* MB_BUTTON_WOODPLAT */ {MB_PM_OBJ, FALSE, 2, OBJECT_TYPE_WOODPLAT, txt_woodplat},
/* MB_BUTTON_RFBOX */    {MB_PM_OBJ, FALSE, 0, OBJECT_TYPE_RFBOX, NULL},
/* MB_BUTTON_SHOWRUN */  {MB_PM_OBJ, FALSE, 0, OBJECT_TYPE_SHOWRUNNER, NULL},
/* MB_BUTTON_POWER */    {MB_PM_OBJ, TRUE, 2, mb_power_idlist, "Powerup"},
/* MB_BUTTON_CONVEYOR */ {MB_PM_OBJ, FALSE, 8, OBJECT_TYPE_CONVEYOR, txt_conveyor},
/* MB_BUTTON_ISCORNER */ {MB_PM_TILE, FALSE, 0, TILE_TYPE_ISCORNER, NULL},
/* MB_BUTTON_TRIGGER */  {MB_PM_OBJ, FALSE, 0, OBJECT_TYPE_TRIGGER, NULL},
};

u8 mb_toolbar_defaults[9] = {
    MB_BUTTON_TERRAIN,
    MB_BUTTON_SLOPE,
    MB_BUTTON_BARS,
    MB_BUTTON_TREE,
    MB_BUTTON_COIN,
    MB_BUTTON_STAR,
    MB_BUTTON_GOOMBA,
    MB_BUTTON_TEST,
    MB_BUTTON_SETTINGS,
};

//Different toolboxes for different game styles
#define _ MB_BUTTON_BLANK
u8 mb_toolbox_btcm[18 * 5] = {
    MB_BUTTON_TERRAIN, MB_BUTTON_SLAB, MB_BUTTON_SLOPE, MB_BUTTON_CORNER, MB_BUTTON_ICORNER, MB_BUTTON_VSLAB, MB_BUTTON_SSLOPE, MB_BUTTON_SCORNER, MB_BUTTON_ISCORNER,
    MB_BUTTON_GOOMBA, MB_BUTTON_BOBOMB, MB_BUTTON_CHUCKYA, MB_BUTTON_KOOPA, MB_BUTTON_LAKITU, MB_BUTTON_FLYGUY, MB_BUTTON_SNUFIT, MB_BUTTON_THWOMP, MB_BUTTON_WHOMP,

    MB_BUTTON_LGENTLE, MB_BUTTON_UGENTLE, MB_BUTTON_TROLL, MB_BUTTON_WATER, MB_BUTTON_FENCE, MB_BUTTON_BARS, MB_BUTTON_POLE, MB_BUTTON_TREE, MB_BUTTON_CULL,
    MB_BUTTON_BULLY, MB_BUTTON_MOTOS, MB_BUTTON_HEAVEHO, MB_BUTTON_BULLET, MB_BUTTON_AMP, MB_BUTTON_PIRANHA, MB_BUTTON_BOO,MB_BUTTON_MR_I,MB_BUTTON_SCUTTLEBUG,

    MB_BUTTON_STAR, MB_BUTTON_COIN, MB_BUTTON_FORMATION, MB_BUTTON_GCOIN, MB_BUTTON_RCOIN, MB_BUTTON_BCOIN, MB_BUTTON_EXCLA, MB_BUTTON_TRIGGER, MB_BUTTON_HEART,
    MB_BUTTON_SPINDRIFT,MB_BUTTON_BLIZZARD,MB_BUTTON_MONEYBAG,MB_BUTTON_SKEETER, MB_BUTTON_POKEY, MB_BUTTON_REX, MB_BUTTON_HAMMER_BRO, MB_BUTTON_PODOBOO, MB_BUTTON_PHANTASM,

    MB_BUTTON_MPLAT, MB_BUTTON_PURPLE_SWITCH, MB_BUTTON_TIMED_BOX, MB_BUTTON_BREAKABLE, MB_BUTTON_RFBOX, MB_BUTTON_DIAMOND, MB_BUTTON_WOODPLAT, MB_BUTTON_NPCCM, MB_BUTTON_BADGE,
    MB_BUTTON_FIRE, MB_BUTTON_FLAMETHROWER, MB_BUTTON_FIRE_SPITTER, MB_BUTTON_FIRE_SPINNER, MB_BUTTON_BBALL, _, MB_BUTTON_CHICKEN, MB_BUTTON_CRABLET, MB_BUTTON_SHOWRUN,

    MB_BUTTON_NOTEBLOCK, MB_BUTTON_BUTTON, MB_BUTTON_BLOCK, MB_BUTTON_CONVEYOR, _, _, _, _, _,
    MB_BUTTON_SPAWN, MB_BUTTON_SMALL_BOX, MB_BUTTON_POWER, _, _, _, _, _, _,
};

u8 mb_toolbox_vanilla[18 * 5] = {
    MB_BUTTON_TERRAIN, MB_BUTTON_SLAB, MB_BUTTON_SLOPE, MB_BUTTON_CORNER, MB_BUTTON_ICORNER, MB_BUTTON_VSLAB, MB_BUTTON_SSLOPE, MB_BUTTON_SCORNER, MB_BUTTON_ISCORNER,
    MB_BUTTON_GOOMBA, MB_BUTTON_BOBOMB, MB_BUTTON_CHUCKYA, MB_BUTTON_KOOPA, MB_BUTTON_LAKITU, MB_BUTTON_FLYGUY, MB_BUTTON_SNUFIT, MB_BUTTON_THWOMP, MB_BUTTON_WHOMP,

    MB_BUTTON_LGENTLE, MB_BUTTON_UGENTLE, MB_BUTTON_TROLL, MB_BUTTON_WATER, MB_BUTTON_FENCE, MB_BUTTON_BARS, MB_BUTTON_POLE, MB_BUTTON_TREE, MB_BUTTON_CULL,
    MB_BUTTON_BULLY, MB_BUTTON_MOTOS, MB_BUTTON_HEAVEHO, MB_BUTTON_BULLET, MB_BUTTON_AMP, MB_BUTTON_PIRANHA, MB_BUTTON_BOO,MB_BUTTON_MR_I,MB_BUTTON_SCUTTLEBUG,

    MB_BUTTON_STAR, MB_BUTTON_COIN, MB_BUTTON_FORMATION, MB_BUTTON_RCOIN, MB_BUTTON_BCOIN, MB_BUTTON_VEXCLA, MB_BUTTON_TRIGGER, MB_BUTTON_HEART, _,
    MB_BUTTON_SPINDRIFT,MB_BUTTON_BLIZZARD,MB_BUTTON_MONEYBAG,MB_BUTTON_SKEETER, MB_BUTTON_POKEY, MB_BUTTON_KING_BOBOMB, MB_BUTTON_WIGGLER, MB_BUTTON_BOWSER, _,

    MB_BUTTON_MPLAT, MB_BUTTON_PURPLE_SWITCH, MB_BUTTON_TIMED_BOX, MB_BUTTON_BREAKABLE, MB_BUTTON_RFBOX, MB_BUTTON_DIAMOND, MB_BUTTON_WOODPLAT, MB_BUTTON_NPC, MB_BUTTON_KTQ,
    MB_BUTTON_FIRE, MB_BUTTON_FLAMETHROWER, MB_BUTTON_FIRE_SPITTER, MB_BUTTON_FIRE_SPINNER, MB_BUTTON_BBALL, _, _, _, _,

    MB_BUTTON_NOTEBLOCK, MB_BUTTON_BUTTON, MB_BUTTON_BLOCK, MB_BUTTON_CONVEYOR, _, _, _, _, _,
    MB_BUTTON_SPAWN, MB_BUTTON_SMALL_BOX, MB_BUTTON_MINE, _, _, _, _, _, _,
};

u8 seq_musicmenu_array[] = {
    SEQ_LEVEL_GRASS,
    SEQ_LEVEL_SLIDE,
    SEQ_LEVEL_WATER,
    SEQ_LEVEL_WATER,
    SEQ_LEVEL_HOT,
    SEQ_LEVEL_SNOW,
    SEQ_LEVEL_SPOOKY,
    SEQ_LEVEL_UNDERGROUND,
    SEQ_LEVEL_UNDERGROUND,
    SEQ_LEVEL_KOOPA_ROAD_2,
    SEQ_VANILLA_BOSS,
    SEQ_LEVEL_BOSS_KOOPA,
    SEQ_LEVEL_BOSS_KOOPA_FINAL,
    SEQ_LEVEL_INSIDE_CASTLE2,

    SEQ_LEVEL_INSIDE_CASTLE,
    SEQ_REDHOT,
    SEQ_FARM,
    SEQ_JUNGLE,
    SEQ_PIRATE,
    SEQ_EVENT_CUTSCENE_ENDING,
    SEQ_BIG_HOUSE,
    SEQ_NSMB_CASTLE,
    SEQ_EVENT_BOSS,
    SEQ_LEVEL_KOOPA_ROAD,
    SEQ_COSMIC_SEED_BOSS,
    SEQ_SHOWRUNNER_BOSS,
    SEQ_COSMIC_SEED_LEVEL,
    SEQ_FINAL_BOSS,

    SEQ_SMS_BIANCO_HILLS,
    SEQ_SMS_SKY_AND_SEA,
    SEQ_SMS_SECRET_COURSE,
    SEQ_SMG_COMET_OBSERVATORY,
    SEQ_SMG_BUOY_BASE,
    SEQ_SMG_BATTLEROCK,
    SEQ_SMG_GHOSTLY_GALAXY,
    SEQ_SMG_PURPLE_COMET,
    SEQ_SMG2_HONEYBLOOM,
    SEQ_PIRANHA_CREEK,
    SEQ_NSMB_DESERT,

    SEQ_KOOPA_BEACH, // mk64 koopa troopa beach
    SEQ_FRAPPE_SNOWLAND,
    SEQ_MK64_BOWSERS_CASTLE,
    SEQ_MK64_RAINBOW_ROAD,
    SEQ_MKDS_WALUIGI_PINBALL,
    SEQ_MK8_RAINBOW_ROAD,

    SEQ_SMRPG_MARIOS_PAD,             // 0x46
    SEQ_SMRPG_NIMBUS_LAND,
    SEQ_FOREST_MAZE,
    SEQ_SMRPG_SUNKEN_SHIP,            // 0x47

    SEQ_PM_DRY_DESERT,
    SEQ_PM_FOREVER_FOREST,
    SEQ_TTYD_PETAL_MEADOWS,
    SEQ_TTYD_EIGHT_KEY_DOMAIN, // riddle tower
    SEQ_TTYD_ROGUEPORT_SEWERS,
    SEQ_TTYD_XNAUT_FORTRESS,
    SEQ_SPM_FLIPSIDE,
    SEQ_SPM_LINELAND_ROAD,
    SEQ_SAMMER_KINGDOM,
    SEQ_SPM_FLORO_CAVERNS,
    SEQ_SPM_OVERTHERE_STAIR,

    SEQ_MP_YOSHIS_TROPICAL_ISLAND,
    SEQ_MP_RAINBOW_CASTLE,
    SEQ_MLPIT_BEHIND_YOSHI_VILLAGE,
    SEQ_PIT_GRITZY_DESERT,
    SEQ_BIS_BUMPSY_PLAINS,
    SEQ_BIS_DEEP_CASTLE,
    SEQ_YI_OVERWORLD,
    SEQ_YI_CRYSTAL_CAVES,
    SEQ_YS_TITLE,                     // 0x51

    SEQ_OOT_KOKIRI_FOREST,
    SEQ_OOT_LOST_WOODS,
    SEQ_OOT_GERUDO_VALLEY,            // 0x52
    SEQ_STONE_TOWER_TEMPLE,
    SEQ_WW_OUTSET_ISLAND,
    SEQ_TP_LAKE_HYLIA,                // 0x53
    SEQ_TP_GERUDO_DESERT,
    SEQ_SS_SKYLOFT,

    SEQ_DK64_FRANTIC_FACTORY,
    SEQ_DK64_HIDEOUT_HELM,
    SEQ_DK_CREEPY_CASTLE,
    SEQ_DK64_GLOOMY_GALLEON,
    SEQ_DK64_FUNGI_FOREST,
    SEQ_DK64_CRYSTAL_CAVES,
    SEQ_DK64_ANGRY_AZTEC,
    SEQ_DKC2_SNOWBOUND_LAND,

    SEQ_BK_BUBBLEGLOOP_SWAMP,
    SEQ_BK_FREEZEEZY_PEAKS,
    SEQ_BK_GOBI_VALLEY,

    SEQ_K64_FACTORY_INSPECTION,
    SEQ_BM_GREEN_GARDEN,
    SEQ_BM_BLACK_FORTRESS,
    SEQ_SA_WINDY_HILL,
    SEQ_PKMN_SKY_TOWER,
    SEQ_TOUHOU_YOUKAI_MOUNTAIN,
    SEQ_FOREST_TEMPLE,
    SEQ_RAYMAN_BAND_LAND,

    SEQ_SMB1_OVERWORLD,
    SEQ_SMB_BOWSER_REMIX,             // 0x4B
    SEQ_SMB2_OVERWORLD,
    SEQ_SMB3_OVERWORLD,
    SEQ_SMB3_CASTLE,
    SEQ_SMW_ATHLETIC,                 // 0x4E
    SEQ_SMW_CASTLE,
};

//envfx table
u8 mb_envfx_table[] = {
    ENVFX_NONE,         // no effects
    ENVFX_ASHES,       // CCM, SL
    ENVFX_SNOW,     // unused
    ENVFX_RAIN,
    ENVFX_SANDSTORM,
};

u8 mb_boundary_table[] = {
    0, // Void
    MB_BOUNDARY_INNER_FLOOR | MB_BOUNDARY_OUTER_FLOOR, // Plain
    MB_BOUNDARY_INNER_FLOOR | MB_BOUNDARY_OUTER_FLOOR | MB_BOUNDARY_INNER_WALLS, // Valley
    MB_BOUNDARY_OUTER_FLOOR | MB_BOUNDARY_INNER_WALLS, // Chasm
    MB_BOUNDARY_INNER_FLOOR | MB_BOUNDARY_OUTER_WALLS, // Plateau
    MB_BOUNDARY_INNER_FLOOR | MB_BOUNDARY_INNER_WALLS | MB_BOUNDARY_CEILING, // Interior
};

#define AT_CEILING(y) ((mb_curr_boundary & MB_BOUNDARY_CEILING) && ((y) == mb_lopt_boundary_height-1))

//skybox table
u8 *mb_skybox_table[] = {
    _water_skybox_yay0SegmentRomStart,
    _water_skybox_yay0SegmentRomEnd,
    _cloud_floor_skybox_yay0SegmentRomStart,
    _cloud_floor_skybox_yay0SegmentRomEnd,
    _bitfs_skybox_yay0SegmentRomStart,
    _bitfs_skybox_yay0SegmentRomEnd,
    _bidw_skybox_yay0SegmentRomStart,
    _bidw_skybox_yay0SegmentRomEnd,
    _bbh_skybox_yay0SegmentRomStart,
    _bbh_skybox_yay0SegmentRomEnd,
    _ccm_skybox_yay0SegmentRomStart,
    _ccm_skybox_yay0SegmentRomEnd,
    _ssl_skybox_yay0SegmentRomStart,
    _ssl_skybox_yay0SegmentRomEnd,
    _wdw_skybox_yay0SegmentRomStart,
    _wdw_skybox_yay0SegmentRomEnd,
    _bits_skybox_yay0SegmentRomStart,
    _bits_skybox_yay0SegmentRomEnd,
    NULL,
    NULL,
};

struct MBTemplate mb_templates[] = {
    /* Grass */  {{0, 16}, 0, 0, MB_THEME_GENERIC,   0, 0,  1, 0,    2,  FALSE, 0},   // BoB, Floating Farm
    /* Sky */    {{1, 46},   0, 1, MB_THEME_GENERIC,   0, 0,  0, 0,    10, TRUE,  3},   // Slider, Nimbus Land
    /* Desert */ {{4, 63}, 4, 6, MB_THEME_SSL,       0, 0,  1, 0,    2,  FALSE, 0},   // LLL, Gritzy Desert
    /* Snow */   {{5, 39}, 2, 5, MB_THEME_SNOW,      0, 0,  1, 0,    2,  FALSE, 0},   // CCM, Frappe Snowland
    /* Lava */   {{4, 15}, 1, 2, MB_THEME_RHR,       8, 0,  1, 0,    3,  TRUE,  0},   // LLL, Red-Hot Reservoir
    /* Water */  {{2, 29}, 0, 0, MB_THEME_JRB,       8, 10, 2, 8,    10, TRUE,  4},   // DDD, Sky and Sea
    /* Cave */   {{7, 67},   0, 3, MB_THEME_HMC,       0, 32, 5, 0,    2,  FALSE, 0},   // HMC, Underground (YI)
    /* Spooky */ {{6, 34},   3, 4, MB_THEME_BBH,       3, 6,  2, 0,    2,  FALSE, 0},   // BBH, Ghostly Galaxy
    /* Castle */ {{13,14},   0, 9, MB_THEME_CASTLE,    0, 16, 5, 0,    2,  FALSE, 0},   // Castle, Cosmic Castle
    /* Retro */  {{96,96},   0, 1, MB_THEME_RETRO,     0, 0,  4, 0,    2,  FALSE, 0},   // RETRO!!!!!!!!!
};

u8 mb_text_colors[][3] = {
    {255, 255, 255},
    {255, 255, 0},
    {150, 150, 150},
    {150, 150, 0},
    {255, 0, 0},
    {0, 150, 255},
};

#define IMBUE_YELLOW 0xFFFF00FF
#define IMBUE_GREEN 0x00DD00FF
#define IMBUE_BLUE 0x0000FFFF
#define IMBUE_RED 0xFF0000FF
#define IMBUE_ORANGE 0xFFA500FF
#define IMBUE_PINK 0xe23a44FF

struct MBImbueData imbue_table[] = {
    /* IMBUE_NONE */        {0, 0, 0},
    /* IMBUE_STAR */        {0, MODEL_MAKER_IMBUE_STAR, IMBUE_YELLOW},
    /* IMBUE_THREE_COINS */ {3, MODEL_MAKER_IMBUE_COIN, IMBUE_YELLOW},
    /* IMBUE_ONE_COIN */    {1, MODEL_MAKER_IMBUE_COIN, IMBUE_YELLOW},
    /* IMBUE_GREEN_COIN */  {3, MODEL_MAKER_IMBUE_COIN, IMBUE_GREEN},
    /* IMBUE_BLUE_COIN */   {5, MODEL_MAKER_IMBUE_COIN, IMBUE_BLUE},
    /* IMBUE_RED_SWITCH */  {0, MODEL_MAKER_IMBUE, IMBUE_RED},
    /* IMBUE_BLUE_SWITCH */ {0, MODEL_MAKER_IMBUE, IMBUE_BLUE},
    /* IMBUE_RED_COIN */    {2, MODEL_MAKER_IMBUE_COIN, IMBUE_RED},
    /* IMBUE_TRIGGER */     {0, MODEL_MAKER_IMBUE_TRIGGER, IMBUE_ORANGE},
    /* IMBUE_CROWBAR */     {0, MODEL_MAKER_IMBUE, IMBUE_GREEN},
    /* IMBUE_BULLET_MASK*/  {0, MODEL_MAKER_IMBUE, IMBUE_PINK},
    /* IMBUE_BADGE_BASE */  {0, MODEL_MAKER_IMBUE_BADGE, 0},
};
