#pragma once

#ifndef MB_DATA_H
#define MB_DATA_H

#include "mb_types.h"

enum MBTileTypes {
    TILE_TYPE_EMPTY,
    // Flippable tiles
    TILE_TYPE_SLOPE = 2,
    TILE_TYPE_DSLOPE,
    TILE_TYPE_SLAB,
    TILE_TYPE_DSLAB,
    TILE_TYPE_CORNER,
    TILE_TYPE_DCORNER,
    TILE_TYPE_ICORNER, // Inner Corner
    TILE_TYPE_DICORNER, // Down Inner Corner
    TILE_TYPE_SCORNER, // Sloped Corner
    TILE_TYPE_DSCORNER, // Down Sloped Corner
    TILE_TYPE_ISCORNER, // Inverted Sloped Corner
    TILE_TYPE_DISCORNER, // Down Inverted Sloped Corner
    TILE_TYPE_UGENTLE,
    TILE_TYPE_DUGENTLE,
    TILE_TYPE_LGENTLE,
    TILE_TYPE_DLGENTLE,

    TILE_END_OF_FLIPPABLE,
    TILE_TYPE_BLOCK = TILE_END_OF_FLIPPABLE,
    TILE_TYPE_SSLOPE,
    TILE_TYPE_SSLAB,
    TILE_TYPE_CULL,
    TILE_TYPE_TROLL,
    TILE_TYPE_FENCE,
    TILE_TYPE_POLE,
    TILE_TYPE_BARS,

    TILE_TYPE_WATER, // only blocks that are empty otherwise
};

enum MBMaterials {
    MB_MATLIST_START,

    // Terrain
    MB_MAT_GRASS = MB_MATLIST_START,
    MB_MAT_GRASS_OLD,
    MB_MAT_CARTOON_GRASS,
    MB_MAT_DARK_GRASS,
    MB_MAT_HMC_GRASS,
    MB_MAT_ORANGE_GRASS,
    MB_MAT_RED_GRASS,
    MB_MAT_PURPLE_GRASS,
    MB_MAT_SAND,
    MB_MAT_JRB_SAND,
    MB_MAT_SNOW,
    MB_MAT_SNOW_OLD,

    MB_MAT_DIRT,
    MB_MAT_SANDDIRT,
    MB_MAT_LIGHTDIRT,
    MB_MAT_HMC_DIRT,
    MB_MAT_ROCKY_DIRT,
    MB_MAT_DIRT_OLD,
    MB_MAT_WAVY_DIRT,
    MB_MAT_WAVY_DIRT_BLUE,
    MB_MAT_SNOWDIRT,
    MB_MAT_PURPLE_DIRT,
    MB_MAT_HMC_LAKEGRASS,

    MB_MATLIST_TERRAIN_END,

    // Stone
    MB_MAT_STONE = MB_MATLIST_TERRAIN_END,
    MB_MAT_HMC_STONE,
    MB_MAT_HMC_MAZEFLOOR,
    MB_MAT_CCM_ROCK,
    MB_MAT_TTM_FLOOR,
    MB_MAT_TTM_ROCK,
    MB_MAT_COBBLESTONE,
    MB_MAT_JRB_WALL,
    MB_MAT_GABBRO,
    MB_MAT_RHR_STONE,
    MB_MAT_LAVA_ROCKS,
    MB_MAT_VOLCANO_WALL,
    MB_MAT_RHR_BASALT,
    MB_MAT_OBSIDIAN,
    MB_MAT_CASTLE_STONE,
    MB_MAT_JRB_UNDERWATER,
    MB_MAT_SNOW_ROCK,
    MB_MAT_ICY_ROCK,
    MB_MAT_DESERT_STONE,
    MB_MAT_RHR_OBSIDIAN,
    MB_MAT_JRB_STONE,

    MB_MATLIST_STONE_END,

    // Bricks
    MB_MAT_BRICKS = MB_MATLIST_STONE_END,
    MB_MAT_DESERT_BRICKS,
    MB_MAT_RHR_BRICK,
    MB_MAT_HMC_BRICK,
    MB_MAT_LIGHTBROWN_BRICK,
    MB_MAT_WDW_BRICK,
    MB_MAT_TTM_BRICK,
    MB_MAT_C_BRICK,
    MB_MAT_BBH_BRICKS,
    MB_MAT_ROOF_BRICKS,
    MB_MAT_C_OUTSIDEBRICK,
    MB_MAT_SNOW_BRICKS,
    MB_MAT_JRB_BRICKS,
    MB_MAT_SNOW_TILE_SIDE,
    MB_MAT_TILESBRICKS,

