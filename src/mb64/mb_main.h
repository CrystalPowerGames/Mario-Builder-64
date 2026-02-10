#pragma once

#ifndef MB_MAIN_H
#define MB_MAIN_H

#include "mb_types.h"
#include "mb_data.h"

#include "model_ids.h"
#include "seq_ids.h"
#include "sounds.h"
#include "surface_terrains.h"
#include "levels/scripts.h"
#include "game/level_geo.h"

#include "libcart/ff/ff.h"

#define MB_TILE_POOL_SIZE 20000 //20000 -> 20479
#define MB_GFX_SIZE 20000 //20000 -> 20479
#define MB_VTX_SIZE 50000 //50000 -> 49151

#define MB_MAX_OBJS 512 //512 -> 511
#define MB_MAX_TRAJECTORIES 20
#define MB_TRAJECTORY_LENGTH 50

#define MB_SPEEDRUN_TIMER_MAX (30 * 60 * 100 - 1)
#define TILE_SIZE 0xff //255

#define MB_VERSION 1
#define MAX_FILE_NAME_SIZE 41
#define MAX_FILE_NAME_INPUT (MAX_FILE_NAME_SIZE - 6)
#define MAX_USERNAME_SIZE 31
#define MAX_USERNAME_INPUT (MAX_USERNAME_SIZE - 1)

//Cursor Position
extern s8 MBCursorPos[3];

//Level Options Struct
struct MBLevelOptions {
    s8 costume; //Equipped Costume
    u8 seq[5]; //Song index
    u8 seqType; //Level Music, Race Music, Boss Music
    u8 seqAlbum; //Category
    u8 seqCategory; //Song index (within category)
    u8 envFX; //Environmental effects
    u8 theme; //Custom theme
    u8 background; //Background
    u8 boundaryMaterial; //Boundary material
    u8 boundary; //If there even is a boundary
    u8 boundaryHeight; //Height of the boundary
    u8 game; //Game Mode (Vanilla, BTCM, etc.)
    u8 size; //Level size (small, medium, large, etc.)
    u8 template; //Level template
    u8 coinStar; //If there is a x-coin star
    u8 coinStarMax; //How many coins are needed to obtain the coin star
    u8 waterLevel; //The water level
    u8 secret; //Secret TODO: verify what this does
};

//Level Options Global Variable
extern struct MBLevelOptions *gLevelOptions;

//Function declarations
void mb_level_options_init(void);

//UVs, etc
extern u8 mb_use_alt_uvs;
extern s8 mb_uv_offset;
extern u8 mb_render_flip_normals;
extern u8 mb_render_vertical;
extern u8 mb_render_culling_off;
extern u8 mb_growth_render_type;
extern u8 mb_curr_mat_has_topside;
extern u8 mb_curr_poly_vert_count;
extern u8 mb_curr_boundary;
extern u8 mb_upsidedown_tile;

//Objects and tiles
extern struct MBTile mb_tile_data[MB_TILE_POOL_SIZE];
extern struct MBObject mb_object_data[MB_MAX_OBJS];
extern u16 mb_tile_data_indices[NUM_MATERIALS_PER_THEME + 10];
extern u16 mb_tile_count;
extern u16 mb_object_count;
extern u16 mb_object_limit_count;
extern u16 mb_build_collision_type;
extern u16 mb_total_coin_count;

//Cursors/selections
extern u8 mb_place_mode;
extern s8 mb_id_selection;
extern u8 mb_rot_selection;
extern s16 mb_param_selection;
extern s16 mb_mat_selection;

//Freecam
extern s16 mb_freecam_pitch;
extern s16 mb_freecam_yaw;
extern u8 mb_freecam_snap;
extern u8 mb_freecam_snap_timer;

//Dialogs
extern s8 mb_dialog_subject_index;
extern s8 mb_dialog_topic_index;

//File information
extern FILINFO mb_file_info;

#define AT_CEILING(y) ((mb_curr_boundary & MB_BOUNDARY_CEILING) && ((y) == mb_lopt_boundary_height-1))