    MB_MATLIST_BRICKS_END,

    // Tiling
    MB_MAT_TILES = MB_MATLIST_BRICKS_END,
    MB_MAT_C_TILES,
    MB_MAT_DESERT_TILES,
    MB_MAT_VP_BLUETILES,
    MB_MAT_SNOW_TILES,
    MB_MAT_JRB_TILETOP,
    MB_MAT_JRB_TILESIDE,
    MB_MAT_HMC_TILES,
    MB_MAT_GRANITE_TILES,
    MB_MAT_RHR_TILES,
    MB_MAT_VP_TILES,
    MB_MAT_DIAMOND_PATTERN,
    MB_MAT_C_STONETOP,
    MB_MAT_SNOW_BRICK_TILES,

    MB_MATLIST_TILES_END,

    // Cut Stone
    MB_MAT_DESERT_BLOCK = MB_MATLIST_TILES_END,
    MB_MAT_VP_BLOCK,
    MB_MAT_BBH_STONE,
    MB_MAT_BBH_STONE_PATTERN,
    MB_MAT_PATTERNED_BLOCK,
    MB_MAT_HMC_SLAB,
    MB_MAT_RHR_BLOCK,
    MB_MAT_GRANITE_BLOCK,
    MB_MAT_C_STONESIDE,
    MB_MAT_C_PILLAR,
    MB_MAT_BBH_PILLAR,
    MB_MAT_RHR_PILLAR,

    MB_MATLIST_CUTSTONE_END,

    // Wood
    MB_MAT_WOOD = MB_MATLIST_CUTSTONE_END,
    MB_MAT_BBH_WOOD_FLOOR,
    MB_MAT_BBH_WOOD_WALL,
    MB_MAT_C_WOOD,
    MB_MAT_JRB_WOOD,
    MB_MAT_JRB_SHIPSIDE,
    MB_MAT_JRB_SHIPTOP,
    MB_MAT_BBH_HAUNTED_PLANKS,
    MB_MAT_BBH_ROOF,
    MB_MAT_SOLID_WOOD,
    MB_MAT_RHR_WOOD,

    MB_MATLIST_WOOD_END,

    // Metal
    MB_MAT_BBH_METAL = MB_MATLIST_WOOD_END,
    MB_MAT_JRB_METALSIDE,
    MB_MAT_JRB_METAL,
    MB_MAT_C_BASEMENTWALL,
    MB_MAT_DESERT_TILES2,
    MB_MAT_VP_RUSTYBLOCK,

    MB_MATLIST_METAL_END,

    // Buildings
    MB_MAT_C_CARPET = MB_MATLIST_METAL_END,
    MB_MAT_C_WALL,
    MB_MAT_ROOF,
    MB_MAT_C_ROOF,
    MB_MAT_SNOW_ROOF,
    MB_MAT_BBH_WINDOW,
    MB_MAT_HMC_LIGHT,
    MB_MAT_VP_CAUTION,
    MB_MAT_RR_BLOCKS,
    MB_MAT_STUDDED_TILE,
    MB_MAT_TTC_BLOCK,
    MB_MAT_TTC_SIDE,
    MB_MAT_TTC_WALL,
    MB_MAT_FLOWERS,

    MB_MATLIST_BUILDING_END,

    // Hazards
    MB_MAT_LAVA = MB_MATLIST_BUILDING_END,
    MB_MAT_LAVA_OLD,
    MB_MAT_SERVER_ACID,
    MB_MAT_BURNING_ICE,
    MB_MAT_QUICKSAND,
    MB_MAT_DESERT_SLOWSAND,
    MB_MAT_VP_VOID,

    MB_MATLIST_HAZARD_END,
    
    // Transparent
    MB_MAT_RHR_MESH = MB_MATLIST_HAZARD_END,
    MB_MAT_VP_MESH,
    MB_MAT_HMC_MESH,
    MB_MAT_BBH_MESH,
    MB_MAT_PINK_MESH,
    MB_MAT_TTC_MESH,
    MB_MAT_ICE,
    MB_MAT_CRYSTAL,
    MB_MAT_VP_SCREEN,

    MB_MATLIST_END,

    // Retro
    MB_MAT_RETRO_GROUND = MB_MATLIST_END,
    MB_MAT_RETRO_BRICKS,
    MB_MAT_RETRO_TREETOP,
    MB_MAT_RETRO_TREEPLAT,
    MB_MAT_RETRO_BLOCK,
    MB_MAT_RETRO_BLUEGROUND,
    MB_MAT_RETRO_BLUEBRICKS,
    MB_MAT_RETRO_BLUEBLOCK,
    MB_MAT_RETRO_WHITEBRICK,
    MB_MAT_RETRO_LAVA,
    MB_MAT_RETRO_UNDERWATERGROUND,