void mb_background_reload(void);
void mb_level_save(void);
void mb_level_load(void);
void sb_loop(void);
void sb_init(void);
void mb_init();
void reset_play_state(void);
void generate_objects_to_level(void);
void block_floor_collision(f32 x, f32 y, f32 z);
void block_ceil_collision(f32 x, f32 y, f32 z);
void block_wall_collision(f32 x, f32 y, f32 z, f32 r);
Gfx *mb_append(s32 callContext, UNUSED struct GraphNode *node, UNUSED Mat4 mtx);
extern Gfx mb_terrain_gfx[MB_GFX_SIZE];
extern Trajectory mb_trajectory_list[MB_MAX_TRAJECTORIES][MB_TRAJECTORY_LENGTH][4];
void rotate_obj_toward_trajectory_angle(struct Object * obj, u32 traj_id);
void play_mb_extra_music(u8 index);
void stop_mb_extra_music(u8 index);
void animate_list_reset(void);
void generate_terrain_gfx(void);
void reload_boundary_and_gfx(void);
void freecam_camera_init(void);
void custom_theme_update(void);
void reload_theme(void);
void custom_theme_draw_block(f32 xpos, f32 ypos, s32 index);

extern u8 gLevelAction;
extern u8 mb_mode;
extern u8 mb_target_mode;
extern Vec3f mb_camera_pos;
extern Vec3f mb_camera_foc;
extern f32 mb_camera_fov;

extern u16 painting_rgba16[32][32];
extern u8 mb_envfx_table[];

//Play mode stuff
extern s16 mb_play_stars;
extern s16 mb_play_stars_max;
extern u64 mb_play_stars_bitfield;
extern u32 mb_play_badge_bitfield;
extern u8 mb_play_onoff;
extern s16 mb_play_s16_water_level;
extern u32 mb_play_speedrun_timer;

extern struct MBLevelSaveHeader mb_save;
extern TCHAR mb_file_name[MAX_FILE_NAME_SIZE];
extern char mb_username[MAX_USERNAME_SIZE];
extern u8 mb_has_username;

extern void *mb_theme_segments[][4];
extern LevelScript * mb_theme_model_scripts[];

enum MBPlaceModes {
    MB_PM_NONE,
    MB_PM_ACTION,
    MB_PM_TILE,
    MB_PM_OBJ,
};

enum MBProcessTileRenderModes {
    PROCESS_TILE_NORMAL,
    PROCESS_TILE_TRANSPARENT,
    PROCESS_TILE_BOTH,
    PROCESS_TILE_VPLEX,
};

#define GRID_TO_POS(gridx) ((gridx) * TILE_SIZE - (32 * TILE_SIZE) + TILE_SIZE/2)
#define POS_TO_GRID(pos) (((pos) + (32 * TILE_SIZE) - TILE_SIZE/2) / TILE_SIZE)

extern struct MBGridObject mb_grid_data[64][64][64];
#define get_grid_tile(pos) (&(mb_grid_data[(pos)[0]][(pos)[1]][(pos)[2]]))

enum MBDirections {
    MB_DIRECTION_UP,
    MB_DIRECTION_DOWN,
    MB_DIRECTION_POS_X,
    MB_DIRECTION_NEG_X,
    MB_DIRECTION_POS_Z,
    MB_DIRECTION_NEG_Z,
};

enum MBCullingShapes {
    MB_FACESHAPE_FULL,
    MB_FACESHAPE_POLETOP,

    MB_FACESHAPE_TRI_1, // make sure irregular shapes can be flipped with ^1
    MB_FACESHAPE_TRI_2,
    MB_FACESHAPE_DOWNTRI_1,
    MB_FACESHAPE_DOWNTRI_2,
    MB_FACESHAPE_HALFSIDE_1, // vertical slab sides
    MB_FACESHAPE_HALFSIDE_2,

    MB_FACESHAPE_TOPTRI,
    MB_FACESHAPE_TOPHALF,