    // Minecraft
    MB_MAT_MC_DIRT,
    MB_MAT_MC_GRASS,
    MB_MAT_MC_COBBLESTONE,
    MB_MAT_MC_STONE,
    MB_MAT_MC_OAK_LOG_TOP,
    MB_MAT_MC_OAK_LOG_SIDE,
    MB_MAT_MC_OAK_LEAVES,
    MB_MAT_MC_WOOD_PLANKS,
    MB_MAT_MC_SAND,
    MB_MAT_MC_BRICKS,
    MB_MAT_MC_LAVA,
    MB_MAT_MC_FLOWING_LAVA,
    MB_MAT_MC_GLASS,
};

enum MBObjectTypes{
    OBJECT_TYPE_SETTINGS,
    OBJECT_TYPE_1, // empty, used to be for screenshot
    OBJECT_TYPE_STAR,
    OBJECT_TYPE_RED_COIN_STAR,
    OBJECT_TYPE_GOOMBA,
    OBJECT_TYPE_BIG_GOOMBA,
    OBJECT_TYPE_TINY_GOOMBA,
    OBJECT_TYPE_PIRANHA_PLANT,
    OBJECT_TYPE_BIG_PIRANHA_PLANT,
    OBJECT_TYPE_TINY_PIRANHA_PLANT,
    OBJECT_TYPE_KOOPA,
    OBJECT_TYPE_COIN,
    OBJECT_TYPE_GREEN_COIN,
    OBJECT_TYPE_RED_COIN,
    OBJECT_TYPE_BLUE_COIN,
    OBJECT_TYPE_BLUE_COIN_SWITCH,
    OBJECT_TYPE_NOTEBLOCK,
    OBJECT_TYPE_BOBOMB,
    OBJECT_TYPE_CHUCKYA,
    OBJECT_TYPE_BULLY,
    OBJECT_TYPE_CHILL_BULLY,
    OBJECT_TYPE_BULLET_BILL,
    OBJECT_TYPE_HEAVE_HO,
    OBJECT_TYPE_MOTOS,
    OBJECT_TYPE_TREE,
    OBJECT_TYPE_EXCL_BOX,
    OBJECT_TYPE_MARIO_SPAWN,
    OBJECT_TYPE_REX,
    OBJECT_TYPE_PODOBOO,
    OBJECT_TYPE_CRABLET,
    OBJECT_TYPE_HAMMER_BRO,
    OBJECT_TYPE_FIRE_BRO,
    OBJECT_TYPE_CHICKEN,
    OBJECT_TYPE_PHANTASM,
    OBJECT_TYPE_WARP_PIPE,
    OBJECT_TYPE_BADGE,
    OBJECT_TYPE_KING_BOBOMB,
    OBJECT_TYPE_KING_WHOMP,
    OBJECT_TYPE_BIG_BOO,
    OBJECT_TYPE_BIG_BULLY,
    OBJECT_TYPE_BIG_CHILL_BULLY,
    OBJECT_TYPE_WIGGLER,
    OBJECT_TYPE_BOWSER,
    OBJECT_TYPE_PLATFORM_TRACK,
    OBJECT_TYPE_PLATFORM_LOOPING,
    OBJECT_TYPE_BOWLING_BALL,
    OBJECT_TYPE_KOOPA_THE_QUICK,
    OBJECT_TYPE_PURPLE_SWITCH,
    OBJECT_TYPE_TIMED_BOX,
    OBJECT_TYPE_RECOVERY_HEART,
    OBJECT_TYPE_TEST_MARIO,// Fake type, used for the Test mario preview
    OBJECT_TYPE_THWOMP,
    OBJECT_TYPE_WHOMP,
    OBJECT_TYPE_GRINDEL,
    OBJECT_TYPE_LAKITU,
    OBJECT_TYPE_FLY_GUY,
    OBJECT_TYPE_SNUFIT,
    OBJECT_TYPE_AMP,
    OBJECT_TYPE_BOO,
    OBJECT_TYPE_MR_I,
    OBJECT_TYPE_SCUTTLEBUG,
    OBJECT_TYPE_BOWSER_BOMB,
    OBJECT_TYPE_FIRE_SPINNER,
    OBJECT_TYPE_COIN_FORMATION,
    OBJECT_TYPE_RED_FLAME,
    OBJECT_TYPE_BLUE_FLAME,
    OBJECT_TYPE_FIRE_SPITTER,
    OBJECT_TYPE_FLAMETHROWER,
    OBJECT_TYPE_SPINDRIFT,
    OBJECT_TYPE_MR_BLIZZARD,
    OBJECT_TYPE_MONEYBAG,
    OBJECT_TYPE_SKEETER,
    OBJECT_TYPE_POKEY,
    OBJECT_TYPE_BBOX_SMALL,
    OBJECT_TYPE_BBOX_NORMAL,
    OBJECT_TYPE_BBOX_CRAZY,
    OBJECT_TYPE_DIAMOND,
    OBJECT_TYPE_SIGN,
    OBJECT_TYPE_BUDDY,
    OBJECT_TYPE_BUTTON,
    OBJECT_TYPE_ON_OFF_BLOCK,
    OBJECT_TYPE_WOODPLAT,
    OBJECT_TYPE_RFBOX,
    OBJECT_TYPE_CULL_PREVIEW, // also fake type
    OBJECT_TYPE_SHOWRUNNER,
    OBJECT_TYPE_CROWBAR,
    OBJECT_TYPE_MASK,
    OBJECT_TYPE_TOAD,
    OBJECT_TYPE_TUXIE,
    OBJECT_TYPE_UKIKI,
    OBJECT_TYPE_MOLEMAN,
    OBJECT_TYPE_COBIE,
    OBJECT_TYPE_CONVEYOR,
    OBJECT_TYPE_TIMEDBLOCK,
    OBJECT_TYPE_TRIGGER,
    OBJECT_TYPE_TRIGGER_STAR,
};

enum MBButtonTypes{
    MB_BUTTON_SETTINGS,
    MB_BUTTON_TEST,
    MB_BUTTON_TERRAIN,
    MB_BUTTON_SLOPE,
    MB_BUTTON_TROLL,
    MB_BUTTON_STAR,
    MB_BUTTON_GOOMBA,
    MB_BUTTON_PIRANHA,
    MB_BUTTON_KOOPA,
    MB_BUTTON_COIN,
    MB_BUTTON_BLANK,
    MB_BUTTON_GCOIN,
    MB_BUTTON_CORNER,
    MB_BUTTON_ICORNER,
    MB_BUTTON_RCOIN,
    MB_BUTTON_BCOIN,
    MB_BUTTON_NOTEBLOCK,
    MB_BUTTON_CULL,
    MB_BUTTON_BOBOMB,
    MB_BUTTON_CHUCKYA,
    MB_BUTTON_BULLY,
    MB_BUTTON_BULLET,
    MB_BUTTON_HEAVEHO,
    MB_BUTTON_MOTOS,
    MB_BUTTON_TREE,
    MB_BUTTON_EXCLA,
    MB_BUTTON_SPAWN,
    MB_BUTTON_REX,
    MB_BUTTON_PODOBOO,
    MB_BUTTON_CRABLET,
    MB_BUTTON_HAMMER_BRO,
    MB_BUTTON_CHICKEN,
    MB_BUTTON_PHANTASM,
    MB_BUTTON_PIPE,
    MB_BUTTON_BADGE,
    MB_BUTTON_WATER,
    MB_BUTTON_FENCE,
    MB_BUTTON_KING_BOBOMB,
    MB_BUTTON_WIGGLER,
    MB_BUTTON_BOWSER,
    MB_BUTTON_MPLAT,
    MB_BUTTON_BBALL,
    MB_BUTTON_KTQ,
    MB_BUTTON_SSLOPE,
    MB_BUTTON_SLAB,
    MB_BUTTON_PURPLE_SWITCH,
    MB_BUTTON_TIMED_BOX,
    MB_BUTTON_HEART,
    MB_BUTTON_FORMATION,
    MB_BUTTON_VSLAB,
    MB_BUTTON_SCORNER,
    MB_BUTTON_UGENTLE,
    MB_BUTTON_LGENTLE,
    MB_BUTTON_BARS,
    MB_BUTTON_THWOMP,
    MB_BUTTON_WHOMP,
    MB_BUTTON_POLE,
    MB_BUTTON_VEXCLA,
    MB_BUTTON_LAKITU,
    MB_BUTTON_FLYGUY,
    MB_BUTTON_SNUFIT,
    MB_BUTTON_AMP,
    MB_BUTTON_BOO,
    MB_BUTTON_MR_I,
    MB_BUTTON_SCUTTLEBUG,
    MB_BUTTON_SPINDRIFT,
    MB_BUTTON_BLIZZARD,
    MB_BUTTON_MONEYBAG,
    MB_BUTTON_SKEETER,
    MB_BUTTON_POKEY,
    MB_BUTTON_MINE,
    MB_BUTTON_FIRE,
    MB_BUTTON_FLAMETHROWER,
    MB_BUTTON_FIRE_SPITTER,
    MB_BUTTON_FIRE_SPINNER,
    MB_BUTTON_BREAKABLE,
    MB_BUTTON_SMALL_BOX,
    MB_BUTTON_DIAMOND,
    MB_BUTTON_NPC,
    MB_BUTTON_NPCCM,
    MB_BUTTON_BUTTON,
    MB_BUTTON_BLOCK,
    MB_BUTTON_WOODPLAT,
    MB_BUTTON_RFBOX,
    MB_BUTTON_SHOWRUN,
    MB_BUTTON_POWER,
    MB_BUTTON_CONVEYOR,
    MB_BUTTON_ISCORNER,
    MB_BUTTON_TRIGGER,