    // & 0x10: Bottom slab priority list
    MB_FACESHAPE_BOTTOMSLAB_PRI = 0x10,
    MB_FACESHAPE_UPPERGENTLE_1 = MB_FACESHAPE_BOTTOMSLAB_PRI,
    MB_FACESHAPE_UPPERGENTLE_2,
    MB_FACESHAPE_BOTTOMSLAB,
    // 0x13 empty
    MB_FACESHAPE_LOWERGENTLE_1 = MB_FACESHAPE_BOTTOMSLAB_PRI + 4,
    MB_FACESHAPE_LOWERGENTLE_2,
    
    // & 0x20: Top slab priority list
    MB_FACESHAPE_TOPSLAB_PRI = 0x20,
    MB_FACESHAPE_DOWNUPPERGENTLE_1 = MB_FACESHAPE_TOPSLAB_PRI,
    MB_FACESHAPE_DOWNUPPERGENTLE_2,
    MB_FACESHAPE_TOPSLAB,
    // 0x23 empty
    MB_FACESHAPE_DOWNLOWERGENTLE_1 = MB_FACESHAPE_TOPSLAB_PRI + 4,
    MB_FACESHAPE_DOWNLOWERGENTLE_2,

    // & 0x40: Empty faces
    MB_FACESHAPE_EMPTY = 0x40,
    // Rotate UVs for certain textures
    MB_FACESHAPE_EMPTY_0,
    MB_FACESHAPE_EMPTY_1,
    MB_FACESHAPE_EMPTY_2,
    MB_FACESHAPE_EMPTY_3,
};

enum MBGrowthTypes {
    MB_GROWTH_NONE,
    MB_GROWTH_FULL,
    MB_GROWTH_NORMAL_SIDE,
    MB_GROWTH_HALF_SIDE, // vertical slabs - either side
    MB_GROWTH_UNDERSLOPE_CORNER, // special check
    MB_GROWTH_DIAGONAL_SIDE,
    MB_GROWTH_VSLAB_SIDE, // vertical slabs - middle face
    MB_GROWTH_UNCONDITIONAL,

    // Anything beyond this is a slope decal type
    // & 1 - left or right side
    // & 2 - gentle or steep
    MB_GROWTH_EXTRADECAL_START = 0x10,
    MB_GROWTH_SLOPE_SIDE_L = MB_GROWTH_EXTRADECAL_START,
    MB_GROWTH_SLOPE_SIDE_R,
    MB_GROWTH_GENTLE_SIDE_L,
    MB_GROWTH_GENTLE_SIDE_R,
};

enum MBDisplayFunctionContexts {
    MB_DF_CONTEXT_INIT,
    MB_DF_CONTEXT_MAIN,
};

#define OBJ_TYPE_BILLBOARD (1 << 0)
#define OBJ_TYPE_TRAJECTORY     (1 << 1)
#define OBJ_TYPE_STAR       (1 << 2)
#define OBJ_TYPE_HAS_DIALOG     (1 << 3)
#define OBJ_TYPE_IMBUABLE       (1 << 4)
#define OBJ_TYPE_IMBUABLE_COINS (1 << 5)
#define OBJ_TYPE_IMBUABLE_TRIGGER (1 << 6)

#define OBJ_OCCUPY_OUTER        (1 << 0)
#define OBJ_OCCUPY_INNER        (1 << 1)

#define OBJ_OCCUPY_FULL        (OBJ_OCCUPY_OUTER | OBJ_OCCUPY_INNER)

extern struct MBExclamationBoxContents *mb_exclamation_box_contents;

enum MBModes {
    MB_MODE_PLAY,
    MB_MODE_MAKE,
    MB_MODE_UNINITIALIZED,
};

extern 

enum MBMakeModes {
    MB_MAKE_MAIN,
    MB_MAKE_PLAY,
    MB_MAKE_TOOLBOX,
    MB_MAKE_SETTINGS,
    MB_MAKE_TRAJECTORY,
    MB_MAKE_SCREENSHOT,
};
extern u8 gMenuState;

struct MBCustomTheme {
    u8 mats[NUM_MATERIALS_PER_THEME];
    u8 topmats[NUM_MATERIALS_PER_THEME];
    u8 topmatsEnabled[NUM_MATERIALS_PER_THEME];
    u8 fence;
    u8 pole;
    u8 bars;
    u8 water;
};

//compressed trajectories
struct MBCompTrajs {
    s8 t;
    u8 x;
    u8 y;
    u8 z;
};

/*
IMPORTANT!

char file_header[10];
u8 version;
char author[31];
u16 piktcher[64][64];

Should always be the first 2 members of the mb_level_save_header struct
no matter what version.
*/

struct MBLevelSaveHeader {
    char file_header[10];
    u8 version;
    char author[MAX_USERNAME_SIZE];
    u16 piktcher[64][64];

    // Level options
    u8 costume;
    u8 seq[5];
    u8 envfx;
    u8 theme;
    u8 bg;
    u8 boundary_mat;
    u8 boundary;
    u8 boundary_height;
    u8 coinstar;
    u8 size;
    u8 waterlevel;
    u8 secret;
    u8 game;

    u8 toolbar[9];
    u8 toolbar_params[9];
    u16 tile_count;
    u16 object_count;

    struct MBCustomTheme custom_theme;

    struct MBCompTrajs trajectories[MB_MAX_TRAJECTORIES][MB_TRAJECTORY_LENGTH];

    u64 pad;
};

#define SRAM_MAGIC 0x0203DD10 // Rovert's favorite binary ROM Address!

struct MBSramConfig {
    char author[MAX_USERNAME_SIZE];
    u8 option_flags;
    u32 magic;
    u64 pad;
};

enum MBMenuModes {
    MM_INIT,
    MM_NO_SD_CARD,
    MM_MAIN,
    MM_MAIN_LIMITED,
    MM_HELP_MODE,
    MM_INFO,
    MM_PLAY,
    MM_MAKE,
    MM_KEYBOARD,
    MM_FILES,
    MM_MAKE_MODE,
};

enum {
    KXM_NEW_LEVEL,
    KXM_NEW_LEVEL_LIMITED,
    KXM_AUTHOR,
    KXM_CHANGE_AUTHOR,
};

extern u8 mb_lopt_game;
enum MBGameModes {
    MB_GAME_VANILLA,
    MB_GAME_BTCM,
};

enum MBLevelActions {
    MB_LA_PLAY_LEVELS,
    MB_LA_BUILD,
    MB_LA_TEST_LEVEL,
};

enum MBThemes {
    MB_THEME_GENERIC,
    MB_THEME_SSL,
    MB_THEME_RHR,
    MB_THEME_HMC,
    MB_THEME_CASTLE,
    MB_THEME_VIRTUAPLEX,
    MB_THEME_SNOW,
    MB_THEME_BBH,
    MB_THEME_JRB,
    MB_THEME_RETRO,
    MB_THEME_CUSTOM,
    MB_THEME_MC,
};

enum MBImbueTypes {
    IMBUE_NONE,
    IMBUE_STAR,
    IMBUE_THREE_COINS,
    IMBUE_ONE_COIN,
    IMBUE_GREEN_COIN,
    IMBUE_BLUE_COIN,
    IMBUE_RED_SWITCH,
    IMBUE_BLUE_SWITCH,
    IMBUE_RED_COIN,
    IMBUE_TRIGGER,
    IMBUE_CROWBAR,
    IMBUE_BULLET_MASK,
    IMBUE_BADGE_BASE,
};

extern s32 gMBMinCoord;
extern s32 gMBMaxCoord;
#define MB_BOUNDARY_INNER_FLOOR   (1 << 0) // Has the main floor
#define MB_BOUNDARY_OUTER_FLOOR   (1 << 1) // Has the fading outer floor
#define MB_BOUNDARY_INNER_WALLS   (1 << 2) // Has the inner walls going up to boundary height - has fading part if death plane
#define MB_BOUNDARY_OUTER_WALLS   (1 << 3) // Has fading outer walls extending downwards
#define MB_BOUNDARY_CEILING       (1 << 4) // Ceiling above the level

#endif