    MB_BUTTON_COUNT,
};


extern f32 mb_camera_zoom_table[][2];
extern void *mb_theme_segments[][4];
extern LevelScript *mb_theme_model_scripts[];
extern u8 mb_rotated_dirs[4][6];

extern void *slope_decal_below_surfs[];
extern struct MBTerrainPoly mb_terrain_fullblock_quads[];
extern struct MBTerrain mb_terrain_fullblock;
extern struct MBTerrainPoly mb_terrain_bars_connected_quads[];
extern struct MBTerrainPoly mb_terrain_bars_unconnected_quad[];
extern struct MBTerrainPoly mb_terrain_bars_center_quads[];
extern struct MBTerrain mb_terrain_pole;
extern struct MBTerrain mb_terrain_fence;
extern struct MBTerrain mb_terrain_fence_col;
extern struct MBTerrainPoly *mb_terrain_water_quadlists[];
extern struct MBTerrainInfo mb_terrain_info_list[];

extern u8 mb_matlist[];
extern struct MBMaterial mb_mat_table[];
extern u32 mb_render_mode_table[];
extern struct MBTheme mb_theme_table[];
extern struct MBCustomTheme mb_default_custom;
extern struct MBCustomTheme mb_curr_custom_theme;

// Returns full tile definition (struct mb_tilemat_def)
#define TILE_MATDEF(matid) (mb_theme_table[mb_lopt_theme].mats[matid])
// Returns main material (struct mb_material)
#define MATERIAL(matid) (mb_mat_table[TILE_MATDEF(matid).mat])

// Returns TRUE if given material has a unique top texture
#define HAS_TOPMAT(matid) (TILE_MATDEF(matid).topmat != TILE_MATDEF(matid).mat)
// Returns top material's topmat struct (struct mb_material)
#define TOPMAT(matid) (mb_mat_table[TILE_MATDEF(matid).topmat])

extern Gfx *mb_fence_texs[];
extern Gfx *mb_bar_texs[][2];
extern Gfx *mb_water_texs[];

// Returns current fence texture
#define FENCE_TEX() (mb_fence_texs[mb_theme_table[mb_lopt_theme].fence])
#define POLE_TEX()  (mb_mat_table[mb_theme_table[mb_lopt_theme].pole].gfx)
#define BARS_TEX() (mb_bar_texs[mb_theme_table[mb_lopt_theme].bars][0])
#define BARS_TOPTEX() (mb_bar_texs[mb_theme_table[mb_lopt_theme].bars][1])
#define WATER_TEX() (mb_water_texs[mb_theme_table[mb_lopt_theme].water])

extern struct MBExclamationBoxContents sExclamationBoxContents_btcm[];
extern struct MBExclamationBoxContents sExclamationBoxContents_vanilla[];
extern struct MBObjectInfo mb_object_type_list[];
extern struct MBButtonUIType mb_ui_buttons[];

extern u8 mb_toolbox_btcm[18 * 5];
extern u8 mb_toolbox_vanilla[18 * 5];
extern u8 seq_musicmenu_array[];
extern u8 mb_envfx_table[];
extern u8 mb_boundary_table[];
extern u8 *mb_skybox_table[];

extern struct mb_settings_button mb_settings_mat_selector[];
extern struct mb_settings_button mb_settings_other_selectors[];

extern struct MBTemplate mb_templates[];
extern u8 mb_text_colors[][3];

extern struct mb_dialog_subject mb_dialog_subjects[];
#define NUM_DIALOG_SUBJECT_COUNT 6

extern struct MBImbueData imbue_table[];

#endif