//SM64 Engine libs
#include <PR/gbi.h>
#include <string.h>
#include "audio/external.h"
#include "game/spawn_sound.h"
#include "game/object_list_processor.h"
#include "actors/maker/header.h"
#include "actors/b/header.h"
#include "actors/bigpainting2/header.h"
#include "engine/surface_load.h"
#include "game/game_init.h"
#include "game/mario_actions_automatic.h"
#include "game/level_update.h"
#include "game/emutest.h"
#include "buffers/framebuffers.h"
#include "game/main.h"
#include "game/puppyprint.h"
#include "engine/math_util.h"
#include "game/ingame_menu.h"
#include "game/mario.h"
#include "game/rendering_graph_node.h"
#include "behavior_data.h"
#include "game/geo_misc.h"

//MB libs
#include "mb_main.h"
#include "menu.h"
#include "display_funcs.h"
#include "painting_frame.h" //There might be a better way to include the painting frame without #include

//Cartridge and RHDC libs
#include "libcart/include/cart.h"
#include "libcart/ff/ff.h"
#include "libpl/libpl-rhdc.h"

//???
void super_cum_working(struct Object *o, u8 type);

//Initialize Game States
u8 gMenuState = MB_MAKE_MAIN;
u8 gLevelAction = MB_LA_BUILD;
u8 gMode = MB_MODE_UNINITIALIZED;
u8 mb_target_mode = MB_MODE_MAKE;
s8 mb_cursor_pos[3] = {32, 8, 32};

//Initialize Camera
Vec3f mb_camera_pos = {0.0f, 0.0f, 0.0f};
Vec3f mb_camera_pos_prev;
Vec3f mb_camera_foc = {0.0f, 0.0f, 0.0f};
f32 mb_camera_fov = 45.0f;
s16 mb_camera_angle = 0;
u8 mb_camera_rot_offset = 0;
s8 mb_camera_zoom_index = 2;
f32 mb_current_camera_zoom[2] = {1500.0f, 800.0f};

//Initialize place mode
u8 mb_place_mode = MB_PM_NONE;

//Iniitialize Selections
s8 mb_id_selection = 0;
u8 mb_rot_selection = 0;
s16 mb_param_selection = 0;
s16 mb_mat_selection = 0;

struct MBGridObject mb_grid_data[64][64][64] = {0};

Gfx mb_terrain_gfx[MB_GFX_SIZE];
Gfx *mb_terrain_gfx_tp;
Gfx *mb_trajectory_gfx;
Vtx mb_terrain_vtx[MB_VTX_SIZE];
Vtx *mb_trajectory_vtx;

u32 mb_gfx_total = 0;
u32 mb_vtx_total = 0;

struct MBTile mb_tile_data[MB_TILE_POOL_SIZE];
struct MBObject mb_object_data[MB_MAX_OBJS];
u16 mb_tile_data_indices[NUM_MATERIALS_PER_THEME + 10] = {0};
u16 mb_tile_count = 0;
u16 mb_object_count = 0;
u16 mb_object_limit_count = 0; // Tracks additional objects like in coin formations, flame spinners
u16 mb_build_collision_type = 0; // 0 = none, 1 = floor, 2 = ceil, 3 = wall
u16 mb_total_coin_count = 0;

struct Object *mb_boundary_object[6]; //one for each side

Trajectory mb_trajectory_list[MB_MAX_TRAJECTORIES][MB_TRAJECTORY_LENGTH][4];
u16 mb_trajectory_edit_index = 0;
u8 mb_trajectory_to_edit = 0;
u8 mb_trajectories_used = 0;

Vtx *mb_curr_vtx;
Gfx *mb_curr_gfx;
u16 mb_gfx_index;

u8 mb_use_alt_uvs = FALSE; // Used for decals and special tile shapes
s8 mb_uv_offset = -16;
u8 mb_render_flip_normals = FALSE; // Used for drawing water tiles
u8 mb_render_vertical = FALSE; // Used for prioritizing vertical UVs over horizontal ones
u8 mb_render_culling_off = FALSE; // Used for drawing preview blocks in custom theme menu
u8 mb_growth_render_type = 0; // 0 - normal, 1 - grass top, 2 - grass side, 3 - fence, 4 - pole
u8 mb_curr_mat_has_topside = FALSE;
u8 mb_curr_poly_vert_count = 4; // 3 = tri, 4 = quad
u8 mb_curr_boundary = 0;
u8 mb_upsidedown_tile = FALSE;

TerrainData mb_curr_coltype = SURFACE_DEFAULT;

// dialog edit stuff
struct MBObject * mb_dialog_edit_ptr = NULL;
s8 mb_dialog_subject_index = 0;
s8 mb_dialog_topic_index = 0;

//play mode stuff
s16 mb_play_stars = 0;
s16 mb_play_stars_max = 0;
u64 mb_play_stars_bitfield = 0;
u32 mb_play_badge_bitfield = 0;
u8 mb_play_onoff = FALSE;
s16 mb_play_s16_water_level = 0;
u32 mb_play_speedrun_timer = 0;

//LEVEL SETTINGS INDEX
u8 mb_lopt_costume = 0;

u8 mb_lopt_seq[5] = {0,0,0,0,0}; // Song index
u8 mb_lopt_seq_seqtype = 0; // Level Music, Race Music, Boss Music 
u8 mb_lopt_seq_album = 0; // Category
u8 mb_lopt_seq_song = 0; // Song index within category
u8 mb_lopt_coinstar_max = 0;

u8 mb_lopt_envfx = 0;
u8 mb_lopt_theme = 0;
u8 mb_lopt_bg = 0;
u8 mb_lopt_boundary_mat = 0;
u8 mb_lopt_boundary = 0;
u8 mb_lopt_boundary_height = 0;
u8 mb_lopt_game = MB_GAME_VANILLA;
u8 mb_lopt_size = 0;
u8 mb_lopt_template = 0;
u8 mb_lopt_coinstar = 0;
u8 mb_lopt_waterlevel = 0;
u8 mb_lopt_secret = 0;

struct MBLevelOptions *gLevelOptions;

u8 mb_prepare_level_screenshot = FALSE;
u8 mb_do_save = FALSE;

struct MBLevelSaveHeader mb_save;
char mb_username[MAX_USERNAME_SIZE];
u8 mb_has_username = FALSE;

u8 mb_num_vertices_cached = 0;
u8 mb_num_tris_cached = 0;
u8 mb_cached_tris[64][3];

struct MBExclamationBoxContents *mb_exclamation_box_contents;

s32 mb_count_stars(void) {
    s32 numStars = 0;
    for (s32 i = 0; i < mb_object_count; i++) {
        if (mb_object_type_list[mb_object_data[i].type].flags & OBJ_TYPE_STAR) {
            numStars++;
        }
        if (mb_object_data[i].imbue == IMBUE_STAR) {
            numStars++;
        }
    }
    return numStars;
}

void bhv_preview_object_init(void) {
    if (!o->oPreviewObjDisplayFunc) return;
    ((DisplayFunc)o->oPreviewObjDisplayFunc)(MB_DF_CONTEXT_INIT);
}
void bhv_preview_object_loop(void) {
    if (!o->oPreviewObjDisplayFunc) return;
    ((DisplayFunc)o->oPreviewObjDisplayFunc)(MB_DF_CONTEXT_MAIN);
}

void play_place_sound(u32 soundBits) {
    play_sound(soundBits, gGlobalSoundSource);
}

void reset_play_state(void) {
    mb_play_stars = 0;
    mb_play_stars_bitfield = 0;
    mb_play_badge_bitfield = 0;

    // & water level
    mb_play_s16_water_level = -8224+(mb_lopt_waterlevel*TILE_SIZE);
    gWDWWaterLevelChanging = FALSE;
    mb_play_onoff = FALSE;

    mb_play_speedrun_timer = 0;
}

u8 mb_grid_min = 0;
u8 mb_grid_size = 64;
s32 gMBMinCoord;
s32 gMBMaxCoord;

u32 coords_in_range(s8 pos[3]) {
    if (pos[0] < mb_grid_min || pos[0] > mb_grid_min + mb_grid_size - 1) return FALSE;
    if (pos[1] < 0 || pos[1] > 63) return FALSE;
    if (pos[2] < mb_grid_min || pos[2] > mb_grid_min + mb_grid_size - 1) return FALSE;
    return TRUE;
}

s32 tile_sanity_check(void) {
    if (mb_tile_count >= MB_TILE_POOL_SIZE) {
        show_error("Tile limit reached! (max 20,000)");
        return FALSE;
    }
    if (mb_vtx_total >= MB_VTX_SIZE - 100) {
        if (mb_id_selection != TILE_TYPE_CULL) {
            show_error("Vertex limit reached! (max 50,000)");
            return FALSE;
        }
    }
    if (mb_gfx_total >= MB_GFX_SIZE - 100) {
        if (mb_id_selection != TILE_TYPE_CULL) {
            show_error("Warning: Mesh data pool full!");
            return FALSE;
        }
    }

    return TRUE;
}

// Get number of extra objects used by an object
s32 get_extra_objects(u32 id, s32 param) {
    if (id == OBJECT_TYPE_FIRE_SPINNER) {
        return (param + 2) * 2;
    }
    if (id == OBJECT_TYPE_COIN_FORMATION) {
        return (param <= 1 ? 5 : 8);
    }
    return mb_object_type_list[id].numExtraObjects;
}

s32 object_sanity_check(void) {
    struct MBObjectInfo *info = &mb_object_type_list[mb_id_selection];

    if (mb_object_limit_count + get_extra_objects(mb_id_selection, mb_param_selection) >= MB_MAX_OBJS) {
        show_error("Object limit reached! (max 512)");
        return FALSE;
    }

    if (info->flags & OBJ_TYPE_TRAJECTORY) {
        if (mb_trajectories_used >= MB_MAX_TRAJECTORIES) {
            show_error("Trajectory limit reached! (max 20)");
            return FALSE;
        }
    }

    if (info->flags & OBJ_TYPE_STAR) {
        // Count stars
        s32 numStars = mb_count_stars();
        if (numStars >= 63) {
            show_error("Star limit reached! (max 63)");
            return FALSE;
        }
    }

    if (mb_id_selection == OBJECT_TYPE_RED_COIN_STAR) {
        for (u32 i = 0; i < mb_object_count; i++) {
            if (mb_object_data[i].type == OBJECT_TYPE_RED_COIN_STAR) {
                show_error("Red Coin Star already placed!");
                return FALSE;
            }
        }
    }
    if (mb_id_selection == OBJECT_TYPE_TRIGGER_STAR) {
        for (u32 i = 0; i < mb_object_count; i++) {
            if (mb_object_data[i].type == OBJECT_TYPE_TRIGGER_STAR) {
                show_error("Star Trigger star already placed!");
                return FALSE;
            }
        }
    }

    return TRUE;
}

struct Object * get_spawn_preview_object() {
    uintptr_t *behaviorAddr = segmented_to_virtual(bhvPreviewObject);
    struct ObjectNode *listHead = &gObjectLists[get_object_list_from_behavior(behaviorAddr)];
    struct Object *obj = (struct Object *) listHead->next;

    while (obj != (struct Object *) listHead) {
        if (obj->behavior == behaviorAddr
            && obj->activeFlags != ACTIVE_FLAG_DEACTIVATED
            && obj != o
        ) {
            if (obj->header.gfx.sharedChild == gLoadedGraphNodes[MODEL_SPAWN]) {
                return obj;
            }
        }

        obj = (struct Object *) obj->header.next;
    }

    return NULL;
}

#define place_terrain_data(pos, type_, rot_, mat_) {        \
    mb_grid_data[pos[0]][pos[1]][pos[2]].rot = rot_;       \
    mb_grid_data[pos[0]][pos[1]][pos[2]].type = type_;     \
    mb_grid_data[pos[0]][pos[1]][pos[2]].mat = mat_;       \
    mb_grid_data[pos[0]][pos[1]][pos[2]].waterlogged = 0;  \
}

#define remove_terrain_data(pos) {                         \
    mb_grid_data[pos[0]][pos[1]][pos[2]].rot = 0;         \
    mb_grid_data[pos[0]][pos[1]][pos[2]].type = TILE_TYPE_EMPTY; \
    mb_grid_data[pos[0]][pos[1]][pos[2]].mat = 0;         \
    mb_grid_data[pos[0]][pos[1]][pos[2]].waterlogged = 0; \
}

#define rotate_direction(dir, rot) (mb_rotated_dirs[rot][dir])

u32 get_faceshape(s8 pos[3], u32 dir) {
    struct MBTerrain *terrain;
    if (mb_render_culling_off) return MB_FACESHAPE_EMPTY;

    struct MBGridObject *tile = get_grid_tile(pos);
    s8 tileType = tile->type;
    if (tileType == TILE_TYPE_EMPTY) return MB_FACESHAPE_EMPTY;

    if (tileType == TILE_TYPE_POLE) terrain = &mb_terrain_pole;
    else terrain = mb_terrain_info_list[tileType].terrain;

    if (!terrain) return MB_FACESHAPE_EMPTY;

    u8 rot = tile->rot;
    dir = rotate_direction(dir,((4-rot) % 4)) ^ 1;

    for (u32 i = 0; i < terrain->numTris; i++) {
        struct MBTerrainPoly *tri = &terrain->tris[i];
        if (tri->faceDir == dir) {
            return tri->faceshape;
        }
    }
    for (u32 i = 0; i < terrain->numQuads; i++) {
        struct MBTerrainPoly *quad = &terrain->quads[i];
        if (quad->faceDir == dir) {
            return quad->faceshape;
        }
    }
    return MB_FACESHAPE_EMPTY;
}

ALWAYS_INLINE s32 get_mat(s8 pos[3]) {
    struct MBGridObject *tile = get_grid_tile(pos);
    if (tile->type >= TILE_TYPE_CULL || tile->type == TILE_TYPE_EMPTY) return -1;
    return tile->mat;
}

u32 get_tile_occupy_flags(u32 type) {
    switch (type) {
        case TILE_TYPE_POLE:
            return OBJ_OCCUPY_INNER;
        case TILE_TYPE_FENCE:
        case TILE_TYPE_TROLL:
            return OBJ_OCCUPY_OUTER;
        case TILE_TYPE_CULL:
            return 0;
        default:
            return OBJ_OCCUPY_FULL;
    }
}

u32 can_place(s8 pos[3], u32 occupyFlags) {
    // Tile Check
    u32 type = get_grid_tile(pos)->type;
    if (type != TILE_TYPE_EMPTY && type != TILE_TYPE_WATER) {
        u32 tileFlags = get_tile_occupy_flags(type);
        if (tileFlags & occupyFlags) return FALSE;
    }
    // Object Check
    for (u32 i = 0; i < mb_object_count; i++) {
        struct MBObject *obj = &mb_object_data[i];
        if (obj->x == pos[0] && obj->y == pos[1] && obj->z == pos[2]) {
            u32 objFlags = mb_object_type_list[obj->type].occupy;
            if (obj->type == OBJECT_TYPE_COIN_FORMATION) { // ring override
                if (obj->bparam == 2) objFlags = OBJ_OCCUPY_OUTER;
            }
            
            if (objFlags & occupyFlags) return FALSE;
        }
    }
    return TRUE;
}

u32 can_place_tile(s8 pos[3]) {
    // Tiles can never stack, even if one is outer and one is inner
    u32 type = get_grid_tile(pos)->type;
    if (type != TILE_TYPE_EMPTY && type != TILE_TYPE_WATER) return FALSE;

    u32 flags = get_tile_occupy_flags(mb_id_selection);
    return can_place(pos, flags);
}

u32 can_place_object(s8 pos[3]) {
    u32 flags = mb_object_type_list[mb_id_selection].occupy;
    if (mb_id_selection == OBJECT_TYPE_COIN_FORMATION) {
        if (mb_param_selection == 2) flags = OBJ_OCCUPY_OUTER;
    } else if (mb_id_selection == OBJECT_TYPE_TRIGGER) {
        return FALSE;
    }
    return can_place(pos, flags);
}

s8 cullOffsetLUT[6][3] = {
    {0, 1, 0},
    {0, -1, 0},
    {1, 0, 0},
    {-1, 0, 0},
    {0, 0, 1},
    {0, 0, -1},
};

#define fullblock_can_be_waterlogged(mat) ((MATERIAL(mat).type == MAT_CUTOUT) || (TOPMAT(mat).type == MAT_CUTOUT))

// Classifications for culling checks.
// Lower types of faces cull higher types of faces
enum BlockSideClassifications {
    CLASS_OPAQUE, // < CUTOUT
    CLASS_HOLLOW_TRANSPARENT,
    CLASS_HOLLOW_CUTOUT,
    CLASS_TRANSPARENT, // > CUTOUT
    CLASS_CUTOUT, // = CUTOUT
};

u32 get_side_class(u32 mat, u32 dir) {
    if (mat == -1) return CLASS_CUTOUT;
    u32 mattype = TOPMAT(mat).type;
    if (dir != MB_DIRECTION_UP) {
        mattype = MATERIAL(mat).type;
        if (mattype < MAT_CUTOUT) {
            if (TOPMAT(mat).type == MAT_CUTOUT) return CLASS_HOLLOW_CUTOUT;
            if (TOPMAT(mat).type > MAT_CUTOUT) return CLASS_HOLLOW_TRANSPARENT;
        }
    }
    if (mattype < MAT_CUTOUT) return CLASS_OPAQUE;
    if (mattype == MAT_CUTOUT) return CLASS_CUTOUT;
    return CLASS_TRANSPARENT;
}

// Used to compare material types and see if any future culling checks can be skipped
// WARNING! using grid tile ->mat directly will result in mat being 0 if the tile is empty/water
// Use get_mat for curMat (-1 = non-material tile)
// If this returns FALSE then the material can be culled.
s32 cutout_skip_culling_check(s32 curMat, s32 otherMat, s32 direction) {
    s32 curMatClass, otherMatClass;
    if (curMat == otherMat) return FALSE;
    curMatClass = get_side_class(curMat, direction);
    otherMatClass = get_side_class(otherMat, direction^1);
    if (curMatClass == otherMatClass) {
        return (curMatClass == CLASS_TRANSPARENT);
    } else {
        if ((curMatClass == CLASS_HOLLOW_CUTOUT) || (curMatClass == CLASS_HOLLOW_TRANSPARENT)) {
            if (direction == MB_DIRECTION_DOWN) {
                if (get_side_class(otherMat, MB_DIRECTION_DOWN) == curMatClass) return FALSE;
            }
        }
        return (curMatClass < otherMatClass);
    }
}

// Used for determining if a water side should be culled.
u32 block_side_is_solid(s32 adjMat, s32 mat, s32 direction) {
    s32 adjMatClass = get_side_class(adjMat, direction^1);
    if (adjMatClass == CLASS_CUTOUT) return FALSE;
    if (adjMatClass != CLASS_HOLLOW_CUTOUT) return TRUE; // cannot be waterlogged
    // Now we know that the side is a hollow cutout
    s32 matClass = get_side_class(mat, MB_DIRECTION_DOWN);
    return (matClass != CLASS_HOLLOW_CUTOUT);
    // Will return FALSE if both materials are hollow cutouts but the waterlogged block
    // isn't a full faceshape. Not a big deal
}

s32 should_cull(s8 pos[3], s32 direction, s32 faceshape, s32 rot) {
    if (faceshape & MB_FACESHAPE_EMPTY) return FALSE;
    if (mb_render_culling_off) return FALSE;
    direction = rotate_direction(direction, rot);

    s8 adjPos[3];
    vec3_sum(adjPos, pos, cullOffsetLUT[direction]);

    if (AT_CEILING(pos[1]) && (direction == MB_DIRECTION_UP)) return TRUE;

    if (!coords_in_range(adjPos)) {
        if (direction == MB_DIRECTION_UP) return FALSE;
        if (direction == MB_DIRECTION_DOWN) {
            return (mb_curr_boundary & MB_BOUNDARY_INNER_FLOOR) != 0;
        }
        return ((mb_curr_boundary & MB_BOUNDARY_INNER_WALLS) && (pos[1] < mb_lopt_boundary_height));
    }
    struct MBGridObject *adjTile = get_grid_tile(adjPos);
    s32 tileType = adjTile->type;
    switch(tileType) {
        case TILE_TYPE_CULL:
            return TRUE;
        case TILE_TYPE_TROLL:
            if (mb_build_collision_type != 0) return FALSE;
            break;
    }

    if (cutout_skip_culling_check(get_mat(pos), adjTile->mat, direction)) return FALSE;
    s32 otherFaceshape = get_faceshape(adjPos, direction);

    if (otherFaceshape & MB_FACESHAPE_EMPTY) return FALSE;
    if (otherFaceshape == MB_FACESHAPE_FULL) return TRUE;
    if (faceshape == MB_FACESHAPE_FULL) return FALSE;
    if ((faceshape == MB_FACESHAPE_TOPTRI) || (faceshape == MB_FACESHAPE_TOPHALF)) {
        if (otherFaceshape == faceshape) {
            u8 otherrot = adjTile->rot;
            return (otherrot == rot);
        } else return FALSE;
    }
    if (faceshape == MB_FACESHAPE_BOTTOMSLAB || faceshape == MB_FACESHAPE_TOPSLAB || faceshape == MB_FACESHAPE_POLETOP) {
        if (otherFaceshape == faceshape) return TRUE;
    }
    if (faceshape == (otherFaceshape^1)) return TRUE;
    // Slab priority lists
    if (((faceshape & 0x10) && (otherFaceshape & 0x10)) ||
        ((faceshape & 0x20) && (otherFaceshape & 0x20))) return (faceshape > otherFaceshape);
    return FALSE;
}

// Additional culling check for certain grass overhangs. Assumes should_cull has already failed.
s32 should_cull_topslab_check(s8 pos[3], s32 direction) {
    s8 adjPos[3];
    vec3_sum(adjPos, pos, cullOffsetLUT[direction]);
    if (!coords_in_range(adjPos)) return FALSE;

    if (cutout_skip_culling_check(get_mat(pos), get_mat(adjPos), direction)) return FALSE;
    s32 otherFaceshape = get_faceshape(adjPos, direction);
    if ((otherFaceshape >= MB_FACESHAPE_DOWNUPPERGENTLE_1) && (otherFaceshape <= MB_FACESHAPE_TOPSLAB)) return TRUE;
    return FALSE;
}

void cache_tri(u8 v1, u8 v2, u8 v3) {
    mb_cached_tris[mb_num_tris_cached][0] = v1;
    mb_cached_tris[mb_num_tris_cached][1] = v2;
    mb_cached_tris[mb_num_tris_cached][2] = v3;
    mb_num_tris_cached++;
}

void display_cached_tris(void) {
    if (mb_num_vertices_cached == 0) return;
    gSPVertex(&mb_curr_gfx[mb_gfx_index++], mb_curr_vtx, mb_num_vertices_cached, 0);
    u32 i = 0;
    while (i < mb_num_tris_cached) {
        // odd number of triangles and last one left
        if ((mb_num_tris_cached - i) == 1) {
            gSP1Triangle(&mb_curr_gfx[mb_gfx_index++], mb_cached_tris[i][0], mb_cached_tris[i][1], mb_cached_tris[i][2], 0);
            i++;
        } else {
            gSP2Triangles(&mb_curr_gfx[mb_gfx_index++], mb_cached_tris[i][0], mb_cached_tris[i][1], mb_cached_tris[i][2], 0, mb_cached_tris[i+1][0], mb_cached_tris[i+1][1], mb_cached_tris[i+1][2], 0);
            i+=2;
        }
    }
    mb_curr_vtx += mb_num_vertices_cached;
    mb_num_tris_cached = 0;
    mb_num_vertices_cached = 0;
}

void check_cached_tris(void) {
    if (mb_num_vertices_cached > 28 || mb_num_tris_cached > 62) {
        display_cached_tris();
    }
}

void rotate_obj_toward_trajectory_angle(struct Object * obj, u32 traj_id) {
    if ((mb_trajectory_list[traj_id][0][0] == -1)||(mb_trajectory_list[traj_id][1][0] == -1)) return;

    s16 angle_to_trajectory;
    if (!trajectory_get_target_angle(&angle_to_trajectory, &mb_trajectory_list[traj_id][0], &mb_trajectory_list[traj_id][1])) {
        return;
    }

    if ( obj_has_model(obj ,MODEL_CHECKERBOARD_PLATFORM) ) {
        angle_to_trajectory += 0x4000;
    }

    obj->oFaceAngleYaw = angle_to_trajectory;
    obj->oMoveAngleYaw = angle_to_trajectory;
}

void draw_dotted_line(s16 pos1[3], s16 pos2[3]) {
    f32 dx2 = sqr(pos2[0] - pos1[0]);
    f32 dy2 = sqr(pos2[1] - pos1[1]);
    f32 dz2 = sqr(pos2[2] - pos1[2]);
    f32 length = sqrtf(dx2 + dy2 + dz2);

    s16 yaw = atan2s(pos2[2] - pos1[2], pos2[0] - pos1[0]);
    s16 pitch = atan2s(sqrtf(dx2 + dz2), pos2[1] - pos1[1]);
    
    f32 sy = 10*sins(yaw);
    f32 cy = 10*coss(yaw);
    f32 sp = 10*sins(pitch);
    f32 cp = 10*coss(pitch);
    f32 spsy = sp*sins(yaw);
    f32 spcy = sp*coss(yaw);

    make_vertex(mb_curr_vtx, mb_num_vertices_cached,     pos1[0] + cy, pos1[1], pos1[2] - sy, 0, 0, 0, 0, 0, 0xFF);
    make_vertex(mb_curr_vtx, mb_num_vertices_cached + 1, pos1[0] - cy, pos1[1], pos1[2] + sy, 0, 0, 0, 0, 0, 0xFF);
    make_vertex(mb_curr_vtx, mb_num_vertices_cached + 2, pos2[0] + cy, pos2[1], pos2[2] - sy, 0, length, 0, 0, 0, 0xFF);
    make_vertex(mb_curr_vtx, mb_num_vertices_cached + 3, pos2[0] - cy, pos2[1], pos2[2] + sy, 0, length, 0, 0, 0, 0xFF);

    cache_tri(mb_num_vertices_cached, mb_num_vertices_cached+1, mb_num_vertices_cached+2);
    cache_tri(mb_num_vertices_cached+1, mb_num_vertices_cached+3, mb_num_vertices_cached+2);
    mb_num_vertices_cached += 4;
    check_cached_tris();

    make_vertex(mb_curr_vtx, mb_num_vertices_cached,     pos1[0] + spsy, pos1[1] - cp, pos1[2] + spcy, 0, 0, 0, 0, 0, 0xFF);
    make_vertex(mb_curr_vtx, mb_num_vertices_cached + 1, pos1[0] - spsy, pos1[1] + cp, pos1[2] - spcy, 0, 0, 0, 0, 0, 0xFF);
    make_vertex(mb_curr_vtx, mb_num_vertices_cached + 2, pos2[0] + spsy, pos2[1] - cp, pos2[2] + spcy, 0, length, 0, 0, 0, 0xFF);
    make_vertex(mb_curr_vtx, mb_num_vertices_cached + 3, pos2[0] - spsy, pos2[1] + cp, pos2[2] - spcy, 0, length, 0, 0, 0, 0xFF);

    cache_tri(mb_num_vertices_cached, mb_num_vertices_cached+1, mb_num_vertices_cached+2);
    cache_tri(mb_num_vertices_cached+1, mb_num_vertices_cached+3, mb_num_vertices_cached+2);
    mb_num_vertices_cached += 4;
    check_cached_tris();
}

void generate_trajectory_gfx(void) {
    mb_curr_gfx = mb_trajectory_gfx;
    mb_curr_vtx = mb_trajectory_vtx;
    mb_gfx_index = 0;
    gSPDisplayList(&mb_curr_gfx[mb_gfx_index++], mat_maker_MakerLineMat_layer1);

    for (s32 traj = 0; traj < mb_trajectories_used; traj++) {
        Trajectory (*curr_trajectory)[4] = mb_trajectory_list[traj];
        if (curr_trajectory[0][0] == -1) continue;
        s16 pos1[3], pos2[3];
        s32 isLoop = FALSE;

        // Find object corresponding to this trajectory
        for (s32 i = 0; i < mb_object_count; i++) {
            if (mb_object_type_list[mb_object_data[i].type].flags & OBJ_TYPE_TRAJECTORY) {
                if (mb_object_data[i].bparam == traj) {
                    // Check if it loops or not
                    if (mb_object_data[i].type == OBJECT_TYPE_PLATFORM_LOOPING) {
                        isLoop = TRUE;
                        gDPSetPrimColor(&mb_curr_gfx[mb_gfx_index++], 0, 0, 0, 0, 255, 255);
                    } else {
                        gDPSetPrimColor(&mb_curr_gfx[mb_gfx_index++], 0, 0, 255, 255, 0, 255);
                    }
                    break;
                }
            }
        }
        s32 i;
        for (i = 0; curr_trajectory[i+1][0] == i+1; i++) {
            vec3_set(pos1, curr_trajectory[i][1], curr_trajectory[i][2], curr_trajectory[i][3]);
            vec3_set(pos2, curr_trajectory[i+1][1], curr_trajectory[i+1][2], curr_trajectory[i+1][3]);
            draw_dotted_line(pos1, pos2);
        }
        if (traj == mb_trajectory_to_edit && gMenuState == MB_MAKE_TRAJECTORY) {
            vec3_set(pos1, curr_trajectory[i][1], curr_trajectory[i][2], curr_trajectory[i][3]);
            vec3_set(pos2, GRID_TO_POS(mb_cursor_pos[0]), GRID_TO_POS(mb_cursor_pos[1]), GRID_TO_POS(mb_cursor_pos[2]));
            draw_dotted_line(pos1, pos2);
        }
        if (isLoop) {
            vec3_set(pos1, curr_trajectory[0][1], curr_trajectory[0][2], curr_trajectory[0][3]);
            draw_dotted_line(pos2, pos1);
        }
        display_cached_tris();
    }

    gSPDisplayList(&mb_curr_gfx[mb_gfx_index++], mat_revert_maker_MakerLineMat_layer1);
    gSPEndDisplayList(&mb_curr_gfx[mb_gfx_index]);
}

s32 find_duplicate_vertex(s32 vx, s32 vy, s32 vz, u8 n0, u8 n1, u8 n2, s16 u, s16 v) {
    for (int i = 0; i < mb_num_vertices_cached; i++) {
        if (mb_curr_vtx[i].v.ob[0] == vx && mb_curr_vtx[i].v.ob[1] == vy && mb_curr_vtx[i].v.ob[2] == vz) {
            if (mb_curr_vtx[i].v.tc[0] == u && mb_curr_vtx[i].v.tc[1] == v) {
                if (mb_curr_vtx[i].v.cn[0] == n0 && mb_curr_vtx[i].v.cn[1] == n1 && mb_curr_vtx[i].v.cn[2] == n2) {
                    return i;
                }
            }
        }
    }
    return -1;
}

void mb_transform_vtx_with_rot(s8 v[][3], s8 oldv[][3], u32 rot) {
    for (u32 i = 0; i < mb_curr_poly_vert_count; i++) {
        s8 x = oldv[i][0];
        s8 z = oldv[i][2];
        switch (rot) {
            case 0: v[i][0] = x; v[i][2] = z; break;
            case 1: v[i][0] = z; v[i][2] = 16-x; break;
            case 2: v[i][0] = 16-x; v[i][2] = 16-z; break;
            case 3: v[i][0] = 16-z; v[i][2] = x; break;
        }
        v[i][1] = oldv[i][1];
    }
}

s32 render_get_normal_and_uvs(s8 v[3][3], u32 direction, u32 rot, u8 *uAxis, u8 *vAxis, s8 n[3]) {
    // Find normal
    Vec3f normal;
    if (direction != MB_DIRECTION_UP) {
        find_vector_perpendicular_to_plane(normal, v[0], v[1], v[2]);
        vec3_normalize(normal);
        n[0] = normal[0] * 0x7F;
        n[1] = normal[1] * 0x7F;
        n[2] = normal[2] * 0x7F;
    } else {
        n[0] = 0;
        n[1] = 0x7F;
        n[2] = 0;
    }
    // Find UVs
    direction = rotate_direction(direction, rot);
    switch (direction) {
        case MB_DIRECTION_NEG_X: *uAxis = 2; *vAxis = 1; return TRUE;
        case MB_DIRECTION_POS_X: *uAxis = 2; *vAxis = 1; return FALSE;
        case MB_DIRECTION_DOWN: // fallthrough
        case MB_DIRECTION_UP: *uAxis = 2; *vAxis = 0; return FALSE;
        case MB_DIRECTION_NEG_Z: *uAxis = 0; *vAxis = 1; return FALSE;
        /*case MB_DIRECTION_POS_Z:*/ default: *uAxis = 0; *vAxis = 1; return TRUE;
    }
}

void render_poly(struct MBTerrainPoly *poly, s8 pos[3], u32 rot) {
    s8 n[3];
    u8 uAxis, vAxis;
    s8 newVtx[4][3];

    s8 vertexIndices[4];
    s32 numNewVertices = 0;

    mb_transform_vtx_with_rot(newVtx, poly->vtx, rot);
    s32 facedir = poly->faceDir;
    s32 useAltUVs = mb_use_alt_uvs;
    if (mb_render_vertical && (poly->faceshape > MB_FACESHAPE_EMPTY)) {
        facedir = (poly->faceshape - MB_FACESHAPE_EMPTY) + 1;
        // The four fixed UV shapes are 0x41 through 0x44
        // This maps them to 2 through 5 (the four lateral directions)
        // 0x40 is mapped to Down which results in no rotation
        useAltUVs = TRUE; // should not coincide with a decal UV
    }
    s32 flipU = render_get_normal_and_uvs(newVtx, facedir, rot, &uAxis, &vAxis, n);
    s32 clampV = (mb_growth_render_type == 2) || (mb_growth_render_type == 3);

    for (u32 i = 0; i < mb_curr_poly_vert_count; i++) {
        s32 x = GRID_TO_POS(pos[0]) + ((newVtx[i][0] - 8) * 16);
        s32 y = GRID_TO_POS(pos[1]) + ((newVtx[i][1] - 8) * 16);
        s32 z = GRID_TO_POS(pos[2]) + ((newVtx[i][2] - 8) * 16);

        s16 u, v;
        if (useAltUVs && poly->altuvs) {
            u = 16 - (*poly->altuvs)[i][0];
            v = 16 - (*poly->altuvs)[i][1];
        } else {
            u = newVtx[i][uAxis];
            if (!flipU) u = 16 - u;
            v = 16 - newVtx[i][vAxis];
        }

        s32 upos = (flipU ? 64-pos[uAxis] : pos[uAxis]);
        s32 vpos = pos[vAxis];
        if (mb_growth_render_type == 3) {
            upos *= 2; // fences
        } else if (mb_growth_render_type == 4) {
            vpos *= 2; // poles
        }

        upos = (upos % 48) - 24;
        vpos = (vpos % 48) - 24;

        u -= upos * 16;
        if (!clampV) v -= vpos * 16;

        u = (u * 64 + mb_uv_offset);
        v = (v * 64 + mb_uv_offset);

        s32 vert = find_duplicate_vertex(x, y, z, n[0], n[1], n[2], u, v);
        if (vert != -1) {
            vertexIndices[i] = vert;
            continue;
        }

        make_vertex(mb_curr_vtx, mb_num_vertices_cached + numNewVertices, x, y, z,
            u, v,
            n[0], n[1], n[2], 0xFF);
        vertexIndices[i] = mb_num_vertices_cached + numNewVertices;
        numNewVertices++;
    }
    
    u32 isQuad = (mb_curr_poly_vert_count == 4);
    if (mb_render_flip_normals) {
        cache_tri(vertexIndices[0], vertexIndices[2], vertexIndices[1]);
        if (isQuad) cache_tri(vertexIndices[1], vertexIndices[2], vertexIndices[3]);
    } else {
        cache_tri(vertexIndices[0], vertexIndices[1], vertexIndices[2]);
        if (isQuad) cache_tri(vertexIndices[1], vertexIndices[3], vertexIndices[2]);
    }
    mb_num_vertices_cached += numNewVertices;
    check_cached_tris();
}

// Create new static surface
extern struct Surface *alloc_surface(u32 dynamic);
extern void add_surface(struct Surface *surface, s32 dynamic);
void mb_create_surface(TerrainData v1[3], TerrainData v2[3], TerrainData v3[3], u32 isStatic) { 
    struct Surface *surface = alloc_surface(!isStatic);

    vec3_copy(surface->vertex1, v1);
    vec3_copy(surface->vertex2, v2);
    vec3_copy(surface->vertex3, v3);

    s16 min,max;
    min_max_3s(v1[1], v2[1], v3[1], &min, &max);
    surface->lowerY = (min - SURFACE_VERTICAL_BUFFER);
    surface->upperY = (max + SURFACE_VERTICAL_BUFFER);

    surface->type = mb_curr_coltype;
    if (isStatic)
        add_surface(surface, FALSE);
    else
        add_surface_to_cell(2, 0, 0, surface); // 2 = block surface, cell parameters unused
};

TerrainData colVtxs[4][3];

void mb_create_poly(struct MBTerrainPoly *poly, s8 pos[3], u32 rot) {
    switch (mb_build_collision_type) {
        case 1: // floors
            if (poly->faceDir != MB_DIRECTION_UP) return;
            break;
        case 2: // ceils
            if (poly->faceDir != MB_DIRECTION_DOWN) return;
            break;
        case 3: // walls
            if ((poly->faceDir == MB_DIRECTION_DOWN) || (poly->faceDir == MB_DIRECTION_UP)) return;
            break;
    }
    s8 newVtx[4][3];
    mb_transform_vtx_with_rot(newVtx, poly->vtx, rot);
    for (u32 k = 0; k < mb_curr_poly_vert_count; k++) {
        colVtxs[k][0] = GRID_TO_POS(pos[0]) + ((newVtx[k][0] - 8) * (TILE_SIZE/16)),
        colVtxs[k][1] = GRID_TO_POS(pos[1]) + ((newVtx[k][1] - 8) * (TILE_SIZE/16)),
        colVtxs[k][2] = GRID_TO_POS(pos[2]) + ((newVtx[k][2] - 8) * (TILE_SIZE/16));
    }
    mb_create_surface(colVtxs[0], colVtxs[1], colVtxs[2], FALSE);
    if (mb_curr_poly_vert_count == 4) {
        mb_create_surface(colVtxs[1], colVtxs[3], colVtxs[2], FALSE);
    }
}

void process_poly(s8 pos[3], struct MBTerrainPoly *quad, u32 rot) {
    if (!mb_build_collision_type)
        render_poly(quad, pos, rot);
    else
        mb_create_poly(quad, pos, rot);
}

void grass_slope_extra_decal_uvs(s8 newUVs[][2], s8 vtx[][3], s32 side, s32 scalefactor, s32 count) {
    for (s32 i = 0; i < count; i++) {
        newUVs[i][0] = 16 - vtx[i][0];
        s32 upFactor = (side == 0 ? vtx[i][0] : 16 - vtx[i][0]);
        newUVs[i][1] = vtx[i][1] - ((upFactor*scalefactor) >> 1); // multiply by scalefactor/2
    }
}

void render_grass_slope_extra_decal(s8 pos[3], u32 direction, u32 grassType) {
    // Check if below block is in range
    s8 newpos[3];
    s8 newUVs[4][2];
    vec3_set(newpos, pos[0], pos[1]-1, pos[2]);
    if (!coords_in_range(newpos)) return;

    // Check if below block matches material
    u8 curMat = get_mat(pos);
    u8 belowMat = get_mat(newpos);
    if (curMat != belowMat) return;

    // Check if below block is right shape and culled
    // Shape of face of above block on same side
    s32 otherFaceshape = get_faceshape(newpos, direction^1);
    if (should_cull(newpos, direction, otherFaceshape, 0)) return;
    if (should_cull_topslab_check(newpos, direction)) return;
    
    // Calculate effective rotation of face to print. very ugly
    u8 targetRot = 0;
    switch (direction) {
        case MB_DIRECTION_POS_X:
            targetRot = 1;
            break;
        case MB_DIRECTION_NEG_X:
            targetRot = 3;
            break;
        //case MB_DIRECTION_POS_Z:
            //targetRot = 0;
            //break;
        case MB_DIRECTION_NEG_Z:
            targetRot = 2;
            break;
    }

    s32 index;
    s32 oldVerts = mb_curr_poly_vert_count;

    switch (otherFaceshape) {
        default:
            return;
        // Face is full quad
        case MB_FACESHAPE_FULL:
            index = 0; mb_curr_poly_vert_count = 4;
            break;
        case MB_FACESHAPE_DOWNTRI_1:
            index = 1; mb_curr_poly_vert_count = 3;
            break;
        case MB_FACESHAPE_DOWNTRI_2:
            index = 2; mb_curr_poly_vert_count = 3;
            break;
        case MB_FACESHAPE_DOWNUPPERGENTLE_1:
        case MB_FACESHAPE_DOWNUPPERGENTLE_2:
        case MB_FACESHAPE_TOPSLAB:
            index = 3; mb_curr_poly_vert_count = 4;
            break;
        case MB_FACESHAPE_HALFSIDE_1:
            if (grassType == MB_GROWTH_SLOPE_SIDE_L) return;
            index = 4; mb_curr_poly_vert_count = 4;
            break;
        case MB_FACESHAPE_HALFSIDE_2:
            if (grassType == MB_GROWTH_SLOPE_SIDE_R) return;
            index = 5; mb_curr_poly_vert_count = 4;
            break;
        case MB_FACESHAPE_DOWNLOWERGENTLE_1:
            index = 6; mb_curr_poly_vert_count = 3;
            break;
        case MB_FACESHAPE_DOWNLOWERGENTLE_2:
            index = 7; mb_curr_poly_vert_count = 3;
            break;
    }

    s32 side = grassType & 1;
    s32 scalefactor = (grassType & 2) ? 1 : 2;

    struct MBTerrainPoly *poly = slope_decal_below_surfs[index];
    grass_slope_extra_decal_uvs(newUVs, poly->vtx, side, scalefactor, mb_curr_poly_vert_count);
    poly->altuvs = newUVs;
    render_poly(poly, newpos, targetRot);
    mb_curr_poly_vert_count = oldVerts;

    return;
}

// Determines if the faceshape as a solid bottom edge and two vertical edges on either side
u32 faceshape_has_full_bottom(u32 faceshape) {
    return (faceshape == MB_FACESHAPE_FULL) ||
        (faceshape >= MB_FACESHAPE_UPPERGENTLE_1 && faceshape <= MB_FACESHAPE_BOTTOMSLAB);
}

u32 should_render_grass_side(s8 pos[3], u32 direction, u32 faceshape, u32 rot, u32 grassType) {
    s8 abovePos[3];
    vec3_set(abovePos, pos[0], pos[1]+1, pos[2]);
    struct MBGridObject *tile = get_grid_tile(pos);
    struct MBGridObject *aboveTile = get_grid_tile(abovePos);
    if (should_cull(pos, direction, faceshape, rot)) return FALSE;

    // Other sides that don't care about the above block (e.g. bottom slab)
    if (grassType == MB_GROWTH_UNCONDITIONAL) return TRUE;

    // Render extra decal for slopes if necessary
    // The side of the slope itself is unconditional and will always render
    if (grassType >= MB_GROWTH_EXTRADECAL_START) {
        render_grass_slope_extra_decal(pos, rotate_direction(direction, rot), grassType);
        return TRUE;
    }

    if (AT_CEILING(pos[1])) return FALSE;
    if (!coords_in_range(abovePos)) return TRUE;

    if (!(faceshape & MB_FACESHAPE_EMPTY)) {
        if ((grassType == MB_GROWTH_HALF_SIDE) || (grassType == MB_GROWTH_NORMAL_SIDE)) {
            if (should_cull_topslab_check(pos, rotate_direction(direction, rot))) return FALSE;
        }
    }

    // Render if above tile is empty
    if (aboveTile->type == TILE_TYPE_EMPTY) return TRUE;
    // Render if above tile is seethrough (some exceptions)
    if (cutout_skip_culling_check(tile->mat, aboveTile->mat, MB_DIRECTION_UP)) return TRUE;

    if (faceshape > MB_FACESHAPE_EMPTY) {
        direction = (faceshape - MB_FACESHAPE_EMPTY) + 1;
    }

    s32 otherFaceshape;
    switch (grassType) {
        case MB_GROWTH_NORMAL_SIDE:
        case MB_GROWTH_HALF_SIDE:
            // Shape of face of above block on same side
            otherFaceshape = get_faceshape(abovePos, rotate_direction(direction, rot)^1);
            switch (otherFaceshape) {
                case MB_FACESHAPE_TRI_1:
                case MB_FACESHAPE_TRI_2:
                case MB_FACESHAPE_FULL:
                case MB_FACESHAPE_UPPERGENTLE_1:
                case MB_FACESHAPE_UPPERGENTLE_2:
                case MB_FACESHAPE_LOWERGENTLE_1:
                case MB_FACESHAPE_LOWERGENTLE_2:
                case MB_FACESHAPE_BOTTOMSLAB:
                    return FALSE;
            }
            if ((grassType == MB_GROWTH_HALF_SIDE) && (faceshape == otherFaceshape)) {
                return FALSE;
            }
            return TRUE;
        case MB_GROWTH_UNDERSLOPE_CORNER:
            ;// some very cursed logic here, this is solely for upside-down inside corners
            u8 faceshape1 = get_faceshape(abovePos, rotate_direction(MB_DIRECTION_POS_Z, rot)^1);
            u8 faceshape2 = get_faceshape(abovePos, rotate_direction(MB_DIRECTION_POS_X, rot)^1);
            // this is basically just checking for if a normal slope corner is on top at the right angle
            if (faceshape1 == MB_FACESHAPE_TRI_1 && faceshape2 == MB_FACESHAPE_TRI_2)
                return FALSE;
            // don't display if either face on top of the inside corner is full
            return !(faceshape_has_full_bottom(faceshape1) || faceshape_has_full_bottom(faceshape2));

        case MB_GROWTH_DIAGONAL_SIDE:
        case MB_GROWTH_VSLAB_SIDE:
            otherFaceshape = get_faceshape(abovePos, MB_DIRECTION_UP);
            if (otherFaceshape == MB_FACESHAPE_FULL) return FALSE;
            if (otherFaceshape != (grassType == MB_GROWTH_DIAGONAL_SIDE ? MB_FACESHAPE_TOPTRI : MB_FACESHAPE_TOPHALF)) return TRUE;
            if (aboveTile->type == TILE_TYPE_SCORNER) return TRUE;
            u8 otherrot = aboveTile->rot;
            return (otherrot != rot);
    }
    return FALSE;
}

void process_poly_with_growth(s8 pos[3], struct MBTerrainPoly *poly, u32 rot) {
    switch (mb_growth_render_type) {
        case 1: // grass top
            if (poly->growthType != MB_GROWTH_FULL) return;
            if (should_cull(pos, poly->faceDir, poly->faceshape, rot)) return;
            break;
        case 2: // grass decal
            if (poly->growthType == MB_GROWTH_FULL || poly->growthType == MB_GROWTH_NONE) return;
            if (!should_render_grass_side(pos, poly->faceDir, poly->faceshape, rot, poly->growthType)) return;
            break;
        default:
            if (mb_curr_mat_has_topside && (poly->growthType == MB_GROWTH_FULL)) return;
            if (should_cull(pos, poly->faceDir, poly->faceshape, rot)) return;
            break;
    }
    process_poly(pos, poly, rot);
}

void process_tile(s8 pos[3], struct MBTerrain *terrain, u32 rot) {
    mb_curr_poly_vert_count = 4;
    for (u32 j = 0; j < terrain->numQuads; j++) {
        struct MBTerrainPoly *quad = &terrain->quads[j];
        process_poly_with_growth(pos, quad, rot);
    }
    mb_curr_poly_vert_count = 3;
    for (u32 j = 0; j < terrain->numTris; j++) {
        struct MBTerrainPoly *tri = &terrain->tris[j];
        process_poly_with_growth(pos, tri, rot);
    }
}

#define BAR_CONNECTED_SIDE(bar) ((bar) & 1)
#define BAR_CONNECTED_TOP(bar) (((bar) >> 1) & 0x1)
#define BAR_CONNECTED_BOTTOM(bar) (((bar) >> 2) & 0x1)

void check_bar_side_connections(s8 pos[3], u8 connections[4]) {
    s8 adjacentPos[3];

    for (u32 rot = 0; rot < 4; rot++) {
        connections[rot] = 0;
        s32 dir = rotate_direction(MB_DIRECTION_POS_Z, rot);
        vec3_sum(adjacentPos, pos, cullOffsetLUT[dir]);
        struct MBGridObject *adjTile = get_grid_tile(adjacentPos);

        if (!coords_in_range(adjacentPos)) {
            if ((mb_curr_boundary & MB_BOUNDARY_INNER_WALLS) && 
                adjacentPos[1] < mb_lopt_boundary_height) {
                connections[rot] = 1;
            }
            continue;
        }

        // If adjacent block is a bar, return true
        if (adjTile->type == TILE_TYPE_BARS) {
            connections[rot] = 1; continue;
        }

        // Else check its a full block
        if ((get_faceshape(adjacentPos, dir) == MB_FACESHAPE_FULL) || (adjTile->type == TILE_TYPE_CULL)) {
            connections[rot] = 1;
        }
    }
}

void check_bar_connections(s8 pos[3], u8 connections[5]) {
    check_bar_side_connections(pos, connections);
    connections[4] = 0;
    // Check top
    s8 adjacentPos[3];
    u8 adjacentConnections[4];

    for (u32 updown = 0; updown < 2; updown++) { // 0 = Up, 1 = Down
        vec3_sum(adjacentPos, pos, cullOffsetLUT[updown]);
        struct MBGridObject *adjTile = get_grid_tile(adjacentPos);

        if (coords_in_range(adjacentPos)) {
            u32 faceshape = get_faceshape(adjacentPos, updown);
            if ((faceshape == MB_FACESHAPE_FULL) || (adjTile->type == TILE_TYPE_CULL)) {
                for (u32 rot = 0; rot < 4; rot++) {
                    connections[rot] |= (1 << (updown+1)); // Apply top flag to all bars
                }
                connections[4] |= (1 << (updown + 1));
            } else if (faceshape == MB_FACESHAPE_TOPHALF) {
                connections[(adjTile->rot + 2) % 4] |= (1 << (updown + 1));
            } else if (adjTile->type == TILE_TYPE_BARS) {
                check_bar_side_connections(adjacentPos, adjacentConnections);
                for (u32 rot = 0; rot < 4; rot++) {
                    connections[rot] |= adjacentConnections[rot] << (updown+1); // Apply top flag to all bars
                }
                connections[4] |= (1 << (updown + 1));
            }
        }
    }
    if ((pos[1] == 0) && (mb_curr_boundary & MB_BOUNDARY_INNER_FLOOR)) { // Culling for bottom
        for (u32 rot = 0; rot < 5; rot++) {
            connections[rot] |= (1 << 2);
        }
    }
    if (AT_CEILING(pos[1])) { // Culling for top
        for (u32 rot = 0; rot < 5; rot++) {
            connections[rot] |= (1 << 1);
        }
    }
}

void render_bars_side(s8 pos[3], u8 connections[5]) {
    for (u32 rot = 0; rot < 4; rot++) {
        u32 leftRot = (rot + 3) % 4;
        u32 rightRot = (rot + 1) % 4;
        if (BAR_CONNECTED_SIDE(connections[rot])) {
            process_poly(pos, &mb_terrain_bars_connected_quads[0], rot);
            process_poly(pos, &mb_terrain_bars_connected_quads[1], rot);
        }
        if (!BAR_CONNECTED_SIDE(connections[rot]) ||
            (BAR_CONNECTED_SIDE(connections[leftRot]) && BAR_CONNECTED_SIDE(connections[rightRot]))) {
            process_poly(pos, mb_terrain_bars_unconnected_quad, rot);
        }
    }
}

void render_bars_top(s8 pos[3], u8 connections[5]) {
    for (u32 rot = 0; rot < 4; rot++) {
        if (BAR_CONNECTED_SIDE(connections[rot])) {
            if (!BAR_CONNECTED_TOP(connections[rot])) {
                process_poly(pos, &mb_terrain_bars_connected_quads[2], rot);
            }
            if (!BAR_CONNECTED_BOTTOM(connections[rot])) {
                process_poly(pos, &mb_terrain_bars_connected_quads[3], rot);
            }
        }
    }
    if (!BAR_CONNECTED_TOP(connections[4])) process_poly(pos, &mb_terrain_bars_center_quads[0], 0);
    if (!BAR_CONNECTED_BOTTOM(connections[4])) process_poly(pos, &mb_terrain_bars_center_quads[1], 0);
}

// Find if specific tile of water is fullblock or shallow
u32 is_water_fullblock(s8 pos[3]) {
    s8 abovePos[3];
    if (AT_CEILING(pos[1])) {
        if (MATERIAL(mb_lopt_boundary_mat).type != MAT_CUTOUT) return TRUE;
    }
    vec3_set(abovePos, pos[0], pos[1]+1, pos[2]);
    if (!coords_in_range(abovePos)) return FALSE;
    // Full block if above block is water
    struct MBGridObject *aboveTile = get_grid_tile(abovePos);
    struct MBGridObject *tile = get_grid_tile(pos);
    if (aboveTile->waterlogged) return TRUE;

    // Full block if above block has solid bottom face
    if (get_faceshape(abovePos, MB_DIRECTION_UP) == MB_FACESHAPE_FULL) {
        u8 aboveMat = get_mat(abovePos);
        // If above block is troll, but current block is also troll, then not full block
        if ((tile->type == TILE_TYPE_TROLL) && (aboveTile->type == TILE_TYPE_TROLL)) return FALSE;
        // If bottom block is hollow cutout and top block is some kind of cutout then not full block
        u8 curMat = get_mat(pos);
        if (!block_side_is_solid(aboveMat, curMat, MB_DIRECTION_UP)) return FALSE;
        return TRUE;
    }
    return FALSE;
}

// return type of render to use
// 0: cull
// 1: normal (low side)
// 2: full (full side)
// 3: top (thin top at side)
u32 get_water_side_render(s8 pos[3], u32 dir, u32 isFullblock) {
    s8 adjacentPos[3];
    vec3_sum(adjacentPos, pos, cullOffsetLUT[dir]);
    if (AT_CEILING(pos[1])) {
        if ((dir == MB_DIRECTION_UP) && (MATERIAL(mb_lopt_boundary_mat).type != MAT_CUTOUT)) return 0;
    }
    struct MBGridObject *adjTile = get_grid_tile(adjacentPos);

    if (!coords_in_range(adjacentPos)) {
        if (dir == MB_DIRECTION_UP) return 1;
        u32 type = isFullblock ? 2 : 1;
        if (dir == MB_DIRECTION_DOWN) {
            return (mb_curr_boundary & MB_BOUNDARY_INNER_FLOOR) ? 0 : type;
        }
        return ((mb_curr_boundary & MB_BOUNDARY_INNER_WALLS) && (pos[1] < mb_lopt_boundary_height)) ? 0 : type;
    }
    if (adjTile->type == TILE_TYPE_CULL) return 0;

    s32 mat = get_mat(pos);
    s32 adjMat = get_mat(adjacentPos);

    // Apply normal side culling
    // Usually this would fail if next to a mesh, but it will pass if
    // the current tile is the same material, which in this case will happen
    // if an entire mesh is waterlogged.
    if ((get_faceshape(adjacentPos, dir) == MB_FACESHAPE_FULL) && block_side_is_solid(adjMat, mat, dir)) return 0;

    // Check if the block that's waterlogged has a full face on the same side
    if ((get_faceshape(pos, dir^1) == MB_FACESHAPE_FULL) && block_side_is_solid(mat, adjMat, dir^1)) {
        if (!isFullblock && (dir == MB_DIRECTION_UP)) {
            return 1;
        }
        return 0;
    }

    if (adjTile->waterlogged) {
        // Check if this is a full block, next to a non-full block.
        // If so, use special thin side
        if (dir != MB_DIRECTION_UP && dir != MB_DIRECTION_DOWN) {
            if (isFullblock && !is_water_fullblock(adjacentPos)) {
                return 3;
            }
        }
        // Full block next to full block so cull
        return 0;
    }


    return isFullblock ? 2 : 1;
}

void render_water(s8 pos[3]) {
    u32 isFullblock = is_water_fullblock(pos);
    for (u32 j = 0; j < 6; j++) {
        u8 sideRender = get_water_side_render(pos, mb_terrain_fullblock_quads[j].faceDir, isFullblock);
        if (sideRender != 0) {
            struct MBTerrainPoly *poly = &mb_terrain_water_quadlists[sideRender - 1][j];
            process_poly(pos, poly, 0);
        }
    }
}

void set_render_mode(u32 tileType, u32 disableZ) {
    u32 rendermode = mb_render_mode_table[tileType];
    if (disableZ) rendermode &= ~(Z_UPD | Z_CMP);
    if (!gIsConsole && (tileType != MAT_TRANSPARENT)) rendermode |= AA_EN;
    gDPPipeSync(&mb_curr_gfx[mb_gfx_index++]);
    gDPSetRenderMode(&mb_curr_gfx[mb_gfx_index++], rendermode, 0);
}

enum tiletypeIndices {
    FENCE_TILETYPE_INDEX = NUM_MATERIALS_PER_THEME,
    POLE_TILETYPE_INDEX,
    BARS_TILETYPE_INDEX,
    CULL_TILETYPE_INDEX,
    WATER_TILETYPE_INDEX,
    END_TILE_INDEX
};

u32 get_tiletype_index(u32 type, u32 mat) {
    switch (type) {
        case TILE_TYPE_FENCE:
            return FENCE_TILETYPE_INDEX;
        case TILE_TYPE_POLE:
            return POLE_TILETYPE_INDEX;
        case TILE_TYPE_BARS:
            return BARS_TILETYPE_INDEX;
        case TILE_TYPE_WATER:
            return WATER_TILETYPE_INDEX;
        case TILE_TYPE_CULL:
            return CULL_TILETYPE_INDEX;
        default:
            if (mb_terrain_info_list[type].terrain) {
                return mat;
            }
    }
    return END_TILE_INDEX;
}

struct MBMaterialTop mb_topmat_table[] = {
    {MB_MAT_GRASS,         mat_maker_MakerGrassSide},
    {MB_MAT_SAND,          mat_maker_MakerSandSide},
    {MB_MAT_SNOW,          mat_maker_MakerSnowSide},
    {MB_MAT_TILES,         mat_maker_MakerTileEdge},
    {MB_MAT_C_STONETOP,    mat_maker_MakerCastleHexRim},
    {MB_MAT_HMC_GRASS,     mat_maker_MakerHGrassSide},
    {MB_MAT_BBH_METAL,     mat_maker_MakerBBHMetalSide},
    {MB_MAT_BBH_STONE,     mat_maker_MakerBBHMetalSide},
    {MB_MAT_JRB_TILETOP,   mat_maker_MakerJRBTileRim},
    {MB_MAT_SNOW_TILES,    mat_maker_MakerSnowTileRim},
    {MB_MAT_FLOWERS,       mat_maker_MakerFlowerEdge},
    {MB_MAT_RETRO_TREETOP, mat_maker_MakerRetroTreeSide},
    {MB_MAT_MC_GRASS,      mat_maker_MakerMCGrassEdge},
    {MB_MAT_LAVA_ROCKS,    mat_maker_MakerLavaRockEdge},
    {MB_MAT_DARK_GRASS,    mat_maker_MakerDarkGrassSide},
    {MB_MAT_CARTOON_GRASS, mat_maker_MakerCartoonGrassSide},
    {MB_MAT_ORANGE_GRASS,  mat_maker_MakerOrangeGrassSide},
    {MB_MAT_RED_GRASS,     mat_maker_MakerRedGrassSide},
    {MB_MAT_PURPLE_GRASS,  mat_maker_MakerPurpleGrassSide},
};

Gfx *get_sidetex(s32 matid) {
    for (s32 i = 0; i < ARRAY_COUNT(mb_topmat_table); i++) {
        if (mb_topmat_table[i].mat == matid) {
            return mb_topmat_table[i].decaltex;
        }
    }
    return NULL;
}


#define retroland_filter_on() if ((mb_lopt_theme == MB_THEME_RETRO) || (mb_lopt_theme == MB_THEME_MC)) { gDPSetTextureFilter(&mb_curr_gfx[mb_gfx_index++], G_TF_POINT); if (!gIsGliden) {mb_uv_offset = 0;} }
#define retroland_filter_off() if ((mb_lopt_theme == MB_THEME_RETRO) || (mb_lopt_theme == MB_THEME_MC)) { gDPSetTextureFilter(&mb_curr_gfx[mb_gfx_index++], G_TF_BILERP); mb_uv_offset = (mb_lopt_theme == MB_THEME_MC ? -32 : -16); }

// Returns true if tile should be processed
// If in vplex screen processing mode, can also override target mat type in order
// to render screen
u32 do_process(u8 *targetMatType, u32 processTileRenderMode) {
    switch (processTileRenderMode) {
        case PROCESS_TILE_NORMAL:
            return (*targetMatType != MAT_TRANSPARENT);
        case PROCESS_TILE_TRANSPARENT:
            return (*targetMatType == MAT_TRANSPARENT);
        case PROCESS_TILE_VPLEX:
            if (*targetMatType == MAT_DECAL) {
                *targetMatType = MAT_SCREEN;
                return TRUE;
            }
            return FALSE;
    }
    return TRUE; // PROCESS_TILE_BOTH
}

void process_tiles(u32 processTileRenderMode) {
    u32 startIndex, endIndex;
    u8 tileType, rot;
    s8 pos[3];

    // Poles
    u8 poleMatType = mb_mat_table[mb_theme_table[mb_lopt_theme].pole].type;
    if (do_process(&poleMatType, processTileRenderMode)) {
        mb_use_alt_uvs = TRUE;
        mb_growth_render_type = 4; // poles
        startIndex = mb_tile_data_indices[POLE_TILETYPE_INDEX];
        endIndex = mb_tile_data_indices[POLE_TILETYPE_INDEX+1];
        set_render_mode( poleMatType, FALSE);
        gSPDisplayList(&mb_curr_gfx[mb_gfx_index++], POLE_TEX());
        for (u32 i = startIndex; i < endIndex; i++) {
            s8 pos[3];
            vec3_set(pos, mb_tile_data[i].x, mb_tile_data[i].y, mb_tile_data[i].z);
            process_tile(pos, &mb_terrain_pole, mb_tile_data[i].rot);
        }
        display_cached_tris();
    }
    mb_use_alt_uvs = FALSE;
    mb_render_vertical = FALSE;

    for (u32 mat = 0; mat < NUM_MATERIALS_PER_THEME; mat++) {
        u8 matType = MATERIAL(mat).type;
        mb_growth_render_type = 0;
        mb_curr_mat_has_topside = HAS_TOPMAT(mat);
        startIndex = mb_tile_data_indices[mat];
        endIndex = mb_tile_data_indices[mat+1];

        if (!do_process(&matType, processTileRenderMode)) {
            goto skip_maintex;
        }

        set_render_mode( matType, FALSE);
        gSPDisplayList(&mb_curr_gfx[mb_gfx_index++], MATERIAL(mat).gfx);
        mb_render_vertical = MATERIAL(mat).vertical;

        // Important to not use matType here so that it's still opaque for screens
        if ((matType == MAT_CUTOUT) || ((MATERIAL(mat).type < MAT_CUTOUT) && (TOPMAT(mat).type >= MAT_CUTOUT))) {
            gSPClearGeometryMode(&mb_curr_gfx[mb_gfx_index++], G_CULL_BACK);
        }

        for (u32 i = startIndex; i < endIndex; i++) {
            tileType = mb_tile_data[i].type;
            rot = mb_tile_data[i].rot;
            vec3_set(pos, mb_tile_data[i].x, mb_tile_data[i].y, mb_tile_data[i].z);
            
            process_tile(pos, mb_terrain_info_list[tileType].terrain, rot);
        }

        display_cached_tris();
skip_maintex:
        
        if (mb_curr_mat_has_topside) {
            u8 topmatType = TOPMAT(mat).type;
            if (!do_process(&topmatType, processTileRenderMode)) continue;
            Gfx *sidetex = get_sidetex(TILE_MATDEF(mat).topmat);
            if (sidetex) {
                mb_use_alt_uvs = TRUE;
                mb_render_vertical = TRUE;
                gSPDisplayList(&mb_curr_gfx[mb_gfx_index++], sidetex);
                mb_growth_render_type = 2;

                // This code is pretty ugly. It renders the side decal in decal mode
                // for opaque parts of the texture, and in cutout mode for transparent parts.
                // Luckily this will only double the number of triangles in the rare case
                // that a custom block with a cutout side texture and a topmat with a side decal
                // is made (in order to cover both the opaque and alpha parts of the side tex.)
                // DECAL MODE
                if (matType != MAT_TRANSPARENT) { // Render in decal mode for cutouts, opaque and screen
                    set_render_mode( MAT_DECAL, FALSE);
                    for (u32 i = startIndex; i < endIndex; i++) {
                        tileType = mb_tile_data[i].type;
                        rot = mb_tile_data[i].rot;
                        vec3_set(pos, mb_tile_data[i].x, mb_tile_data[i].y, mb_tile_data[i].z);

                        process_tile(pos, mb_terrain_info_list[tileType].terrain, rot);
                    }
                    display_cached_tris();
                }
                // OPAQUE MODE
                if (matType >= MAT_CUTOUT) { // Render in cutout mode for cutouts and transparent
                    set_render_mode( MAT_CUTOUT, FALSE);
                    for (u32 i = startIndex; i < endIndex; i++) {
                        tileType = mb_tile_data[i].type;
                        rot = mb_tile_data[i].rot;
                        vec3_set(pos, mb_tile_data[i].x, mb_tile_data[i].y, mb_tile_data[i].z);

                        process_tile(pos, mb_terrain_info_list[tileType].terrain, rot);
                    }
                    display_cached_tris();
                }
                mb_use_alt_uvs = FALSE;
            }

            set_render_mode(topmatType, FALSE);
            gSPDisplayList(&mb_curr_gfx[mb_gfx_index++], TOPMAT(mat).gfx);
            mb_render_vertical = TOPMAT(mat).vertical;

            mb_growth_render_type = 1;
            for (u32 i = startIndex; i < endIndex; i++) {
                tileType = mb_tile_data[i].type;
                rot = mb_tile_data[i].rot;
                vec3_set(pos, mb_tile_data[i].x, mb_tile_data[i].y, mb_tile_data[i].z);

                process_tile(pos, mb_terrain_info_list[tileType].terrain, rot);
            }
            display_cached_tris();
        }
        gSPSetGeometryMode(&mb_curr_gfx[mb_gfx_index++], G_CULL_BACK);
    }
}

// Fade: 0 = no fade, 1 = fade at bottom, 2 = fade at top
void render_boundary_quad(struct MBBoundaryQuad *quad, s16 y, s16 yHeight, u32 fade) {
    // Get normal
    Vec3f normal;
    find_vector_perpendicular_to_plane(normal, quad->vtx[0], quad->vtx[1], quad->vtx[2]);
    vec3_normalize(normal);
    s8 n[3];
    s32 normalMult = (mb_render_flip_normals ? -0x7F : 0x7F);
    n[0] = normal[0]*normalMult;
    n[1] = normal[1]*normalMult;
    n[2] = normal[2]*normalMult;

    s32 uScale = quad->uYScale ? (yHeight * 64) : mb_grid_size*16;
    s32 vScale = quad->vYScale ? (yHeight * 64) : mb_grid_size*16;
    for (u32 i = 0; i < 4; i++) {
        s16 u = (quad->u[i & 1])*uScale + mb_uv_offset;
        s16 v = (quad->v[i >> 1])*vScale + mb_uv_offset;
        if (quad->flipUvs) { s16 tmp = u; u = v; v = tmp;}
        if (yHeight % 2) v += 512; // offset for odd number of blocks
        u8 alpha = 255;
        if ((ABS(quad->vtx[i][0]) > 32) || (ABS(quad->vtx[i][2]) > 32)) {
            alpha = 0;
        } else if (((fade == 1) && (quad->vtx[i][1] == 0)) || ((fade == 2) && (quad->vtx[i][1] == 1))) {
            alpha = 0;
        }
        make_vertex(mb_curr_vtx, i+mb_num_vertices_cached, quad->vtx[i][0]*mb_grid_size*4, (quad->vtx[i][1]*yHeight*TILE_SIZE + y), quad->vtx[i][2]*mb_grid_size*4,
            u, v, n[0], n[1], n[2], alpha);
    }
    if (mb_render_flip_normals) {
        cache_tri(mb_num_vertices_cached, mb_num_vertices_cached+2, mb_num_vertices_cached+1);
        cache_tri(mb_num_vertices_cached+1, mb_num_vertices_cached+2, mb_num_vertices_cached+3);
    } else {
        cache_tri(mb_num_vertices_cached, mb_num_vertices_cached+1,mb_num_vertices_cached+2);
        cache_tri(mb_num_vertices_cached+1, mb_num_vertices_cached+3, mb_num_vertices_cached+2);
    }
    mb_num_vertices_cached += 4;
    check_cached_tris();
}

// Takes a bottom and top in number of blocks
void render_boundary(struct MBBoundaryQuad *quadList, u32 count, s16 yBottom, s16 yTop, u32 fade) {
    for (u32 i = 0; i < count; i++) {
        render_boundary_quad(&quadList[i], yBottom * TILE_SIZE, yTop - yBottom, fade);
    }
    display_cached_tris();
}

// Takes a height in SM64 units, no verticality
void render_boundary_precise(struct MBBoundaryQuad *quadList, u32 count, s16 yBottom, u32 fade) {
    for (u32 i = 0; i < count; i++) {
        render_boundary_quad(&quadList[i], yBottom, 0, fade);
    }
    display_cached_tris();
}

struct MBBoundaryQuad floor_boundary[] = {
    {{{32, 0, 32}, {32, 0, 0}, {0, 0, 32}, {0, 0, 0}}, {-16, 16}, {-16, 16}, FALSE, FALSE, FALSE},
    {{{0, 0, 32}, {0, 0, 0}, {-32, 0, 32}, {-32, 0, 0}}, {-16, 16}, {-16, 16}, FALSE, FALSE, FALSE},
    {{{32, 0, 0}, {32, 0, -32}, {0, 0, 0}, {0, 0, -32}}, {-16, 16}, {-16, 16}, FALSE, FALSE, FALSE},
    {{{0, 0, 0}, {0, 0, -32}, {-32, 0, 0}, {-32, 0, -32}}, {-16, 16}, {-16, 16}, FALSE, FALSE, FALSE},
};

struct MBBoundaryQuad floor_edge_boundary[] = {
    {{{48, 0, 32}, {48, 0, 0}, {32, 0, 32}, {32, 0, 0}}, {-16, 16}, {-8, 8}, FALSE, FALSE, FALSE},
    {{{48, 0, 32}, {32, 0, 32}, {48, 0, 48}, {32, 0, 48}}, {-8, 8}, {8, -8}, FALSE, FALSE, TRUE},
    {{{32, 0, 48}, {32, 0, 32}, {0, 0, 48}, {0, 0, 32}}, {-8, 8}, {-16, 16}, FALSE, FALSE, FALSE},

    {{{-32, 0, 32}, {-32, 0, 0}, {-48, 0, 32}, {-48, 0, 0}}, {-16, 16}, {-8, 8}, FALSE, FALSE, FALSE},
    {{{-32, 0, 48}, {-32, 0, 32}, {-48, 0, 48}, {-48, 0, 32}}, {-8, 8}, {-8, 8}, FALSE, FALSE, FALSE},
    {{{0, 0, 48}, {0, 0, 32}, {-32, 0, 48}, {-32, 0, 32}}, {-8, 8}, {-16, 16}, FALSE, FALSE, FALSE},

    {{{48, 0, 0}, {48, 0, -32}, {32, 0, 0}, {32, 0, -32}}, {-16, 16}, {-8, 8}, FALSE, FALSE, FALSE},
    {{{48, 0, -32}, {48, 0, -48}, {32, 0, -32}, {32, 0, -48}}, {-8, 8}, {-8, 8}, FALSE, FALSE, FALSE},
    {{{32, 0, -32}, {32, 0, -48}, {0, 0, -32}, {0, 0, -48}}, {-8, 8}, {-16, 16}, FALSE, FALSE, FALSE},

    {{{-32, 0, 0}, {-32, 0, -32}, {-48, 0, 0}, {-48, 0, -32}}, {-16, 16}, {-8, 8}, FALSE, FALSE, FALSE},
    {{{-32, 0, -48}, {-48, 0, -48}, {-32, 0, -32}, {-48, 0, -32}}, {-8, 8}, {8, -8}, FALSE, FALSE, TRUE},
    {{{0, 0, -32}, {0, 0, -48}, {-32, 0, -32}, {-32, 0, -48}}, {-8, 8}, {-16, 16}, FALSE, FALSE, FALSE},
};

struct MBBoundaryQuad wall_boundary[] = {
    {{{32, 1, 0}, {32, 1, -32}, {32, 0, 0}, {32, 0, -32}}, {16, -16}, {-8, 8}, FALSE, TRUE, FALSE},
    {{{32, 1, 32}, {32, 1, 0}, {32, 0, 32}, {32, 0, 0}}, {16, -16}, {-8, 8}, FALSE, TRUE, FALSE},

    {{{0, 1, 32}, {32, 1, 32}, {0, 0, 32}, {32, 0, 32}}, {16, -16}, {-8, 8}, FALSE, TRUE, FALSE},
    {{{-32, 1, 32}, {0, 1, 32}, {-32, 0, 32}, {0, 0, 32}}, {16, -16}, {-8, 8}, FALSE, TRUE, FALSE},

    {{{-32, 1, 0}, {-32, 1, 32}, {-32, 0, 0}, {-32, 0, 32}}, {16, -16}, {-8, 8}, FALSE, TRUE, FALSE},
    {{{-32, 1, -32}, {-32, 1, 0}, {-32, 0, -32}, {-32, 0, 0}}, {16, -16}, {-8, 8}, FALSE, TRUE, FALSE},

    {{{0, 1, -32}, {-32, 1, -32}, {0, 0, -32}, {-32, 0, -32}}, {16, -16}, {-8, 8}, FALSE, TRUE, FALSE},
    {{{32, 1, -32}, {0, 1, -32}, {32, 0, -32}, {0, 0, -32}}, {16, -16}, {-8, 8}, FALSE, TRUE, FALSE},
};

void render_boundary_decal_edge(Gfx *sidetex, s32 yBottom, u32 sideMatType) {
    if (sideMatType != MAT_TRANSPARENT) {
        set_render_mode( MAT_DECAL, FALSE);
        gSPDisplayList(&mb_curr_gfx[mb_gfx_index++], sidetex);
        render_boundary(wall_boundary, ARRAY_COUNT(wall_boundary), yBottom, yBottom+1, 0);
    }
    if (sideMatType >= MAT_CUTOUT) {
        set_render_mode( MAT_CUTOUT, FALSE);
        gSPDisplayList(&mb_curr_gfx[mb_gfx_index++], sidetex);
        render_boundary(wall_boundary, ARRAY_COUNT(wall_boundary), yBottom, yBottom+1, 0);
    }
}

void process_boundary(u32 processRenderMode) {
    u8 planeMat = mb_lopt_boundary_mat;
    struct MBMaterial *mat, *sidemat;
    mat = &TOPMAT(planeMat);
    sidemat = &MATERIAL(planeMat);

    // Outer walls (Plateau)
    if (mb_curr_boundary & MB_BOUNDARY_OUTER_WALLS) {
        u8 sidematType = sidemat->type;
        u8 showBackface = ((mat->type >= MAT_CUTOUT) && (sidemat->type <= MAT_CUTOUT)) || (sidemat->type == MAT_CUTOUT);
        mb_render_flip_normals = TRUE;

        if (do_process(&sidematType, processRenderMode)) {
            set_render_mode( sidematType, FALSE);
            gSPDisplayList(&mb_curr_gfx[mb_gfx_index++], sidemat->gfx);
            if (showBackface) {
                mb_render_flip_normals = FALSE;
                render_boundary(wall_boundary, ARRAY_COUNT(wall_boundary), -33, -32, 0);
                mb_render_flip_normals = TRUE;
            }
            render_boundary(wall_boundary, ARRAY_COUNT(wall_boundary), -33, -32, 0);

            Gfx *sidetex = get_sidetex(TILE_MATDEF(planeMat).topmat);
            if (sidetex && HAS_TOPMAT(planeMat)) {
                render_boundary_decal_edge(sidetex, -33, sidematType);
            }
        }
        // Fade at bottom
        gSPSetGeometryMode(&mb_curr_gfx[mb_gfx_index++], G_CULL_BACK);
        if (processRenderMode == PROCESS_TILE_TRANSPARENT) {
            gDPPipeSync(&mb_curr_gfx[mb_gfx_index++]);
            gDPSetRenderMode(&mb_curr_gfx[mb_gfx_index++], G_RM_AA_ZB_XLU_SURF, G_RM_AA_ZB_XLU_SURF2);
            gSPDisplayList(&mb_curr_gfx[mb_gfx_index++], sidemat->gfx);
            if (showBackface) {
                mb_render_flip_normals = FALSE;
                render_boundary(wall_boundary, ARRAY_COUNT(wall_boundary), -42, -33, 1);
                mb_render_flip_normals = TRUE;
            }
            render_boundary(wall_boundary, ARRAY_COUNT(wall_boundary), -42, -33, 1);
        }
        mb_render_flip_normals = FALSE;
    }

    // Main floor
    if (mb_curr_boundary & MB_BOUNDARY_INNER_FLOOR) {
        u8 matType = mat->type;
        if (do_process(&matType, processRenderMode)) {
            set_render_mode( matType, FALSE);
            gSPDisplayList(&mb_curr_gfx[mb_gfx_index++], mat->gfx);
            render_boundary(floor_boundary, ARRAY_COUNT(floor_boundary), -32, -32, 0);
        }
    }

    // Inner walls
    if ((mb_curr_boundary & MB_BOUNDARY_INNER_WALLS)) {
        u8 sidematType = sidemat->type;
        u8 showBackface = (sidemat->type == MAT_CUTOUT);
        u32 renderWalls = TRUE; // Whether to render main solid walls
        u32 renderFade = FALSE; // Whether to render fade at bottom
        s32 bottomY = -32;
        s32 topY = mb_lopt_boundary_height-32;
        if (showBackface) {
            gSPClearGeometryMode(&mb_curr_gfx[mb_gfx_index++], G_CULL_BACK);
        }
        
        if (!(mb_curr_boundary & MB_BOUNDARY_INNER_FLOOR)) {
            renderFade = TRUE;
            bottomY -= 1; // Make sure decal has a solid face to print on
        } else {
            renderWalls = (mb_lopt_boundary_height > 0);
        }

        Gfx *sidetex = get_sidetex(TILE_MATDEF(planeMat).topmat);
        u32 renderTopDecal = sidetex && HAS_TOPMAT(planeMat) && !(mb_curr_boundary & MB_BOUNDARY_CEILING);
        if (renderTopDecal) topY -= 1;

        if (renderWalls && do_process(&sidematType, processRenderMode)) {
            set_render_mode( sidematType, FALSE);
            gSPDisplayList(&mb_curr_gfx[mb_gfx_index++], sidemat->gfx);
            if (topY > bottomY + 32) {
                render_boundary(wall_boundary, ARRAY_COUNT(wall_boundary), bottomY, bottomY + 32, 0);
                render_boundary(wall_boundary, ARRAY_COUNT(wall_boundary), bottomY + 32, topY, 0);
            } else {
                render_boundary(wall_boundary, ARRAY_COUNT(wall_boundary), bottomY, topY, 0);
            }

            if (renderTopDecal) {
                // Render rim of regular material before decal
                render_boundary(wall_boundary, ARRAY_COUNT(wall_boundary), topY, topY + 1, 0);
                render_boundary_decal_edge(sidetex, topY, sidematType);
            }
        }
        gSPSetGeometryMode(&mb_curr_gfx[mb_gfx_index++], G_CULL_BACK);
        // Fade if no floor
        if (renderFade) {
            // Black floor to block out skybox
            if ((processRenderMode == PROCESS_TILE_NORMAL) && (mb_lopt_bg != 4) && (mb_lopt_bg != 9)) {
                gDPPipeSync(&mb_curr_gfx[mb_gfx_index++]);
                gDPSetRenderMode(&mb_curr_gfx[mb_gfx_index++], G_RM_AA_ZB_OPA_SURF, G_RM_AA_ZB_OPA_SURF2);
                gSPDisplayList(&mb_curr_gfx[mb_gfx_index++], &mat_maker_MakerBlack);
                render_boundary(floor_boundary, ARRAY_COUNT(floor_boundary), -40, -40, 0);
            } else if (processRenderMode == PROCESS_TILE_TRANSPARENT) {
                gDPPipeSync(&mb_curr_gfx[mb_gfx_index++]);
                gDPSetRenderMode(&mb_curr_gfx[mb_gfx_index++], G_RM_AA_ZB_XLU_SURF, G_RM_AA_ZB_XLU_SURF2);
repeatBackface:
                gSPDisplayList(&mb_curr_gfx[mb_gfx_index++], sidemat->gfx);
                render_boundary(wall_boundary, ARRAY_COUNT(wall_boundary), -40, bottomY, 0);
                gSPDisplayList(&mb_curr_gfx[mb_gfx_index++], &mat_maker_MakerBlack);
                render_boundary(wall_boundary, ARRAY_COUNT(wall_boundary), -40, bottomY, 2);
                if (showBackface) {
                    mb_render_flip_normals = TRUE;
                    showBackface = FALSE;
                    goto repeatBackface; // im such an awesome coder
                }
                mb_render_flip_normals = FALSE;
            }
        }
    }

    // Outer floor
    if (mb_curr_boundary & MB_BOUNDARY_OUTER_FLOOR) {
        s32 y = -32;
        if (mb_curr_boundary & MB_BOUNDARY_INNER_WALLS) {
            y = mb_lopt_boundary_height-32;
        }
        u32 topMatOpaque = (mat->type < MAT_CUTOUT);
        if (topMatOpaque && (processRenderMode == PROCESS_TILE_VPLEX)) {
            gDPPipeSync(&mb_curr_gfx[mb_gfx_index++]);
            gDPSetRenderMode(&mb_curr_gfx[mb_gfx_index++], G_RM_VPLEX_SCREEN, G_RM_VPLEX_SCREEN2);
            gSPDisplayList(&mb_curr_gfx[mb_gfx_index++], mat->gfx);
            render_boundary(floor_edge_boundary, ARRAY_COUNT(floor_edge_boundary), y, y, 0);
        }

        if (processRenderMode == PROCESS_TILE_TRANSPARENT) {
            if (topMatOpaque) {
                gDPPipeSync(&mb_curr_gfx[mb_gfx_index++]);
                gDPSetRenderMode(&mb_curr_gfx[mb_gfx_index++], G_RM_AA_ZB_XLU_DECAL, G_RM_AA_ZB_XLU_DECAL2);
            } else {
                gDPPipeSync(&mb_curr_gfx[mb_gfx_index++]);
                gDPSetRenderMode(&mb_curr_gfx[mb_gfx_index++], G_RM_AA_ZB_XLU_SURF, G_RM_AA_ZB_XLU_SURF2);
            }
            gSPDisplayList(&mb_curr_gfx[mb_gfx_index++], mat->gfx);
            render_boundary(floor_edge_boundary, ARRAY_COUNT(floor_edge_boundary), y, y, 0);
        }
    }

    // Ceiling
    if (mb_curr_boundary & MB_BOUNDARY_CEILING) {
        u8 sidematType = sidemat->type;
        u8 showBackface = (sidemat->type == MAT_CUTOUT);
        if (showBackface) {
            gSPClearGeometryMode(&mb_curr_gfx[mb_gfx_index++], G_CULL_BACK);
        }
        s32 y = mb_lopt_boundary_height-32;
        if (do_process(&sidematType, processRenderMode)) {
            mb_render_flip_normals = TRUE;
            set_render_mode( sidematType, FALSE);
            gSPDisplayList(&mb_curr_gfx[mb_gfx_index++], sidemat->gfx);
            render_boundary(floor_boundary, ARRAY_COUNT(floor_boundary), y, y, 0);
            mb_render_flip_normals = FALSE;
        }
        gSPSetGeometryMode(&mb_curr_gfx[mb_gfx_index++], G_CULL_BACK);
    }
}

void generate_terrain_gfx(void) {
    u8 tileType, rot;
    s8 pos[3];

    mb_curr_gfx = mb_terrain_gfx;
    mb_curr_vtx = mb_terrain_vtx;
    mb_gfx_index = 0;
    mb_build_collision_type = 0;

    mb_use_alt_uvs = FALSE;
    mb_render_flip_normals = FALSE;
    mb_render_vertical = FALSE;
    mb_curr_mat_has_topside = FALSE;
    mb_growth_render_type = 0;
    mb_uv_offset = (mb_lopt_theme == MB_THEME_MC ? -32 : -16);

    retroland_filter_on();

    // Recalculate level boundary
    mb_curr_boundary = mb_boundary_table[mb_lopt_boundary];
    if (mb_curr_boundary & MB_BOUNDARY_INNER_FLOOR) {
        if (mb_lopt_boundary_height == 0) {
            mb_curr_boundary &= ~MB_BOUNDARY_CEILING;
            mb_curr_boundary &= ~MB_BOUNDARY_INNER_WALLS;
        }
        if (!(mb_curr_boundary & (MB_BOUNDARY_INNER_WALLS | MB_BOUNDARY_OUTER_WALLS))) {
            mb_curr_boundary |= MB_BOUNDARY_OUTER_FLOOR;
        }
    }
    gMBMinCoord = (mb_grid_min - 32) * TILE_SIZE;
    gMBMaxCoord = (mb_grid_min + mb_grid_size - 32) * TILE_SIZE;
    if (!(mb_curr_boundary & MB_BOUNDARY_OUTER_FLOOR)) {
        gMBMinCoord -= 8*TILE_SIZE;
        gMBMaxCoord += 8*TILE_SIZE;
    }

    process_tiles(PROCESS_TILE_VPLEX);
    mb_curr_poly_vert_count = 4;
    process_boundary(PROCESS_TILE_VPLEX);

    //BOTTOM PLANE
    process_boundary(PROCESS_TILE_NORMAL);

    // Special Tiles
    mb_growth_render_type = 0;
    mb_use_alt_uvs = TRUE;
    u32 startIndex;
    u32 endIndex;

    // Bars
    u8 connections[5];
    startIndex = mb_tile_data_indices[BARS_TILETYPE_INDEX];
    endIndex = mb_tile_data_indices[BARS_TILETYPE_INDEX+1];
    set_render_mode( MAT_CUTOUT, FALSE);
    gSPDisplayList(&mb_curr_gfx[mb_gfx_index++], BARS_TEX());
    for (u32 i = startIndex; i < endIndex; i++) {
        s8 pos[3];
        vec3_set(pos, mb_tile_data[i].x, mb_tile_data[i].y, mb_tile_data[i].z);
        check_bar_connections(pos, connections);
        render_bars_side(pos, connections);
    }
    display_cached_tris();
    gSPDisplayList(&mb_curr_gfx[mb_gfx_index++], BARS_TOPTEX());
    gSPClearGeometryMode(&mb_curr_gfx[mb_gfx_index++], G_CULL_BACK);
    for (u32 i = startIndex; i < endIndex; i++) {
        s8 pos[3];
        vec3_set(pos, mb_tile_data[i].x, mb_tile_data[i].y, mb_tile_data[i].z);
        check_bar_connections(pos, connections);
        render_bars_top(pos, connections);
    }
    display_cached_tris();
    gSPSetGeometryMode(&mb_curr_gfx[mb_gfx_index++], G_CULL_BACK);

    // Fences

    startIndex = mb_tile_data_indices[FENCE_TILETYPE_INDEX];
    endIndex = mb_tile_data_indices[FENCE_TILETYPE_INDEX+1];
    gSPDisplayList(&mb_curr_gfx[mb_gfx_index++], FENCE_TEX());
    mb_growth_render_type = 3; // fence
    for (u32 i = startIndex; i < endIndex; i++) {
        s8 pos[3];
        vec3_set(pos, mb_tile_data[i].x, mb_tile_data[i].y, mb_tile_data[i].z);
        process_tile(pos, &mb_terrain_fence, mb_tile_data[i].rot);
    }
    display_cached_tris();

    process_tiles(PROCESS_TILE_NORMAL);

    retroland_filter_off();
    gDPSetTextureLUT(&mb_curr_gfx[mb_gfx_index++], G_TT_NONE);
    gDPPipeSync(&mb_curr_gfx[mb_gfx_index++]);
    gDPSetRenderMode(&mb_curr_gfx[mb_gfx_index++], G_RM_AA_ZB_OPA_SURF, G_RM_AA_ZB_OPA_SURF2);
    gSPEndDisplayList(&mb_curr_gfx[mb_gfx_index++]);

    mb_terrain_gfx_tp = &mb_curr_gfx[mb_gfx_index];
    retroland_filter_on();

    mb_curr_poly_vert_count = 4;
    process_boundary(PROCESS_TILE_TRANSPARENT);

    set_render_mode( MAT_TRANSPARENT, FALSE);
    gSPDisplayList(&mb_curr_gfx[mb_gfx_index++], WATER_TEX());

    mb_play_s16_water_level = -8224+(mb_lopt_waterlevel*TILE_SIZE); // Update water plane height

    // Render water blocks, interiors
    mb_render_flip_normals = TRUE;
    for (u32 i = 0; i < mb_tile_count; i++) {
        if (mb_tile_data[i].waterlogged) {
            if (((mb_tile_data[i].type == TILE_TYPE_BLOCK) || (mb_tile_data[i].type == TILE_TYPE_TROLL)) &&
                !fullblock_can_be_waterlogged(mb_tile_data[i].mat)) continue;
            vec3_set(pos, mb_tile_data[i].x, mb_tile_data[i].y, mb_tile_data[i].z);
            render_water(pos);
        }
    }
    // Render water blocks, exteriors
    mb_render_flip_normals = FALSE;
    for (u32 i = 0; i < mb_tile_count; i++) {
        if (mb_tile_data[i].waterlogged) {
            if (((mb_tile_data[i].type == TILE_TYPE_BLOCK) || (mb_tile_data[i].type == TILE_TYPE_TROLL)) &&
                !fullblock_can_be_waterlogged(mb_tile_data[i].mat)) continue;
            vec3_set(pos, mb_tile_data[i].x, mb_tile_data[i].y, mb_tile_data[i].z);
            render_water(pos);
        }
    }
    display_cached_tris();

    //mb_render_flip_normals = TRUE;
    // Render main water plane, bottom side
    // if (mb_lopt_waterlevel != 0) {
    //     render_floor((mb_lopt_waterlevel - 32) * TILE_SIZE - 32);
    // }
    //mb_render_flip_normals = FALSE;

    process_tiles(PROCESS_TILE_TRANSPARENT);
    gDPPipeSync(&mb_curr_gfx[mb_gfx_index++]);
    gDPSetRenderMode(&mb_curr_gfx[mb_gfx_index++], G_RM_AA_ZB_XLU_SURF, G_RM_AA_ZB_XLU_SURF2);
    retroland_filter_off();
    gDPSetTextureLUT(&mb_curr_gfx[mb_gfx_index++], G_TT_NONE);
    gSPEndDisplayList(&mb_curr_gfx[mb_gfx_index++]);

    mb_trajectory_gfx = &mb_curr_gfx[mb_gfx_index];
    mb_trajectory_vtx = mb_curr_vtx;

    generate_trajectory_gfx();

    mb_vtx_total = mb_curr_vtx - mb_terrain_vtx;
    mb_gfx_total = (mb_curr_gfx + mb_gfx_index) - mb_terrain_gfx;

    osViSetSpecialFeatures(OS_VI_DITHER_FILTER_OFF);
    osViSetSpecialFeatures(OS_VI_DIVOT_OFF);

    if (mb_vtx_total >= MB_VTX_SIZE) {
        show_error("CRITICAL WARNING: Vertex limit exceeded.");
    } else if (mb_vtx_total >= MB_VTX_SIZE - 30) {
        show_error("WARNING: Vertex limit is about to overflow.\nCreate any more vertices and you're cooked.");
    }
};

// Called whenever boundary is changed
void reload_boundary_and_gfx(void) {
    generate_terrain_gfx();
    generate_boundary_collision();
}

Gfx preview_gfx[50];
Vtx preview_vtx[100];
Gfx water_gfx[50];
Vtx water_vtx[100];

extern void geo_append_display_list(void *displayList, s32 layer);

void render_preview_block(u32 matid, u32 topmatid, s8 pos[3], struct MBTerrain *terrain, u32 rot, u32 processType, u32 disableZ) {
    mb_curr_mat_has_topside = (topmatid != matid);

    u8 matType = mb_mat_table[matid].type;

    if (do_process(&matType, processType)) {
        set_render_mode( matType, disableZ);
        gSPDisplayList(&mb_curr_gfx[mb_gfx_index++], mb_mat_table[matid].gfx);
        mb_render_vertical = mb_mat_table[matid].vertical;
        // Important to not use matType here so that it's still opaque for screens
        if (((matType == MAT_CUTOUT) ||
            ((mb_mat_table[matid].type < MAT_CUTOUT) && (mb_mat_table[topmatid].type >= MAT_CUTOUT)))
            && !disableZ) {
            gSPClearGeometryMode(&mb_curr_gfx[mb_gfx_index++], G_CULL_BACK);
        }
        process_tile(pos, terrain, rot);
        display_cached_tris();
    }

    if (mb_curr_mat_has_topside) {
        u8 topMatType = mb_mat_table[topmatid].type;
        if (!do_process(&topMatType, processType)) return;
        Gfx *sidetex = get_sidetex(topmatid);
        if (sidetex != NULL) {
            mb_use_alt_uvs = TRUE;
            mb_render_vertical = TRUE;
            mb_growth_render_type = 2;
            gSPDisplayList(&mb_curr_gfx[mb_gfx_index++], sidetex);

            // DECAL MODE
            if (matType != MAT_TRANSPARENT) { // Render in decal mode for cutouts, opaque and screen
                set_render_mode( MAT_DECAL, disableZ);
                process_tile(pos, terrain, rot);
                display_cached_tris();
            }
            // OPAQUE MODE
            if (matType >= MAT_CUTOUT) { // Render in cutout mode for cutouts and transparent
                set_render_mode( MAT_CUTOUT, disableZ);
                process_tile(pos, terrain, rot);
                display_cached_tris();
            }

            mb_use_alt_uvs = FALSE;
        }
        mb_growth_render_type = 1;

        set_render_mode( topMatType, disableZ);
        gSPDisplayList(&mb_curr_gfx[mb_gfx_index++], mb_mat_table[topmatid].gfx);
        mb_render_vertical = mb_mat_table[topmatid].vertical;
        process_tile(pos, terrain, rot);
        display_cached_tris();
    }
    gSPSetGeometryMode(&mb_curr_gfx[mb_gfx_index++], G_CULL_BACK);
}

void render_water_plane(void) {
    if (mb_lopt_waterlevel != 0) {
        mb_curr_vtx = water_vtx;
        mb_curr_gfx = water_gfx;
        mb_gfx_index = 0;

        retroland_filter_on();
        set_render_mode( MAT_TRANSPARENT, FALSE);
        gSPDisplayList(&mb_curr_gfx[mb_gfx_index++], WATER_TEX());
        gSPClearGeometryMode(&mb_curr_gfx[mb_gfx_index++], G_CULL_BACK);
        render_boundary_precise(floor_boundary, ARRAY_COUNT(floor_boundary), mb_play_s16_water_level, 0);
        render_boundary_precise(floor_edge_boundary, ARRAY_COUNT(floor_edge_boundary), mb_play_s16_water_level, 0);
        gSPSetGeometryMode(&mb_curr_gfx[mb_gfx_index++], G_CULL_BACK);
        gDPPipeSync(&mb_curr_gfx[mb_gfx_index++]);
        gDPSetRenderMode(&mb_curr_gfx[mb_gfx_index++], G_RM_AA_ZB_XLU_INTER, G_RM_AA_ZB_XLU_INTER2);
        gDPSetTextureLUT(&mb_curr_gfx[mb_gfx_index++], G_TT_NONE);
        retroland_filter_off();
        gSPEndDisplayList(&mb_curr_gfx[mb_gfx_index]);

        geo_append_display_list(water_gfx, LAYER_TRANSPARENT);
    }
}

u8 mb_append_frameone_bandaid_fix = FALSE; // N64-Only RCP Lockup 1 found day before release. This seems to fix it. May God be with me, Amen.
Gfx *mb_append(s32 callContext, UNUSED struct GraphNode *node, UNUSED Mat4 mtx) {
    if (callContext == GEO_CONTEXT_RENDER) {
        if (!mb_append_frameone_bandaid_fix) {
            mb_append_frameone_bandaid_fix = TRUE;
            return NULL;
        }

        geo_append_display_list(mb_terrain_gfx, LAYER_OPAQUE);
        geo_append_display_list(mb_terrain_gfx_tp, LAYER_TRANSPARENT);

        //this extra append is for the editor tile preview
        if (mb_mode == MB_MODE_MAKE) {
            if (!mb_prepare_level_screenshot) {
                geo_append_display_list(mb_trajectory_gfx, LAYER_OPAQUE);
            }
            //generate dl
            if (mb_place_mode != MB_PM_TILE) {
                render_water_plane();
                return NULL;
            }
            mb_curr_gfx = preview_gfx;
            mb_curr_vtx = preview_vtx;
            mb_gfx_index = 0;
            mb_growth_render_type = 0;
            mb_use_alt_uvs = FALSE;

            mb_build_collision_type = 0;

            retroland_filter_on();

            if (mb_id_selection == TILE_TYPE_WATER) {
                gSPDisplayList(&mb_curr_gfx[mb_gfx_index++], WATER_TEX());
                gSPGeometryMode(&mb_curr_gfx[mb_gfx_index++], 0, G_CULL_BACK);
                mb_curr_poly_vert_count = 4;
                render_water(mb_cursor_pos);
                display_cached_tris();
                retroland_filter_off();
                gDPSetTextureLUT(&mb_curr_gfx[mb_gfx_index++], G_TT_NONE);
                gSPEndDisplayList(&mb_curr_gfx[mb_gfx_index]);

                geo_append_display_list(mb_curr_gfx, LAYER_TRANSPARENT_INTER);

                render_water_plane();
                return NULL;
            }

            struct MBTerrain *terrain = mb_terrain_info_list[mb_id_selection].terrain;
            u8 mat = TILE_MATDEF(mb_mat_selection).mat;
            u8 topmat = TILE_MATDEF(mb_mat_selection).topmat;
            if (mb_id_selection == TILE_TYPE_POLE) {
                terrain = &mb_terrain_pole;
                mat = mb_theme_table[mb_lopt_theme].pole;
                topmat = mat;
                mb_use_alt_uvs = TRUE;
                mb_growth_render_type = 4; // pole
            }

            if (terrain) {
                // Handle Virtuaplex screen effect
                if (mat == MB_MAT_VP_SCREEN || topmat == MB_MAT_VP_SCREEN) {
                    render_preview_block(mat, topmat, mb_cursor_pos, terrain, mb_rot_selection, PROCESS_TILE_VPLEX, FALSE);

                    gSPEndDisplayList(&mb_curr_gfx[mb_gfx_index]);
                    geo_append_display_list(mb_curr_gfx, LAYER_FORCE);

                    mb_curr_gfx += mb_gfx_index;
                    mb_gfx_index = 0;
                }
                render_preview_block(mat, topmat, mb_cursor_pos, terrain, mb_rot_selection, PROCESS_TILE_BOTH, FALSE);

            } else if (mb_id_selection != TILE_TYPE_CULL) {
                mb_use_alt_uvs = TRUE;
                mb_curr_poly_vert_count = 4;
                if (mb_id_selection == TILE_TYPE_FENCE) {
                    gSPDisplayList(&mb_curr_gfx[mb_gfx_index++], FENCE_TEX());
                    set_render_mode( MAT_CUTOUT, FALSE);
                    mb_growth_render_type = 3; // fence
                    process_tile(mb_cursor_pos, &mb_terrain_fence, mb_rot_selection);
                } else if (mb_id_selection == TILE_TYPE_BARS) {
                    set_render_mode( MAT_CUTOUT, FALSE);
                    gSPDisplayList(&mb_curr_gfx[mb_gfx_index++], BARS_TEX());
                    u8 connections[5];
                    check_bar_connections(mb_cursor_pos, connections);
                    render_bars_side(mb_cursor_pos, connections);
                    display_cached_tris();
                    gSPDisplayList(&mb_curr_gfx[mb_gfx_index++], BARS_TOPTEX());
                    gSPClearGeometryMode(&mb_curr_gfx[mb_gfx_index++], G_CULL_BACK);
                    render_bars_top(mb_cursor_pos, connections);
                }
                display_cached_tris();
                gSPSetGeometryMode(&mb_curr_gfx[mb_gfx_index++], G_CULL_BACK);
                mb_use_alt_uvs = FALSE;
            }

            gDPPipeSync(&mb_curr_gfx[mb_gfx_index++]);
            gDPSetRenderMode(&mb_curr_gfx[mb_gfx_index++], G_RM_AA_ZB_XLU_INTER, G_RM_AA_ZB_XLU_INTER2);
            gDPSetTextureLUT(&mb_curr_gfx[mb_gfx_index++], G_TT_NONE);
            retroland_filter_off();
            gSPEndDisplayList(&mb_curr_gfx[mb_gfx_index]);

            geo_append_display_list(mb_curr_gfx, LAYER_TRANSPARENT_INTER);
        }

        render_water_plane();
    }
    return NULL;
}

void generate_boundary_quad_collision(struct MBBoundaryQuad *quadList, u32 count, s16 yBottom, s16 yTop, s16 size, u32 reverse) {
    TerrainData newVtxs[4][3];
    s16 yHeight = yTop - yBottom;
    for (u32 i = 0; i < count; i++) {
        for (u32 j = 0; j < 4; j++) {
            newVtxs[j][0] = quadList[i].vtx[j][0]*size*4;
            newVtxs[j][1] = (quadList[i].vtx[j][1]*yHeight + yBottom)*TILE_SIZE;
            newVtxs[j][2] = quadList[i].vtx[j][2]*size*4;
        }
        if (reverse) {
            mb_create_surface(newVtxs[0], newVtxs[2], newVtxs[1], TRUE);
            mb_create_surface(newVtxs[1], newVtxs[2], newVtxs[3], TRUE);
        } else {
            mb_create_surface(newVtxs[0], newVtxs[1], newVtxs[2], TRUE);
            mb_create_surface(newVtxs[1], newVtxs[3], newVtxs[2], TRUE);
        }
    }
}

void generate_block_collision(s8 pos[3]) {
    if (!coords_in_range(pos)) return;
    s32 tileType = get_grid_tile(pos)->type;

    mb_growth_render_type = 0;
    mb_curr_poly_vert_count = 4;

    if (tileType == TILE_TYPE_FENCE) {
        if (gCollisionFlags & COLLISION_FLAG_CAMERA) return;
        mb_curr_coltype = SURFACE_NO_CAM_COLLISION;
        process_tile(pos, &mb_terrain_fence_col, get_grid_tile(pos)->rot);
        return;
    }

    if (tileType == TILE_TYPE_BARS) {
        if (gCollisionFlags & COLLISION_FLAG_CAMERA) return;
        u8 connections[5];
        mb_curr_coltype = SURFACE_VANISH_CAP_WALLS;
        check_bar_connections(pos, connections);
        render_bars_side(pos, connections);
        render_bars_top(pos, connections);
        return;
    }

    if (tileType == TILE_TYPE_EMPTY || tileType >= TILE_TYPE_CULL) {
        return;
    }

    if (mb_build_collision_type == 1) {
        mb_growth_render_type = 1;
        mb_curr_coltype = TOPMAT(get_grid_tile(pos)->mat).col;
    }
    else mb_curr_coltype = MATERIAL(get_grid_tile(pos)->mat).col;

    process_tile(pos, mb_terrain_info_list[tileType].terrain, get_grid_tile(pos)->rot);
}

void scan_fences(s8 pos[3]) {
    for (s32 dir = 2; dir < 6; dir++) {
        s8 newPos[3];
        vec3_sum(newPos, pos, cullOffsetLUT[dir]);
        if ((get_grid_tile(newPos)->type == TILE_TYPE_FENCE) && (rotate_direction(MB_DIRECTION_POS_Z, get_grid_tile(newPos)->rot) == dir)) {
            generate_block_collision(newPos);
        }
    }
}

#define COL_POS_TO_GRID(pos) (((pos) + (32 * TILE_SIZE)) / TILE_SIZE)

#define begin_block_collision() { \
    gSurfacePool = gBlockSurfacePool; \
    gSurfaceNodePool = gBlockSurfaceNodePool; \
    gSurfacesAllocated = &gBlockSurfacesAllocated; \
    gSurfaceNodesAllocated = &gBlockSurfaceNodesAllocated; \
}
#define end_block_collision() { \
    gSurfacePool = gMainSurfacePool; \
    gSurfaceNodePool = gMainSurfaceNodePool; \
    gSurfacesAllocated = &gMainSurfacesAllocated; \
    gSurfaceNodesAllocated = &gMainSurfaceNodesAllocated; \
}


void block_floor_collision(f32 x, f32 y, f32 z) {
    s8 pos[3];
    mb_build_collision_type = 1;

    pos[0] = COL_POS_TO_GRID(x);
    pos[1] = COL_POS_TO_GRID(y + FIND_FLOOR_BUFFER);
    pos[2] = COL_POS_TO_GRID(z);

    begin_block_collision();
    scan_fences(pos);
    generate_block_collision(pos);
    pos[1]--;

    while (pos[1] >= 0) {
        scan_fences(pos);
        generate_block_collision(pos);

        if (!coords_in_range(pos) || (gCollisionFlags & COLLISION_FLAG_SHORT_FLOOR_CHECK)) {
            break;
        }
        s32 faceshapeTop = get_faceshape(pos, MB_DIRECTION_UP);
        s32 faceshapeBottom = get_faceshape(pos, MB_DIRECTION_DOWN);
        if (faceshapeTop == MB_FACESHAPE_FULL || faceshapeBottom == MB_FACESHAPE_FULL) {
            // Tile covers entire block, so stop searching
            break;
        }
        pos[1]--;
    }

    end_block_collision();
}

void block_ceil_collision(f32 x, f32 y, f32 z) {
    s8 pos[3];
    mb_build_collision_type = 2;

    pos[0] = COL_POS_TO_GRID(x);
    pos[1] = COL_POS_TO_GRID(y-5) + 1;
    pos[2] = COL_POS_TO_GRID(z);

    begin_block_collision();

    generate_block_collision(pos);
    pos[1]--;
    generate_block_collision(pos);

    end_block_collision();
}

void block_wall_collision(f32 x, f32 y, f32 z, f32 r) {
    s8 pos[3];
    mb_build_collision_type = 3;

    s32 minx = COL_POS_TO_GRID(x - r);
    s32 maxx = COL_POS_TO_GRID(x + r);
    pos[1] = COL_POS_TO_GRID(y);
    s32 minz = COL_POS_TO_GRID(z - r);
    s32 maxz = COL_POS_TO_GRID(z + r);

    begin_block_collision();

    for (s32 x = minx; x <= maxx; x++) {
        pos[0] = x;
        for (s32 z = minz; z <= maxz; z++) {
            pos[2] = z;
            generate_block_collision(pos);
        }
    }

    end_block_collision();
}

int check_pole(struct MarioState *m, s8 pos[3]) {
    if (get_grid_tile(pos)->type == TILE_TYPE_POLE) {
        f32 poleX = GRID_TO_POS(pos[0]);
        f32 poleZ = GRID_TO_POS(pos[2]);
        f32 horizDist = sqrtf(sqr(m->pos[0] - poleX) + sqr(m->pos[2] - poleZ));
        if (horizDist < 118) {
            // Get dimensions of pole
            s8 poleY = pos[1];
            do {
                pos[1]++;
            } while (pos[1] < 64 && get_grid_tile(pos)->type == TILE_TYPE_POLE);
            f32 poleTop = GRID_TO_POS(pos[1]) - TILE_SIZE/2;
            pos[1] = poleY;
            do {
                pos[1]--;
            } while (pos[1] >= 0 && get_grid_tile(pos)->type == TILE_TYPE_POLE);
            f32 poleBottom = GRID_TO_POS(pos[1]) + TILE_SIZE/2;

            gMarioCurrentPole.pos[0] = poleX;
            gMarioCurrentPole.pos[1] = poleBottom;
            gMarioCurrentPole.pos[2] = poleZ;
            gMarioCurrentPole.height = poleTop - poleBottom;
            gMarioCurrentPole.poleType = 0;
            interact_pole(m, 0);
            return TRUE;
        }
    }
    return FALSE;
}

void check_poles(struct MarioState *m) {
    s8 pos[3];

    pos[0] = COL_POS_TO_GRID(m->pos[0]);
    pos[1] = COL_POS_TO_GRID(m->pos[1]);
    pos[2] = COL_POS_TO_GRID(m->pos[2]);

    if (check_pole(m, pos)) return;

    s8 topPos = COL_POS_TO_GRID(m->pos[1] + 160);
    if (topPos != pos[1]) {
        pos[1] = topPos;
        if (check_pole(m, pos)) return;
    }

    // Iterate over polelike objects
    struct ObjectNode *listHead = &gObjectLists[OBJ_LIST_POLELIKE];
    struct Object *obj = (struct Object *) listHead->next;

    while (obj != (struct Object *) listHead) {
        f32 horizDist = sqrtf(sqr(m->pos[0] - obj->oPosX) + sqr(m->pos[2] - obj->oPosZ));
        if (horizDist < 118.f) {
            u16 poleHeight;
            u8 poleType = 0;
            if (obj->behavior == segmented_to_virtual(bhvTree)) {
                poleHeight = 500;
                poleType = (obj->oBehParams2ndByte == 2 ? 2 : 1);
            } else if (obj->behavior == segmented_to_virtual(bhvKoopaFlag)) {
                poleHeight = 700;
            } else {
                obj = (struct Object *) obj->header.next;
                continue;
            }
            if (m->pos[1] > obj->oPosY && m->pos[1] + 160.f < obj->oPosY + poleHeight) {
                gMarioCurrentPole.pos[0] = obj->oPosX;
                gMarioCurrentPole.pos[1] = obj->oPosY;
                gMarioCurrentPole.pos[2] = obj->oPosZ;
                gMarioCurrentPole.height = poleHeight;
                gMarioCurrentPole.poleType = poleType;
                interact_pole(m, 0);
                return;
            }
        }

        obj = (struct Object *) obj->header.next;
    }
}

void generate_boundary_collision(void) {
    *gSurfaceNodesAllocated = 0;
    *gSurfacesAllocated = 0;
    clear_static_surfaces();

    mb_curr_poly_vert_count = 4;

    u32 deathsize = ((mb_curr_boundary & MB_BOUNDARY_OUTER_FLOOR) || (mb_curr_boundary & MB_BOUNDARY_CEILING))? mb_grid_size : (mb_grid_size + 16);
    mb_curr_coltype = SURFACE_DEATH_PLANE;
    generate_boundary_quad_collision(floor_boundary, ARRAY_COUNT(floor_boundary), -40, -40, deathsize, FALSE);

    if (mb_curr_boundary & MB_BOUNDARY_INNER_FLOOR) {
        mb_curr_coltype = TOPMAT(mb_lopt_boundary_mat).col;
        generate_boundary_quad_collision(floor_boundary, ARRAY_COUNT(floor_boundary), -32, -32, mb_grid_size, FALSE);
    }
    mb_curr_coltype = MATERIAL(mb_lopt_boundary_mat).col;
    if (MATERIAL(mb_lopt_boundary_mat).type == MAT_TRANSPARENT) {
        mb_curr_coltype = SURFACE_DEFAULT;
    }
    if (mb_curr_boundary & MB_BOUNDARY_INNER_WALLS) {
        s32 bottomY = (mb_curr_boundary & MB_BOUNDARY_INNER_FLOOR) ? -32 : -40;
        s32 topY = mb_lopt_boundary_height-32;
        generate_boundary_quad_collision(wall_boundary, ARRAY_COUNT(wall_boundary), bottomY, topY, mb_grid_size, FALSE);
    } else if (mb_curr_boundary & MB_BOUNDARY_OUTER_WALLS) {
        generate_boundary_quad_collision(wall_boundary, ARRAY_COUNT(wall_boundary), -42, -32, mb_grid_size, TRUE);
    }
    if (mb_curr_boundary & MB_BOUNDARY_CEILING) {
        generate_boundary_quad_collision(floor_boundary, ARRAY_COUNT(floor_boundary), mb_lopt_boundary_height-32, mb_lopt_boundary_height-32, mb_grid_size, TRUE);
    }

    gNumStaticSurfaceNodes = *gSurfaceNodesAllocated;
    gNumStaticSurfaces = *gSurfacesAllocated;
}


s32 mb_get_water_level(s32 x, s32 y, s32 z) {
    //(mb_lopt_waterlevel - 32) * TILE_SIZE - (TILE_SIZE / 8)
    y += 10;
    s32 waterPlaneHeight = (mb_lopt_waterlevel == 0 ? FLOOR_LOWER_LIMIT : mb_play_s16_water_level);
    if (y < waterPlaneHeight) {
        return waterPlaneHeight;
    }
    // Convert world coordinates into grid coordinates
    s8 pos[3];
    vec3_set(pos, (x + 32*TILE_SIZE) / TILE_SIZE, (y + 32*TILE_SIZE) / TILE_SIZE, (z + 32*TILE_SIZE) / TILE_SIZE);

    // Check if out of range
    if (pos[1] > 63) {
        pos[1] = 63;
    }
    if (y < -32*TILE_SIZE) {
        return waterPlaneHeight;
    }
    if (!coords_in_range(pos)) return waterPlaneHeight;
    // If block contains water, scan upwards, otherwise scan downwards
    if (get_grid_tile(pos)->waterlogged) {
        // Find grid Y coordinate of highest water block.
        // Stop scanning once we hit a non-water block or the top is reached
        pos[1]++;
        while (pos[1] < 64 && get_grid_tile(pos)->waterlogged) {
            pos[1]++;
        }
        pos[1]--;
    } else {
        // Find grid Y coordinate of lowest non-water block.
        // Stop scanning once we hit a water block or the bottom is reached
        pos[1]--;
        while (pos[1] > -1 && !get_grid_tile(pos)->waterlogged) {
            pos[1]--;
        }
        if (pos[1] == -1) {
            return waterPlaneHeight;
        }
    }

    s32 waterBlockHeight;
    if (is_water_fullblock(pos)) {
        waterBlockHeight = (pos[1] - 31) * TILE_SIZE;
    } else {
        waterBlockHeight = (pos[1] - 31) * TILE_SIZE - (TILE_SIZE / 8);
    }
    return MAX(waterBlockHeight, waterPlaneHeight);
}

struct Object *spawn_preview_object(s8 pos[3], s32 rot, s32 param, struct MBObjectInfo *info, const BehaviorScript *script) {
    struct Object *preview_object = spawn_object(gMarioObject, info->modelId, script);
    preview_object->oPosX = GRID_TO_POS(pos[0]);
    preview_object->oPosY = GRID_TO_POS(pos[1]) - TILE_SIZE/2 + info->yOffset;
    preview_object->oPosZ = GRID_TO_POS(pos[2]);
    preview_object->oFaceAngleYaw = rot*0x4000;
    preview_object->oBehParams2ndByte = param;
    preview_object->oBehParams = (param << 16);
    preview_object->oPreviewObjDisplayFunc = info->dispFunc;
    preview_object->oOpacity = 255;
    obj_scale(preview_object, info->scale);
    if (info->flags & OBJ_TYPE_BILLBOARD) {
        preview_object->header.gfx.node.flags |= GRAPH_RENDER_BILLBOARD;
    }
    if (info->anim) {
        preview_object->oAnimations = (struct Animation **)info->anim;
        super_cum_working(preview_object,0);
        preview_object->header.gfx.animInfo.animAccel = 0.0f;
    }
    if ((info->flags & OBJ_TYPE_TRAJECTORY)
        && (script != bhvCurrPreviewObject)
        && (info->behavior != bhvLoopingPlatform)) {
        rotate_obj_toward_trajectory_angle(preview_object,param);
    }
    return preview_object;
}

void unload_all_preview_objs(void) {
    uintptr_t *behaviorAddr = segmented_to_virtual(bhvPreviewObject);
    struct ObjectNode *listHead = &gObjectLists[get_object_list_from_behavior(behaviorAddr)];
    struct Object *obj = (struct Object *) listHead->next;

    while (obj != (struct Object *) listHead) {
        struct Object *nextobj = (struct Object *) obj->header.next;
        if (obj->behavior == behaviorAddr) {
            unload_object(obj);
        }
        obj = nextobj;
    }
}

void generate_object_preview(void) {
    s32 totalCoins = 0;
    s32 curExtraCoins = 0;
    s32 doubleCoins = FALSE;
    mb_object_limit_count = 0;
    
    unload_all_preview_objs();

    for(u32 i = 0; i < mb_object_count; i++){
        if (gFreeObjectList.next == NULL) break;
        s32 curType = mb_object_data[i].type;
        struct MBObjectInfo *info = &mb_object_type_list[curType];
        s32 param = mb_object_data[i].bparam;

        s8 pos[3];
        vec3_set(pos, mb_object_data[i].x, mb_object_data[i].y, mb_object_data[i].z);
        curExtraCoins = 0;

        spawn_preview_object(pos, mb_object_data[i].rot, param, info, bhvPreviewObject);
        curExtraCoins += info->numCoins;
        if (curType == OBJECT_TYPE_EXCL_BOX) {
            curExtraCoins += mb_exclamation_box_contents[param].numCoins;
        }
        if (curType == OBJECT_TYPE_BADGE && param == 8) { // Greed badge
            doubleCoins = TRUE;
        }

        s32 extraObjs = get_extra_objects(curType, param);
        if (curType == OBJECT_TYPE_COIN_FORMATION) {
            curExtraCoins += extraObjs;
        }

        s32 curImbue = mb_object_data[i].imbue;
        if (curImbue != IMBUE_NONE) {
            int badgeid = 0;
            if (curImbue >= IMBUE_BADGE_BASE) {
                badgeid = curImbue - IMBUE_BADGE_BASE;
                curImbue = IMBUE_BADGE_BASE;
            }
            
            struct Object * imbue_marker = spawn_object(gMarioObject,imbue_table[curImbue].model,bhvPreviewObject);
            imbue_marker->oBehParams2ndByte = badgeid;
            imbue_marker->oExtraVariable1 = imbue_table[curImbue].color;
            imbue_marker->oPreviewObjDisplayFunc = df_hide_during_screenshot;
            imbue_marker->header.gfx.node.flags |= GRAPH_RENDER_BILLBOARD;
            imbue_marker->oPosX = GRID_TO_POS(pos[0]);
            imbue_marker->oPosY = GRID_TO_POS(pos[1]);
            imbue_marker->oPosZ = GRID_TO_POS(pos[2]);
            mb_object_limit_count ++;
            curExtraCoins = imbue_table[curImbue].coins; // replaces coin count
            if (curType == OBJECT_TYPE_SHOWRUNNER) {
                curExtraCoins += info->numCoins; // showrunner always drops coins
            }
        }

        mb_object_limit_count += extraObjs + 1;
        totalCoins += curExtraCoins;
    }
    if (doubleCoins) totalCoins *= 2;

    u32 length = MIN(totalCoins / 20, 50);
    mb_lopt_coinstar_max = length;

    if (mb_lopt_coinstar > length) {
        mb_lopt_coinstar = length;
    }
    mb_total_coin_count = totalCoins;

    generate_trajectory_gfx();
}

void generate_objects_to_level(void) {
    struct Object *obj;
    u32 i;
    mb_play_stars_max = 0;
    for(i=0;i<mb_object_count;i++){
        struct MBObjectInfo *info = &mb_object_type_list[mb_object_data[i].type];
        s32 param = mb_object_data[i].bparam;

        obj = spawn_object(gMarioObject, info->modelId, info->behavior);
        obj->oPosX = GRID_TO_POS(mb_object_data[i].x);
        obj->oPosY = GRID_TO_POS(mb_object_data[i].y) - TILE_SIZE/2 + info->yOffset;
        obj->oPosZ = GRID_TO_POS(mb_object_data[i].z);
        obj->oFaceAngleYaw = mb_object_data[i].rot*0x4000;
        obj->oMoveAngleYaw = mb_object_data[i].rot*0x4000;
        obj->oBehParams2ndByte = param;
        obj->oBehParams = (param << 16);
        obj->oImbue = mb_object_data[i].imbue;

        //assign star ids
        if ((info->flags & OBJ_TYPE_STAR)||(mb_object_data[i].imbue == IMBUE_STAR)) {
            if (mb_play_stars_max < 63) {
                obj->oBehParams = ((mb_play_stars_max << 24)|(o->oBehParams2ndByte << 16));
                mb_play_stars_max++;
            }
        }
    }
    if (mb_lopt_coinstar > 0) mb_play_stars_max++; // 100 coin star
}

// shift all indices past the given one by 1
// return index to insert a new block at
u32 shift_tile_data_indices(u32 tiletypeIndex) {
    u32 tiledataIndex = mb_tile_data_indices[tiletypeIndex + 1];
    // Shift all indices forward one
    for (u32 i = tiletypeIndex + 1; i < ARRAY_COUNT(mb_tile_data_indices); i++) {
        mb_tile_data_indices[i]++;
    }

    // Shift all data forward one
    for (u32 i = mb_tile_count; i > tiledataIndex; i--) {
        mb_tile_data[i] = mb_tile_data[i - 1];
    }
    return tiledataIndex;
}

u32 is_cull_marker_useless(s8 pos[3]) {
    s8 adjacentPos[3];

    for (u8 dir = 0; dir < 6; dir++) {
        vec3_sum(adjacentPos, pos, cullOffsetLUT[dir]);
        u8 tileType = get_grid_tile(adjacentPos)->type;
        if ((tileType != TILE_TYPE_EMPTY) && (tileType != TILE_TYPE_CULL)) {
            return FALSE;
        }
    }

    return TRUE;
}


void place_tile(s8 pos[3]) {
    u8 waterlogged = FALSE;
    // Placing tile upon water automatically waterlogs new tile
    // But delete the old tile first
    if (get_grid_tile(pos)->type == TILE_TYPE_WATER) {
        waterlogged = TRUE;
        for (u32 i = 0; i < mb_tile_count; i++) {
        //search for tile to delete
            if ((mb_tile_data[i].x == pos[0])&&(mb_tile_data[i].y == pos[1])&&(mb_tile_data[i].z == pos[2])) {
                mb_tile_count--;
                for (u32 j = WATER_TILETYPE_INDEX + 1; j < ARRAY_COUNT(mb_tile_data_indices); j++) {
                    mb_tile_data_indices[j]--;
                }
                for (u32 j = i; j < mb_tile_count; j++) {
                    mb_tile_data[j] = mb_tile_data[j+1];
                }
                break;
            }
        }
    }

    if (mb_id_selection == TILE_TYPE_BLOCK) {
        if (!fullblock_can_be_waterlogged(mb_mat_selection)) {
            waterlogged = FALSE;
        }
    }
    // If placing a cull marker, check that its actually next to a tile
    if (mb_id_selection == TILE_TYPE_CULL && is_cull_marker_useless(pos)) {
        return;
    }

    if (mb_terrain_info_list[mb_id_selection].terrain != NULL) {
        TerrainData coltype = TOPMAT(mb_mat_selection).col;
        if (SURFACE_IS_BURNING(coltype)) {
            play_place_sound(SOUND_GENERAL_LOUD_BUBBLE | SOUND_VIBRATO);
        } else {
            play_place_sound(SOUND_ACTION_TERRAIN_STEP + get_terrain_sound_addend(coltype));
        }
    } else {
        switch (mb_id_selection ) {
            case TILE_TYPE_FENCE:
                if (mb_lopt_theme == MB_THEME_RHR) {
                    play_place_sound(SOUND_ACTION_TERRAIN_STEP + (SOUND_TERRAIN_STONE << 16));
                } else {
                    play_place_sound(SOUND_ACTION_TERRAIN_STEP + (SOUND_TERRAIN_SPOOKY << 16));
                }
                break;
            case TILE_TYPE_POLE:
            case TILE_TYPE_BARS:
                play_place_sound(SOUND_ACTION_TERRAIN_STEP + (SOUND_TERRAIN_STONE << 16));
                break;
            case TILE_TYPE_CULL:
                play_place_sound(SOUND_GENERAL_DOOR_INSERT_KEY | SOUND_VIBRATO);
                break;
        }
    }

    place_terrain_data(pos, mb_id_selection, mb_rot_selection, mb_mat_selection);
    get_grid_tile(pos)->waterlogged = waterlogged;
    u32 index = get_tiletype_index(mb_id_selection, mb_mat_selection);
    u32 newtileIndex = shift_tile_data_indices(index);

    mb_tile_data[newtileIndex].x = pos[0];
    mb_tile_data[newtileIndex].y = pos[1];
    mb_tile_data[newtileIndex].z = pos[2];
    mb_tile_data[newtileIndex].type = mb_id_selection;
    mb_tile_data[newtileIndex].mat = mb_mat_selection;
    mb_tile_data[newtileIndex].rot = mb_rot_selection;
    mb_tile_data[newtileIndex].waterlogged = waterlogged;
    mb_tile_count++;
}


void place_water(s8 pos[3]) {
    struct MBGridObject *tile = get_grid_tile(pos);
    if (tile->waterlogged) return;

    if (tile->type != TILE_TYPE_EMPTY) {
        // cant waterlog a full block
        if ((tile->type == TILE_TYPE_BLOCK) && !fullblock_can_be_waterlogged(tile->mat)) {
            return;
        }
        play_place_sound(SOUND_ACTION_TERRAIN_STEP + (SOUND_TERRAIN_WATER << 16));
        tile->waterlogged = TRUE;
        u32 tileIndex = get_tiletype_index(tile->type, tile->mat);
        for (u32 i = mb_tile_data_indices[tileIndex]; i < mb_tile_data_indices[tileIndex + 1]; i++) {
            if (mb_tile_data[i].x == pos[0] && mb_tile_data[i].y == pos[1] && mb_tile_data[i].z == pos[2]) {
                mb_tile_data[i].waterlogged = TRUE;
                break;
            }
            // should not be possible to reach here
        }
    } else {
        // empty currently, so add water
        place_terrain_data(pos, TILE_TYPE_WATER, 0, 0);
        tile->waterlogged = TRUE;
        u32 newtileIndex = shift_tile_data_indices(WATER_TILETYPE_INDEX);

        mb_tile_data[newtileIndex].x = pos[0];
        mb_tile_data[newtileIndex].y = pos[1];
        mb_tile_data[newtileIndex].z = pos[2];
        mb_tile_data[newtileIndex].type = TILE_TYPE_WATER; // should be unused
        mb_tile_data[newtileIndex].mat = 0; // should be unused
        mb_tile_data[newtileIndex].rot = 0;
        mb_tile_data[newtileIndex].waterlogged = TRUE;
        mb_tile_count++;
        play_place_sound(SOUND_ACTION_TERRAIN_STEP + (SOUND_TERRAIN_WATER << 16));
    }
}


void remove_trajectory(u32 index) {
    // Scan all objects
    // If their trajectory index is past the one being deleted, lower it by 1
    for (u32 i = 0; i < mb_object_count; i++) {
        if (mb_object_type_list[mb_object_data[i].type].flags & OBJ_TYPE_TRAJECTORY) {
            if (mb_object_data[i].bparam > index) {
                mb_object_data[i].bparam--;
            }
        }
    }
    // Move trajectories back by one
    for (s32 i = index; i < mb_trajectories_used - 1; i++) {
        bcopy(mb_trajectory_list[i + 1], mb_trajectory_list[i], sizeof(mb_trajectory_list[0]));
    }
    // Zero out the last one
    bzero(mb_trajectory_list[mb_trajectories_used - 1], sizeof(mb_trajectory_list[0]));
    mb_trajectories_used--;
}


void delete_object(s32 index) {
    if (mb_object_type_list[mb_object_data[index].type].flags & OBJ_TYPE_TRAJECTORY) { 
        remove_trajectory(mb_object_data[index].bparam);
    }

    mb_object_count--;
    for (u32 i = index; i < mb_object_count; i++) {
        mb_object_data[i] = mb_object_data[i+1];
    }
    generate_object_preview();
}

void place_number_spawner_check(s8 pos[3], s32 spawnerType, s32 objType, s32 objImbue) {
    s32 objectCount = 0;
    s32 hasSpawner = FALSE;
    for (s32 i = 0; i < mb_object_count; i++) {
        if (mb_object_data[i].type == spawnerType) {
            hasSpawner = TRUE;
        } else if ((mb_object_data[i].type == objType) ||
                    (mb_object_data[i].imbue == objImbue)) {
            objectCount++;
        }
    }
    if (hasSpawner) df_spawn_number(pos, objectCount);
}

void should_spawn_place_number(s8 pos[3]) {
    if (mb_id_selection == OBJECT_TYPE_RED_COIN) {
        place_number_spawner_check(pos, OBJECT_TYPE_RED_COIN_STAR, OBJECT_TYPE_RED_COIN, IMBUE_RED_COIN);
    } else if (mb_id_selection == OBJECT_TYPE_TRIGGER) {
        place_number_spawner_check(pos, OBJECT_TYPE_TRIGGER_STAR, OBJECT_TYPE_TRIGGER, IMBUE_TRIGGER);
    } else if ((mb_object_type_list[mb_id_selection].flags & OBJ_TYPE_STAR)) {
        s32 starCount = 0;
        for (s32 i = 0; i < mb_object_count; i++) {
            if ((mb_object_type_list[mb_object_data[i].type].flags & OBJ_TYPE_STAR) ||
                (mb_object_data[i].imbue == IMBUE_STAR)){
                starCount++;
            }
        }
        df_spawn_number(pos, starCount);
    }
}

void place_object(s8 pos[3]) {
    // If spawn, delete old spawn
    if (mb_id_selection == OBJECT_TYPE_MARIO_SPAWN) {
        for (s32 i = 0; i < mb_object_count; i++) {
            if (mb_object_data[i].type == OBJECT_TYPE_MARIO_SPAWN) {
                s8 pos[3];
                vec3_set(pos, mb_object_data[i].x, mb_object_data[i].y, mb_object_data[i].z);
                delete_object(i);
                break;
            }
        }
    }

    mb_object_data[mb_object_count].x = pos[0];
    mb_object_data[mb_object_count].y = pos[1];
    mb_object_data[mb_object_count].z = pos[2];
    mb_object_data[mb_object_count].type = mb_id_selection;
    mb_object_data[mb_object_count].rot = mb_rot_selection;
    mb_object_data[mb_object_count].imbue = IMBUE_NONE;

    if (mb_object_type_list[mb_id_selection].flags & OBJ_TYPE_TRAJECTORY) {
        mb_trajectory_to_edit = mb_trajectories_used;
        mb_object_data[mb_object_count].bparam = mb_trajectories_used;
        mb_trajectories_used++;

        gMenuState = MB_MAKE_TRAJECTORY;
        create_yellow_text("Building path in progress!\n\n\x10: Place waypoint\n\x11: Undo\nSTART: Confirm");
        toolbar_set_active(FALSE);

        mb_trajectory_list[mb_trajectory_to_edit][0][0] = -1;
        mb_trajectory_edit_index = 0;
    } else if (mb_object_type_list[mb_id_selection].flags & OBJ_TYPE_HAS_DIALOG) {
        mb_object_data[mb_object_count].bparam = 0;
        mb_dialog_edit_ptr = &mb_object_data[mb_object_count];
    } else {
        mb_object_data[mb_object_count].bparam = mb_param_selection;
    }

    if (mb_id_selection == OBJECT_TYPE_MONEYBAG) {
        mb_object_data[mb_object_count].imbue = IMBUE_THREE_COINS;
    } else if (mb_id_selection == OBJECT_TYPE_BOO) {
        mb_object_data[mb_object_count].imbue = IMBUE_ONE_COIN;
    }

    mb_object_count++;

    play_place_sound(mb_object_type_list[mb_id_selection].soundBits);
    should_spawn_place_number(pos);
}

void imbue_action(void) {
    for (u32 i=0;i<mb_object_count;i++) {
        s32 objType = mb_object_data[i].type;
        s32 canbeImbued = (mb_object_type_list[objType].flags & OBJ_TYPE_IMBUABLE);
        if (mb_id_selection == OBJECT_TYPE_TRIGGER) {
            canbeImbued |= (mb_object_type_list[objType].flags & OBJ_TYPE_IMBUABLE_TRIGGER);
        }
        if (canbeImbued && (mb_object_data[i].x == mb_cursor_pos[0])&&(mb_object_data[i].y == mb_cursor_pos[1])&&(mb_object_data[i].z == mb_cursor_pos[2])) {
            u8 imbue_success = FALSE;
            if (!object_sanity_check()) break;
            u8 oldImbue = mb_object_data[i].imbue;

            switch(mb_id_selection) {
                case OBJECT_TYPE_STAR:
                    ;s32 numStars = mb_count_stars();
                    if (numStars >= 63) {
                        show_error("Star limit reached! (max 63)");
                        imbue_success = FALSE;
                        break;
                    }
                    mb_object_data[i].imbue = IMBUE_STAR;
                    imbue_success = TRUE;
                    break;
                case OBJECT_TYPE_BLUE_COIN:
                    if (mb_object_type_list[objType].flags & OBJ_TYPE_IMBUABLE_COINS) {
                        mb_object_data[i].imbue = IMBUE_BLUE_COIN;
                        imbue_success = TRUE;
                    }
                    break;
                case OBJECT_TYPE_COIN:
                case OBJECT_TYPE_COIN_FORMATION:
                    if (mb_object_type_list[objType].flags & OBJ_TYPE_IMBUABLE_COINS) {
                        if ((objType == OBJECT_TYPE_BBOX_NORMAL) ||
                            (objType == OBJECT_TYPE_RFBOX) ||
                            (objType == OBJECT_TYPE_MONEYBAG)) {
                            mb_object_data[i].imbue = IMBUE_THREE_COINS;
                        } else {
                            mb_object_data[i].imbue = IMBUE_ONE_COIN;
                        }
                        imbue_success = TRUE;
                    }
                    break;

                case OBJECT_TYPE_BUTTON:
                    mb_object_data[i].imbue = IMBUE_RED_SWITCH;
                    if (mb_param_selection == 1) {
                        mb_object_data[i].imbue = IMBUE_BLUE_SWITCH;
                    }
                    imbue_success = TRUE;
                    break;

#define generic_imbue(obj, imbueType) \
                case obj: \
                    mb_object_data[i].imbue = imbueType; \
                    imbue_success = TRUE; \
                    break;

                generic_imbue(OBJECT_TYPE_RED_COIN, IMBUE_RED_COIN);
                generic_imbue(OBJECT_TYPE_TRIGGER, IMBUE_TRIGGER);
                generic_imbue(OBJECT_TYPE_CROWBAR, IMBUE_CROWBAR);
                generic_imbue(OBJECT_TYPE_MASK, IMBUE_BULLET_MASK);
                generic_imbue(OBJECT_TYPE_GREEN_COIN, IMBUE_GREEN_COIN);
                generic_imbue(OBJECT_TYPE_BADGE, IMBUE_BADGE_BASE + mb_param_selection);
            }

            if (imbue_success && (oldImbue != mb_object_data[i].imbue)) {
                if (objType == OBJECT_TYPE_EXCL_BOX && mb_object_data[i].bparam <= 3) {
                    mb_object_data[i].bparam = 4;
                }

                play_place_sound(mb_object_type_list[mb_id_selection].soundBits);
                generate_object_preview();
                should_spawn_place_number(mb_cursor_pos);
                return;
            }
            break;
        }
    }
    if (mb_id_selection == OBJECT_TYPE_TRIGGER) {
        show_error("Star Triggers must be placed in other objects!");
    }
}

void place_thing_action(void) {
    if (mb_place_mode == MB_PM_TILE) {
        if (tile_sanity_check()) {
            if (mb_id_selection == TILE_TYPE_WATER) {
                place_water(mb_cursor_pos);
            } else if (can_place_tile(mb_cursor_pos)) {
                place_tile(mb_cursor_pos);
            } else {
                return;
            }
            generate_terrain_gfx();
        }
    } else if (mb_place_mode == MB_PM_OBJ) {
        if (can_place_object(mb_cursor_pos)) {
            if (object_sanity_check()) {
                place_object(mb_cursor_pos);
                generate_object_preview();
            }
        } else {
            imbue_action();
        }
    }
}


void delete_useless_cull_markers() {
    for (u32 i = mb_tile_data_indices[CULL_TILETYPE_INDEX]; i < mb_tile_data_indices[CULL_TILETYPE_INDEX + 1]; i++) {
        s8 pos[3];
        vec3_set(pos, mb_tile_data[i].x, mb_tile_data[i].y, mb_tile_data[i].z);

        if (!is_cull_marker_useless(pos)) {
            continue;
        }

        // Useless, delete
        remove_terrain_data(pos);
        mb_tile_count--;

        for (u32 j = CULL_TILETYPE_INDEX + 1; j < ARRAY_COUNT(mb_tile_data_indices); j++) {
            mb_tile_data_indices[j]--;
        }
        for (u32 j = i; j < mb_tile_count; j++) {
            mb_tile_data[j] = mb_tile_data[j+1];
        }

        i--;
    }
}


//function name is delete tile, it deletes objects too
void delete_tile_action(s8 pos[3]) {
    s16 index = -1;

    for (u32 i = 0; i < mb_tile_count; i++) {
        //search for tile to delete
        if ((mb_tile_data[i].x == pos[0])&&(mb_tile_data[i].y == pos[1])&&(mb_tile_data[i].z == pos[2])) {
            index = i;
            remove_terrain_data(pos);
            play_place_sound(SOUND_GENERAL_DOOR_INSERT_KEY | SOUND_VIBRATO);
            mb_tile_count--;
        }
    }

    if (index != -1) {
        u32 tiletypeIndex = get_tiletype_index(mb_tile_data[index].type, mb_tile_data[index].mat);

        for (u32 i = tiletypeIndex + 1; i < ARRAY_COUNT(mb_tile_data_indices); i++) {
            mb_tile_data_indices[i]--;
        }
        for (u32 i = index; i < mb_tile_count; i++) {
            mb_tile_data[i] = mb_tile_data[i+1];
        }
        delete_useless_cull_markers();
        generate_terrain_gfx();
    }

    for (u32 i=0;i<mb_object_count;i++) {
        if ((mb_object_data[i].x == pos[0])&&(mb_object_data[i].y == pos[1])&&(mb_object_data[i].z == pos[2])) {
            if (mb_object_data[i].type == OBJECT_TYPE_MARIO_SPAWN) {
                show_error("Cannot delete spawn point!");
                break;
            }
            delete_object(i);
            i--;
            play_place_sound(SOUND_GENERAL_DOOR_INSERT_KEY | SOUND_VIBRATO);
        }
    }
}


void update_painting() {
    s16 x;
    s16 y;
    u16 *u16_array = segmented_to_virtual(bigpainting2_bigger_painting_rgba16);
    for (x = 0; x < 64; x++) {
        for (y = 0; y < 64; y++) {
            u16_array[(y*64)+x] = mb_save.piktcher[y][x];
        } 
    }
}

void mb_init_toolbox(void) {
    bzero(&mb_toolbox_params, sizeof(mb_toolbox_params));
    switch(mb_lopt_game) {
        case MB_GAME_BTCM:
            bcopy(&mb_toolbox_btcm,&mb_toolbox,sizeof(mb_toolbox));
            mb_exclamation_box_contents = sExclamationBoxContents_btcm;
            break;
        case MB_GAME_VANILLA:
            bcopy(&mb_toolbox_vanilla,&mb_toolbox,sizeof(mb_toolbox));
            mb_exclamation_box_contents = sExclamationBoxContents_vanilla;
            break;
    }
}

TCHAR mb_file_name[MAX_FILE_NAME_SIZE];
FIL mb_file;
FILINFO mb_file_info;

char file_header_string[] = "MB-v1.1";

extern u16 sRenderedFramebuffer;
#define INSTANT_INPUT_BLACKLIST (EMU_CONSOLE | EMU_WIIVC | EMU_ARES | EMU_SIMPLE64 | EMU_CEN64)

void mb_level_save(void) {
    //bzero(&mb_save, sizeof(mb_save)); // should be safe to not need this right?
    if (mb_vtx_total >= MB_VTX_SIZE) {
        show_error("Save Failed - Vertex limit exceeded.");
        return;
    }

    //file header
    strncpy(mb_save.file_header, file_header_string, 10);

    //author
    if ((mb_has_username) && (mb_save.author[0] == '\0')) {
        strncpy(mb_save.author, mb_username, MAX_USERNAME_SIZE);
    }

    mb_save.tile_count = mb_tile_count;
    mb_save.object_count = mb_object_count;

    mb_save.costume = mb_lopt_costume;
    mb_save.seq[0] = mb_lopt_seq[0];
    mb_save.seq[1] = mb_lopt_seq[1];
    mb_save.seq[2] = mb_lopt_seq[2];
    mb_save.envfx = mb_lopt_envfx;
    mb_save.theme = mb_lopt_theme;
    mb_save.bg = mb_lopt_bg;
    mb_save.boundary_mat = mb_lopt_boundary_mat;
    mb_save.boundary = mb_lopt_boundary;
    mb_save.boundary_height = mb_lopt_boundary_height;
    mb_save.coinstar = mb_lopt_coinstar;
    mb_save.size = mb_lopt_size;
    mb_save.waterlevel = mb_lopt_waterlevel;
    mb_save.secret = mb_lopt_secret;
    mb_save.game = mb_lopt_game;

    for (s32 i = 0; i < MB_MAX_TRAJECTORIES; i++) {
        for (s32 j = 0; j < MB_TRAJECTORY_LENGTH; j++) {
            mb_save.trajectories[i][j].t = mb_trajectory_list[i][j][0];
            mb_save.trajectories[i][j].x = POS_TO_GRID(mb_trajectory_list[i][j][1]);
            mb_save.trajectories[i][j].y = POS_TO_GRID(mb_trajectory_list[i][j][2]);
            mb_save.trajectories[i][j].z = POS_TO_GRID(mb_trajectory_list[i][j][3]);
        }
    }

    // If in screenshot mode
    if (mb_prepare_level_screenshot) {
        u8 screenshot_failure = TRUE;

        for (s32 x=0;x<64;x++) {
            for (s32 y=0;y<64;y++) {
                int i = (y*64)+x;
                //take a "screenshot" of the level & burn in a painting frame
                if (mb_painting_frame_rgba16[(i*2)+1]==0x00) {
                    // Take samples (double resolution)
                    u16 sample[4];
                    for (s32 sx=0;sx<2;sx++) {
                        for (s32 sy=0;sy<2;sy++) {
                            if (gEmulator & INSTANT_INPUT_BLACKLIST) {
                                sample[sy*2+sx] = (gFramebuffers[(sRenderedFramebuffer+2)%3][ ((s32)((y*2+sy)*1.875f))*320 + (s32)((x*2+sx)*1.875f+40) ] | 1);
                            } else {
                                sample[sy*2+sx] = (gFramebuffers[0][ ((s32)((y*2+sy)*1.875f))*320 + (s32)((x*2+sx)*1.875f+40) ] | 1);
                            }
                        }
                    }

                    // Average 4 samples into single pixel
                    f32 avgColor[3] = {0.0f,0.0f,0.0f}; //floating point is overkill but might as well make it accurate as possible
                    for (int c = 0; c < 3; c++) {
                        int shift;
                        switch(c) {
                            case 0: shift=11;break;
                            case 1: shift=6;break;
                            case 2: shift=1;break;
                        }
                        for (int s = 0; s < 4; s++) {
                            avgColor[c] += (f32)((sample[s] >> shift) & 0x1F);
                        }
                        avgColor[c] /= 4.0f; //average of 4 samples
                    }

                    mb_save.piktcher[y][x] = ((u16)avgColor[0] << 11) | ((u16)avgColor[1] << 6) | ((u16)avgColor[2] << 1) | 1;
                    //mb_save.piktcher[y][x] = sample[0];

                    if (mb_save.piktcher[y][x] > 1) { //assumes all fb rgba16 values is initialized to 1 or 0
                        screenshot_failure = FALSE;
                    }
                } else {
                    //painting frame
                    mb_save.piktcher[y][x] = ((mb_painting_frame_rgba16[(i*2)]<<8) | mb_painting_frame_rgba16[(i*2)+1]);
                }
            }
        }

        if (screenshot_failure) {
            //framebuffer emulation not enabled, use ?
            show_error("Screenshot failed.\nMake sure framebuffer emulation (FBE) is enabled.");
            bcopy(&mb_painting_frame_mystery_rgba16,&mb_save.piktcher,sizeof(mb_save.piktcher));
        }

        update_painting();
    }

    if (mb_save.piktcher[0][0] == 0) { //0 is a transparent pixel in rgba16
        //use mystery painting if no screenshot has been taken yet
        bcopy(&mb_painting_frame_mystery_rgba16,&mb_save.piktcher,sizeof(mb_save.piktcher));
    }
    bcopy(&mb_curr_custom_theme,&mb_save.custom_theme,sizeof(struct MBCustomTheme));
    bcopy(&mb_toolbar, &mb_save.toolbar, sizeof(mb_save.toolbar));
    bcopy(&mb_toolbar_params, &mb_save.toolbar_params, sizeof(mb_save.toolbar_params));

    TCHAR path[256];
    create_level_file_path(path, mb_file_name, NULL);
    UINT bytes_written;
    f_open(&mb_file,path, FA_READ | FA_WRITE | FA_CREATE_ALWAYS);
    //write header
    f_write(&mb_file,&mb_save,sizeof(mb_save),&bytes_written);
    //write tiles
    f_write(&mb_file,&mb_tile_data,(sizeof(mb_tile_data[0])*mb_tile_count),&bytes_written);
    //write objects
    f_write(&mb_file,&mb_object_data,(sizeof(mb_object_data[0])*mb_object_count),&bytes_written);

    f_close(&mb_file);
}

/// @brief Loads a level into memory
void mb_level_load() {
    s32 i;
    s32 j;
    u8 fresh = FALSE;

    bzero(&mb_save, sizeof(mb_save));
    bzero(&mb_grid_data, sizeof(mb_grid_data));

    TCHAR path[256];
    create_level_file_path(path, mb_file_name, NULL);
    FRESULT code = f_stat(path,&mb_file_info);
    if (code == FR_OK) {
        UINT bytes_read;
        //file exists, load it
        f_open(&mb_file,path, FA_READ | FA_WRITE);
        //read header
        f_read(&mb_file,&mb_save,sizeof(mb_save),&bytes_read);
        //read tiles
        f_read(&mb_file,&mb_tile_data,sizeof(mb_tile_data[0])*mb_save.tile_count,&bytes_read);
        //read objects
        f_read(&mb_file,&mb_object_data,sizeof(mb_object_data[0])*mb_save.object_count,&bytes_read);
        //Close object
        f_close(&mb_file);
    } else {
        //Load into a fresh level
        fresh = TRUE;
        strncpy(mb_file_info.fname,mb_file_name,MAX_FILE_NAME_SIZE);

        //Set version
        mb_save.version = MB_VERSION;

        //Place spawn location
        mb_save.object_count = 1;
        mb_object_data[0].x = 32;
        mb_object_data[0].z = 32;
        mb_object_data[0].y = mb_templates[mb_lopt_template].spawnHeight;
        mb_object_data[0].type = OBJECT_TYPE_MARIO_SPAWN;

        mb_save.game = mb_lopt_game;
        mb_save.size = mb_lopt_size;

        mb_save.seq[0] = mb_templates[mb_lopt_template].music[mb_lopt_game];
        mb_save.seq[1] = 1;
        mb_save.seq[2] = 10;
        if (mb_lopt_game == MB_GAME_BTCM) {
            mb_save.seq[2] = 22;
        }
        mb_save.envfx = mb_templates[mb_lopt_template].envfx;
        mb_save.theme = mb_templates[mb_lopt_template].theme;
        mb_save.bg = mb_templates[mb_lopt_template].bg;
        mb_save.boundary_mat = mb_templates[mb_lopt_template].boundaryMat;
        mb_save.waterlevel = mb_templates[mb_lopt_template].water;
        mb_save.boundary = mb_templates[mb_lopt_template].boundary;
        mb_save.boundary_height = mb_templates[mb_lopt_template].boundaryHeight;

        bcopy(&mb_toolbar_defaults,&mb_save.toolbar,sizeof(mb_save.toolbar));
        bzero(&mb_save.toolbar_params,sizeof(mb_save.toolbar_params));

        if (mb_templates[mb_lopt_template].platform) {
            u8 i = 0;
            for (s32 x = -1; x <= 1; x++) {
                for (s32 z = -1; z <= 1; z++) {
                    mb_tile_data[i].x = 32+x;
                    mb_tile_data[i].y = mb_templates[mb_lopt_template].spawnHeight - 3;
                    mb_tile_data[i].z = 32+z;
                    mb_tile_data[i].type = TILE_TYPE_BLOCK;
                    mb_tile_data[i].mat = mb_templates[mb_lopt_template].platformMat;
                    i++;
                }
            }
            mb_save.tile_count = i;
        }

        bcopy(&mb_default_custom,&mb_save.custom_theme,sizeof(struct MBCustomTheme));
    }

    if (mb_save.version < MB_VERSION) {
        append_puppyprint_log("Performing upgrade from version %d", mb_save.version);
        mb_perform_file_upgrade(&mb_save, &mb_tile_data, &mb_object_data);
    }

    mb_save.author[MAX_USERNAME_SIZE - 1] = '\0'; // memory leak prevention
    mb_tile_count = mb_save.tile_count;
    mb_object_count = mb_save.object_count;

    mb_lopt_costume = mb_save.costume;

    mb_lopt_seq[0] = mb_save.seq[0];
    mb_lopt_seq[1] = mb_save.seq[1];
    mb_lopt_seq[2] = mb_save.seq[2];
    mb_lopt_envfx = mb_save.envfx;
    mb_lopt_theme = mb_save.theme;
    mb_lopt_bg = mb_save.bg;

    mb_lopt_boundary_mat = mb_save.boundary_mat;
    mb_lopt_boundary = mb_save.boundary;
    mb_lopt_boundary_height = mb_save.boundary_height;
    mb_lopt_coinstar = mb_save.coinstar;
    mb_lopt_size = mb_save.size;
    mb_lopt_waterlevel = mb_save.waterlevel;
    mb_lopt_secret = mb_save.secret;

    switch (mb_lopt_size) {
        case 0:
            mb_grid_min = 16;
            mb_grid_size = 32;
            break;
        case 1:
            mb_grid_min = 8;
            mb_grid_size = 48;
            break;
        case 2:
            mb_grid_min = 0;
            mb_grid_size = 64;
            break;
    }

    mb_lopt_game = mb_save.game;

    //copy toolbar
    bcopy(&mb_save.toolbar,&mb_toolbar,sizeof(mb_toolbar));
    bcopy(&mb_save.toolbar_params,&mb_toolbar_params,sizeof(mb_toolbar_params));

    // copy custom theme
    bcopy(&mb_save.custom_theme,&mb_curr_custom_theme,sizeof(struct MBCustomTheme));

    // for (u32 i = 0; i < NUM_MATERIALS_PER_THEME; i++) {
    //     mb_curr_custom_theme.mats[i] = random_float() * MB_MATLIST_END;
    //     mb_curr_custom_theme.topmats[i] = random_float() * MB_MATLIST_END;
    //     mb_curr_custom_theme.topmatsEnabled[i] = 1;
    // }
    update_custom_theme();

    u32 oldIndex = 0;
    bzero(&mb_tile_data_indices,sizeof(mb_tile_data_indices));
    // Load tiles and build index list. Assume all tiles are in order
    for (i = 0; i < mb_tile_count; i++) {
        //bcopy(&mb_save.tiles[i],&mb_tile_data[i],sizeof(mb_tile_data[i]));
        u32 curIndex = get_tiletype_index(mb_tile_data[i].type, mb_tile_data[i].mat);

        if (curIndex != oldIndex) {
            // These tiles do not exist in the level so fill in the indices
            mb_tile_data_indices[oldIndex + 1] = i;
            for (u32 j = oldIndex + 1; j < curIndex; j++) {
                mb_tile_data_indices[j + 1] = mb_tile_data_indices[j];
            }
            oldIndex = curIndex;
        }

        s8 pos[3];
        vec3_set(pos, mb_tile_data[i].x, mb_tile_data[i].y, mb_tile_data[i].z);

        place_terrain_data(pos, mb_tile_data[i].type, mb_tile_data[i].rot, mb_tile_data[i].mat);
        get_grid_tile(pos)->waterlogged = mb_tile_data[i].waterlogged;
    }
    // Fill in remaining indices that were unused
    for (u32 i = oldIndex + 1; i < ARRAY_COUNT(mb_tile_data_indices); i++) {
        mb_tile_data_indices[i] = mb_tile_count;
    }

    mb_trajectories_used = 0;
    for (i = 0; i < mb_object_count; i++) {
        if (mb_object_type_list[mb_object_data[i].type].flags & OBJ_TYPE_TRAJECTORY) {
            mb_trajectories_used++;
        }
    }

    for (i = 0; i < MB_MAX_TRAJECTORIES; i++) {
        for (j = 0; j < MB_TRAJECTORY_LENGTH; j++) {
            mb_trajectory_list[i][j][0] = mb_save.trajectories[i][j].t;
            mb_trajectory_list[i][j][1] = GRID_TO_POS(mb_save.trajectories[i][j].x);
            mb_trajectory_list[i][j][2] = GRID_TO_POS(mb_save.trajectories[i][j].y);
            mb_trajectory_list[i][j][3] = GRID_TO_POS(mb_save.trajectories[i][j].z);
        }
    }
    

    mb_init_toolbox();

    if (!fresh) {
        update_painting();
    }
}

//Initialize the game system
void mb_init() {
    //Load level
    mb_level_load();

    //
    if (gLevelAction != MB_LA_PLAY_LEVELS) {
        vec3_set(mb_cursor_pos, 32, 8, 32);
        mb_camera_foc[0] = GRID_TO_POS(32);
        mb_camera_foc[1] = 0.0f;
        mb_camera_foc[2] = GRID_TO_POS(32);
    }
}

///Initializes
void mb_level_options_init() {

    //Set object vars to zero
    gLevelOptions->costume = 0;

    gLevelOptions->seq[5] = {0,0,0,0,0}; // Song index
    gLevelOptions->seqType = 0; // Level Music, Race Music, Boss Music 
    gLevelOptions->seqAlbum = 0; // Category
    gLevelOptions->seqCategory = 0; // Song index within category
    gLevelOptions->coinStarMax = 0;

    gLevelOptions->envFX = 0;
    gLevelOptions->theme = 0;
    gLevelOptions->bg = 0;
    gLevelOptions->boundaryMaterial = 0;
    gLevelOptions->boundary = 0;
    gLevelOptions->boundaryHeight = 0;
    gLevelOptions->game = MB_GAME_VANILLA;
    gLevelOptions->size = 0;
    gLevelOptions->template = 0;
    gLevelOptions->coinStar = 0;
    gLevelOptions->waterLevel = 0;
    gLevelOptions->secret = 0;
}

/// @brief Reload the background
void mb_background_reload() {
    u8 bg = gLevelOptions->background;

    u8 *srcStart = mb_skybox_table[bg * 2];
    u8 *srcEnd = mb_skybox_table[bg * 2 + 1];

    //Don't count if the skybox doesn't exist
    if (srcStart == NULL) {
        return;
    }

    u32 compSize = ALIGN16(srcEnd - srcStart);
    u8 *compressed = main_pool_alloc(compSize, MEMORY_POOL_RIGHT);

    if (compressed != NULL) {
        dma_read(compressed, srcStart, srcEnd);
        Propack_UnpackM1(compressed, get_segment_base_addr(SEGMENT_SKYBOX));
        sSegmentROMTable[SEGMENT_SKYBOX] = (uintptr_t) srcStart;
        main_pool_free(compressed);
    }

    generate_terrain_gfx(); // since some backgrounds affect the boundary
}

void sb_init(void) {
    struct Object *spawn_obj;

    reload_bg();
    reload_boundary_and_gfx();

    switch(mb_mode) {
        case MB_MODE_MAKE:
            gMenuState = MB_MAKE_MAIN;
            o->header.gfx.node.flags &= ~GRAPH_RENDER_INVISIBLE;
            generate_object_preview();

            //init visual tile bounds
            for (u8 i=0; i<6; i++) {
                mb_boundary_object[i] = spawn_object(o,MODEL_MAKER_BOUNDARY,bhvStaticObject);
            }
            mb_boundary_object[2]->oFaceAngleRoll = -0x4000;
            mb_boundary_object[3]->oFaceAngleRoll = -0x4000;
            mb_boundary_object[4]->oFaceAnglePitch = 0x4000;
            mb_boundary_object[5]->oFaceAnglePitch = 0x4000;

            play_music(SEQ_PLAYER_LEVEL, SEQUENCE_ARGS(4, seq_musicmenu_array[mb_lopt_seq[0]]), 0);
            create_toolbar();
            show_tip();
            create_coord_display();
            break;
        case MB_MODE_PLAY:
            gMenuState = MB_MAKE_PLAY;
            reset_rng();
            gGlobalTimer = 0;
            generate_objects_to_level();
            load_obj_warp_nodes();

            spawn_obj = cur_obj_nearest_object_with_behavior(bhvSpawn);
            if (spawn_obj) {
                if (gLevelAction == MB_LA_BUILD) {
                    gMarioState->pos[0] = (f32)(GRID_TO_POS(mb_cursor_pos[0]));
                    gMarioState->pos[1] = (f32)(GRID_TO_POS(mb_cursor_pos[1]));
                    gMarioState->pos[2] = (f32)(GRID_TO_POS(mb_cursor_pos[2]));
                    set_mario_action(gMarioState,ACT_IDLE,0);
                    gMarioState->faceAngle[1] = mb_rot_selection*0x4000;
                } else {
                    gMarioState->faceAngle[1] = spawn_obj->oFaceAngleYaw;
                    vec3_copy(gMarioState->pos,&spawn_obj->oPosVec);
                    set_mario_action(gMarioState,ACT_SPAWN_SPIN_AIRBORNE,0);
                }
                gMarioState->pos[1] -= TILE_SIZE/2;

                reset_camera(gCurrentArea->camera);

                struct Object *warpobj = cur_obj_nearest_object_with_behavior(bhvSpinAirborneWarp);
                if (warpobj) {
                    vec3_copy(&warpobj->oPosVec,&spawn_obj->oPosVec);
                    warpobj->oPosY -= TILE_SIZE/2;
                }
            }

            o->header.gfx.node.flags |= GRAPH_RENDER_INVISIBLE;

            play_music(SEQ_PLAYER_LEVEL, SEQUENCE_ARGS(4, seq_musicmenu_array[mb_lopt_seq[0]]), 0);

        break;
    }
}

// Keep cursor in bounds
s32 snap_cursor(void) {
    s32 gridmax = mb_grid_min + mb_grid_size - 1;
    s32 gridymax = (mb_curr_boundary & MB_BOUNDARY_CEILING) ? mb_lopt_boundary_height-1 : 63;
    if (mb_cursor_pos[0] < mb_grid_min) {mb_cursor_pos[0] = mb_grid_min; return TRUE;}
    if (mb_cursor_pos[0] > gridmax) {mb_cursor_pos[0] = gridmax; return TRUE;}
    if (mb_cursor_pos[2] < mb_grid_min) {mb_cursor_pos[2] = mb_grid_min; return TRUE;}
    if (mb_cursor_pos[2] > gridmax) {mb_cursor_pos[2] = gridmax; return TRUE;}
    if (mb_cursor_pos[1] < 0) {mb_cursor_pos[1] = 0; return TRUE;}
    if (mb_cursor_pos[1] > gridymax) {mb_cursor_pos[1] = gridymax; return TRUE;}
    return FALSE;
}

// Wrap cursor to other side of level if out of bounds
void wrap_cursor(void) {
    s32 gridmax = mb_grid_min + mb_grid_size - 1;
    s32 gridymax = (mb_curr_boundary & MB_BOUNDARY_CEILING) ? mb_lopt_boundary_height-1 : 63;
    if (mb_cursor_pos[0] < mb_grid_min) {mb_cursor_pos[0] = gridmax; return;}
    if (mb_cursor_pos[0] > gridmax) {mb_cursor_pos[0] = mb_grid_min; return;}
    if (mb_cursor_pos[2] < mb_grid_min) {mb_cursor_pos[2] = gridmax; return;}
    if (mb_cursor_pos[2] > gridmax) {mb_cursor_pos[2] = mb_grid_min; return;}
    if (mb_cursor_pos[1] < 0) {mb_cursor_pos[1] = gridymax; return;}
    if (mb_cursor_pos[1] > gridymax) {mb_cursor_pos[1] = 0; return;}
}

extern void play_sound_cbutton_side(void);

u8 c_button_timer = 0;
u8 cursor_wrap = FALSE;
u8 mb_joystick_timer = 0;
u32 main_cursor_logic() {
    u8 cursorMoved = FALSE;
    int joystick = 0;

    if (mb_joystick_timer > 0) {
        mb_joystick_timer--;
    }

    if ((gPlayer1Controller->rawStickX < 10)&&(gPlayer1Controller->rawStickX > -10)&&(gPlayer1Controller->rawStickY < 10)&&(gPlayer1Controller->rawStickY > -10)) {
        mb_joystick_timer = 0;
    }

    if (mb_joystick_timer == 0) {
        if (gPlayer1Controller->rawStickX > 60) {
            mb_joystick_timer = 5;
            joystick = 3;
        } else if (gPlayer1Controller->rawStickX < -60) {
            mb_joystick_timer = 5;
            joystick = 1;
        } else if (gPlayer1Controller->rawStickY > 60) {
            mb_joystick_timer = 5;
            joystick = 4;
        } else if (gPlayer1Controller->rawStickY < -60) {
            mb_joystick_timer = 5;
            joystick = 2;
        }
    }

    if (sDelayedWarpOp == WARP_OP_NONE) {
        if (joystick != 0) {
            switch(((joystick-1)+mb_camera_rot_offset)%4) {
                case 0:
                    mb_cursor_pos[0]++;
                    cursorMoved = TRUE;
                break;
                case 1:
                    mb_cursor_pos[2]--;
                    cursorMoved = TRUE;
                break;
                case 2:
                    mb_cursor_pos[0]--;
                    cursorMoved = TRUE;
                break;
                case 3:
                    mb_cursor_pos[2]++;
                    cursorMoved = TRUE;
                break;
            }
        }

        c_button_timer ++;
        if (!(gPlayer1Controller->buttonDown & (U_CBUTTONS|D_CBUTTONS))) {
            c_button_timer = 0;
        }

        if ((gPlayer1Controller->buttonDown & U_CBUTTONS)&&(c_button_timer%5 == 1)) {
            mb_cursor_pos[1]++;
            cursorMoved = TRUE;
        }
        if ((gPlayer1Controller->buttonDown & D_CBUTTONS)&&(c_button_timer%5 == 1)) {
            mb_cursor_pos[1]--;
            cursorMoved = TRUE;
        }
    }
    if (gPlayer1Controller->buttonPressed & R_CBUTTONS) {
        mb_camera_rot_offset++;
    }
    if (gPlayer1Controller->buttonPressed & L_CBUTTONS) {
        mb_camera_rot_offset--;
    }
    mb_camera_rot_offset = (mb_camera_rot_offset % 4)+4;

    if (cursorMoved) {
        if (!cursor_wrap) {
           if (snap_cursor()) cursor_wrap = TRUE;
        } else {
            wrap_cursor();
            cursor_wrap = FALSE;
        }
    }

    //camera zooming
    if (gPlayer1Controller->buttonPressed & D_JPAD) {
        mb_camera_zoom_index++;
    }
    mb_camera_zoom_index = (mb_camera_zoom_index+5)%5;

    o->oPosX = GRID_TO_POS(mb_cursor_pos[0]); 
    o->oPosY = GRID_TO_POS(mb_cursor_pos[1]); 
    o->oPosZ = GRID_TO_POS(mb_cursor_pos[2]); 

    return cursorMoved;
}

void update_boundary_wall() {
    for (u8 i=0; i<6; i++) {
        vec3_copy(&mb_boundary_object[i]->oPosVec,&o->oPosVec);
    }
    mb_boundary_object[0]->oPosY = GRID_TO_POS(0);
    mb_boundary_object[1]->oPosY = GRID_TO_POS(64);
    mb_boundary_object[2]->oPosX = GRID_TO_POS(mb_grid_min);
    mb_boundary_object[3]->oPosX = GRID_TO_POS(mb_grid_min + mb_grid_size);
    mb_boundary_object[4]->oPosZ = GRID_TO_POS(mb_grid_min);
    mb_boundary_object[5]->oPosZ = GRID_TO_POS(mb_grid_min + mb_grid_size);

    if (gMenuState == MB_MAKE_SCREENSHOT) {
        for (int i=0; i<6; i++) {
            mb_boundary_object[i]->header.gfx.node.flags |= GRAPH_RENDER_INVISIBLE;
        }
    } else {
        for (int i=0; i<6; i++) {
            mb_boundary_object[i]->header.gfx.node.flags &= ~GRAPH_RENDER_INVISIBLE;
        }
    }
}

void delete_preview_object(void) {
    struct Object *previewObj = cur_obj_nearest_object_with_behavior(bhvCurrPreviewObject);
    while (previewObj) {
        unload_object(previewObj);
        previewObj = cur_obj_nearest_object_with_behavior(bhvCurrPreviewObject);
    }
}

void custom_theme_update() {
    for (u32 i = 0; i < NUM_MATERIALS_PER_THEME; i++) {
        mb_theme_table[MB_THEME_CUSTOM].mats[i].mat = mb_curr_custom_theme.mats[i];
        if (mb_curr_custom_theme.topmatsEnabled[i]) {
            mb_theme_table[MB_THEME_CUSTOM].mats[i].topmat = mb_curr_custom_theme.topmats[i];
        } else {
            mb_theme_table[MB_THEME_CUSTOM].mats[i].topmat = mb_curr_custom_theme.mats[i];
        }
    }
    mb_theme_table[MB_THEME_CUSTOM].fence = mb_curr_custom_theme.fence;
    mb_theme_table[MB_THEME_CUSTOM].pole = mb_curr_custom_theme.pole;
    mb_theme_table[MB_THEME_CUSTOM].bars = mb_curr_custom_theme.bars;
    mb_theme_table[MB_THEME_CUSTOM].water = mb_curr_custom_theme.water;
}


void prepare_block_draw(f32 xpos, f32 ypos) {
    Mat4 mtx1, mtx2;
    Vec3f pos;
    Vec3s rot;

    Mtx *perspMtx = alloc_display_list(sizeof(*perspMtx));
    guFrustum(perspMtx, -SCREEN_WIDTH/2 + xpos, SCREEN_WIDTH/2 + xpos, -SCREEN_HEIGHT/2 - ypos, SCREEN_HEIGHT/2 - ypos, 128, 4000, 0.005f);
    gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(perspMtx), G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);

    Lights1* curLight; //Lights1* curLight = (Lights1*);
    alloc_display_list(sizeof(Lights1));
    extern Lights1 *defaultLight;
    bcopy(&defaultLight, curLight, sizeof(Lights1));

    curLight->l->l.dir[0] = (s8)(globalLightDirection[0]);
    curLight->l->l.dir[1] = (s8)(globalLightDirection[1]);
    curLight->l->l.dir[2] = (s8)(globalLightDirection[2]);

    gSPSetLights1(gDisplayListHead++, (*curLight));

    Mtx *mtx = alloc_display_list(sizeof(*mtx));
    vec3_set(pos, 0, 0, -1500);
    vec3_set(rot, 0, (s16)(0x200*gGlobalTimer), 0);
    mtxf_rotate_zxy_and_translate(mtx1, gVec3fZero, rot);
    vec3_set(rot, 0x1800, 0, 0);
    mtxf_rotate_zxy_and_translate(mtx2, pos, rot);
    mtxf_mul(mtx1, mtx1, mtx2);
    mtxf_to_mtx(mtx, mtx1);
    gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(mtx), G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_PUSH);

    mb_build_collision_type = 0;
    mb_growth_render_type = 0;
    mb_render_culling_off = TRUE;
    mb_curr_mat_has_topside = FALSE;
    mb_use_alt_uvs = FALSE;
    mb_render_flip_normals = FALSE;
}

void finish_block_draw() {
    mb_render_culling_off = FALSE;

    gSPDisplayList(gDisplayListHead++, mb_curr_gfx);
    
    mb_curr_gfx += mb_gfx_index;
    mb_gfx_index = 0;

    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);

    create_dl_ortho_matrix();
}

void custom_theme_draw_block(f32 xpos, f32 ypos, s32 index) {
    xpos = -xpos;
    prepare_block_draw(xpos, ypos);

    s8 pos[3];
    vec3_set(pos,32,32,32);

    Vtx *startVtx = mb_curr_vtx;

    if (index < NUM_MATERIALS_PER_THEME) {
        u8 renderedMat = mb_curr_custom_theme.mats[index];
        u8 renderedTopmat = mb_curr_custom_theme.topmats[index];
        if (!mb_curr_custom_theme.topmatsEnabled[index]) renderedTopmat = renderedMat;

        render_preview_block(renderedMat, renderedTopmat, pos, &mb_terrain_fullblock, 0, PROCESS_TILE_BOTH, TRUE);
    } else {
        mb_use_alt_uvs = TRUE;
        mb_curr_poly_vert_count = 4;
        if (index == 10) { // Poles
            gSPDisplayList(&mb_curr_gfx[mb_gfx_index++], mb_mat_table[mb_curr_custom_theme.pole].gfx);
            set_render_mode( mb_mat_table[mb_curr_custom_theme.pole].type, TRUE);
            mb_growth_render_type = 4; // poles
            process_tile(pos, &mb_terrain_pole, 0);
        } else if (index == 11) { // Fence
            gSPDisplayList(&mb_curr_gfx[mb_gfx_index++], mb_fence_texs[mb_curr_custom_theme.fence]);
            set_render_mode( MAT_CUTOUT, TRUE);
            mb_growth_render_type = 3; // fence
            process_tile(pos, &mb_terrain_fence, 0);
        } else if (index == 12) { // Iron Mesh
            set_render_mode( MAT_CUTOUT, TRUE);
            u8 connections[5] = {1,0,1,0,1};
            gSPDisplayList(&mb_curr_gfx[mb_gfx_index++], mb_bar_texs[mb_curr_custom_theme.bars][1]);
            gSPClearGeometryMode(&mb_curr_gfx[mb_gfx_index++], G_CULL_BACK);
            render_bars_top(pos, connections);
            display_cached_tris();
            gSPSetGeometryMode(&mb_curr_gfx[mb_gfx_index++], G_CULL_BACK);
            gSPDisplayList(&mb_curr_gfx[mb_gfx_index++], mb_bar_texs[mb_curr_custom_theme.bars][0]);
            render_bars_side(pos, connections);
        } else if (index == 13) { // Water
            gSPDisplayList(&mb_curr_gfx[mb_gfx_index++], mb_water_texs[mb_curr_custom_theme.water]);
            set_render_mode( MAT_TRANSPARENT, TRUE);
            render_water(pos);
        }
        display_cached_tris();
        mb_use_alt_uvs = FALSE;
    }

    gDPPipeSync(&mb_curr_gfx[mb_gfx_index++]);
    gDPSetRenderMode(&mb_curr_gfx[mb_gfx_index++], G_RM_AA_ZB_OPA_SURF, G_RM_AA_ZB_OPA_SURF2);
    gDPSetTextureLUT(&mb_curr_gfx[mb_gfx_index++], G_TT_NONE);
    gSPEndDisplayList(&mb_curr_gfx[mb_gfx_index++]);

    for (Vtx *vtx = startVtx; vtx < mb_curr_vtx; vtx++) {
        vtx->v.ob[0] -= TILE_SIZE/2;
        if (index != 11) vtx->v.ob[2] -= TILE_SIZE/2;
    }

    finish_block_draw();
}

void reload_theme(void) {
    reload_boundary_and_gfx();
    generate_object_preview();
}

s16 mb_freecam_pitch;
s16 mb_freecam_yaw;
u8 mb_freecam_snap = FALSE;
u8 mb_freecam_snap_timer = 0;

void freecam_camera_init(void) {
    vec3f_copy(mb_camera_pos_prev,mb_camera_pos);

    Vec3f d;
    vec3_diff(d, mb_camera_pos, mb_camera_foc);
    f32 xz = (sqr(d[0]) + sqr(d[2]));
    mb_freecam_pitch = atan2s(sqrtf(xz), d[1]) + 0x4000;
    mb_freecam_yaw = atan2s(d[2], d[0]);

    mb_freecam_snap = FALSE;
    mb_freecam_snap_timer = 0;
    mb_prepare_level_screenshot = TRUE;
}

void freecam_return(void) {
    gMenuState = MB_MAKE_MAIN;
    mb_camera_fov = 45.0f;
    mb_prepare_level_screenshot = FALSE;
    mb_freecam_snap = FALSE;
    vec3f_copy(mb_camera_pos,mb_camera_pos_prev);
    generate_object_preview();
    show_coord_display();
    destroy_yellow_text();

    show_toolbar();
    toolbar_set_active(TRUE);
}

void freecam_camera_main(void) {
    if (mb_freecam_snap) {
        mb_freecam_snap_timer++;
        if (mb_freecam_snap_timer == 3) {
            play_sound(SOUND_MENU_CLICK_CHANGE_VIEW, gGlobalSoundSource);
            for (u8 i=0; i<16; i++) {
                osRecvMesg(&gGameVblankQueue, &gMainReceivedMesg, OS_MESG_BLOCK);
            }
            save_level();
        }
        if (mb_freecam_snap_timer > 30) {
            freecam_return();
        }
        return;
    }

    // camera controls
    mb_freecam_yaw += gPlayer1Controller->rawStickX*-4;
    mb_freecam_pitch += gPlayer1Controller->rawStickY*-4;

    if (mb_freecam_pitch < 0x100) {
        mb_freecam_pitch = 0x100;
    }
    if (mb_freecam_pitch > 0x7000) {
        mb_freecam_pitch = 0x7000;
    }

    if (gPlayer1Controller->buttonDown & U_CBUTTONS) {
        mb_camera_pos[0] += ( sins(mb_freecam_yaw) * -sins(mb_freecam_pitch) * 45.0f );
        mb_camera_pos[1] += ( coss(mb_freecam_pitch) * 45.0f );
        mb_camera_pos[2] += ( coss(mb_freecam_yaw) * -sins(mb_freecam_pitch) * 45.0f );
    }

    if (gPlayer1Controller->buttonDown & D_CBUTTONS) {
        mb_camera_pos[0] += ( sins(mb_freecam_yaw) * -sins(mb_freecam_pitch) * -45.0f );
        mb_camera_pos[1] += ( coss(mb_freecam_pitch) * -45.0f );
        mb_camera_pos[2] += ( coss(mb_freecam_yaw) * -sins(mb_freecam_pitch) * -45.0f );
    }

    if (gPlayer1Controller->buttonDown & R_CBUTTONS) {
        mb_camera_pos[0] += ( sins(mb_freecam_yaw + 0x4000) * 30.0f );
        mb_camera_pos[2] += ( coss(mb_freecam_yaw + 0x4000) * 30.0f );
    }

    if (gPlayer1Controller->buttonDown & L_CBUTTONS) {
        mb_camera_pos[0] += ( sins(mb_freecam_yaw - 0x4000) * 30.0f );
        mb_camera_pos[2] += ( coss(mb_freecam_yaw - 0x4000) * 30.0f );
    }

    if (gPlayer1Controller->buttonDown & L_TRIG) {
        mb_camera_fov -= 1.0f;
        cur_obj_play_sound_1(SOUND_AIR_AMP_BUZZ);

        if (mb_camera_fov < 1.0f) {
            mb_camera_fov = 1.0f;
        }
    }

    if (gPlayer1Controller->buttonDown & R_TRIG) {
        mb_camera_fov += 1.0f;
        cur_obj_play_sound_1(SOUND_AIR_AMP_BUZZ);

        if (mb_camera_fov > 100.0f) {
            mb_camera_fov = 100.0f;
        }
    }

    if (gPlayer1Controller->buttonPressed & START_BUTTON) {
        mb_freecam_snap = TRUE;
    }

    if (gPlayer1Controller->buttonPressed & B_BUTTON) {
        freecam_return();
    }

    // transform camera
    mb_camera_foc[0] = mb_camera_pos[0] + ( sins(mb_freecam_yaw) * -sins(mb_freecam_pitch) * 100.0f );
    mb_camera_foc[1] = mb_camera_pos[1] + ( coss(mb_freecam_pitch) * 100.0f );
    mb_camera_foc[2] = mb_camera_pos[2] + ( coss(mb_freecam_yaw) * -sins(mb_freecam_pitch) * 100.0f );
}

u8 sPrevPreviewID; // used for resetting preview obj
u8 sPrevPreviewParam;

void sb_loop(void) {
    Vec3f cam_pos_offset = {0.0f,mb_current_camera_zoom[1],0};
    u8 cursorMoved = FALSE;

    if (mb_do_save) {
        mb_do_save = FALSE;
        save_level();
    }

    mb_camera_angle = approach_s16_asymptotic(mb_camera_angle,mb_camera_rot_offset*0x4000,4);
    cam_pos_offset[0] = sins(mb_camera_angle+0x8000)*mb_current_camera_zoom[0];
    cam_pos_offset[2] = coss(mb_camera_angle+0x8000)*mb_current_camera_zoom[0];

    mb_current_camera_zoom[0] = lerp(mb_current_camera_zoom[0], mb_camera_zoom_table[mb_camera_zoom_index][0],0.2f);
    mb_current_camera_zoom[1] = lerp(mb_current_camera_zoom[1], mb_camera_zoom_table[mb_camera_zoom_index][1],0.2f);

    switch(gMenuState) {
        case MB_MAKE_MAIN:
            ListComponent *toolbarlist = get_child(gToolbar);
            if (toolbarlist->base.inactive) {
                break;
            }
            cursorMoved = main_cursor_logic();

            // Update preview object
            if (cursorMoved || (sPrevPreviewID != mb_id_selection) || (sPrevPreviewParam != mb_param_selection)) {
                delete_preview_object();
            }
            sPrevPreviewID = mb_id_selection;
            sPrevPreviewParam = mb_param_selection;

            if (gPlayer1Controller->buttonPressed & Z_TRIG) {
                mb_rot_selection = (mb_rot_selection + 1) % 4;
                delete_preview_object();
            }


            //Single A press
            if (mb_place_mode == MB_PM_ACTION) {
                if (gPlayer1Controller->buttonPressed & (A_BUTTON | START_BUTTON)) {
                    switch (mb_id_selection) {
                        // Begin Test
                        case OBJECT_TYPE_TEST_MARIO:
                            if ((!can_place(mb_cursor_pos, OBJ_OCCUPY_INNER)) && !can_place(mb_cursor_pos, OBJ_OCCUPY_OUTER)) {
                                show_error("Cannot start test here!");
                                break;
                            }
                            if (!gWarpTransition.isActive && sDelayedWarpOp == WARP_OP_NONE) {
                                if (gSDCard) {
                                    save_level();
                                }
                                toolbar_set_active(FALSE);
                                mb_target_mode = MB_MODE_PLAY;
                                reset_play_state();
                                level_trigger_warp(gMarioState, WARP_OP_LOOK_UP);
                                sSourceWarpNodeId = 0x0A;
                                play_sound(SOUND_MENU_STAR_SOUND_LETS_A_GO, gGlobalSoundSource);
                            }
                            break;
                        // Open settings menu
                        case OBJECT_TYPE_SETTINGS:
                            gMenuState = MB_MAKE_SETTINGS;
                            hide_toolbar();
                            hide_coord_display();
                            settings_menu_create();
                            play_sound(SOUND_MENU_CLICK_FILE_SELECT, gGlobalSoundSource);
                            break;
                    }
                }
            } else {
                if (gPlayer1Controller->buttonPressed & A_BUTTON || ((gPlayer1Controller->buttonDown & A_BUTTON) && cursorMoved)) {
                    place_thing_action();
                }
            }

            if (gPlayer1Controller->buttonPressed & B_BUTTON || ((gPlayer1Controller->buttonDown & B_BUTTON) && cursorMoved)) {
                delete_tile_action(mb_cursor_pos);
            }

            if (gPlayer1Controller->buttonPressed & START_BUTTON && (mb_place_mode != MB_PM_ACTION)) {
                gMenuState = MB_MAKE_TOOLBOX;
                play_sound(SOUND_MENU_CLICK_FILE_SELECT, gGlobalSoundSource);
                create_toolbox();
                hide_coord_display();
                delete_preview_object();
            }

            struct Object *spawnobjp = get_spawn_preview_object();
            if (mb_prepare_level_screenshot) {
                o->header.gfx.node.flags |= GRAPH_RENDER_INVISIBLE;
                if (spawnobjp) spawnobjp->header.gfx.node.flags |= GRAPH_RENDER_INVISIBLE;
            } else {
                o->header.gfx.node.flags &= ~GRAPH_RENDER_INVISIBLE;
                if (spawnobjp) spawnobjp->header.gfx.node.flags &= ~GRAPH_RENDER_INVISIBLE;
            }

            update_boundary_wall();
            break;
        case MB_MAKE_TOOLBOX: //MAKE MODE TOOLBOX
            delete_preview_object();
            break;
        case MB_MAKE_TRAJECTORY: //trajectory maker
            delete_preview_object();
            cursorMoved = main_cursor_logic();

            if (cursorMoved) {
                generate_trajectory_gfx();
            }

            if (mb_trajectory_edit_index == 0) {
                // Initial placement on top of the object
                mb_trajectory_list[mb_trajectory_to_edit][0][0] = 0;
                mb_trajectory_list[mb_trajectory_to_edit][0][1] = o->oPosX;
                mb_trajectory_list[mb_trajectory_to_edit][0][2] = o->oPosY;
                mb_trajectory_list[mb_trajectory_to_edit][0][3] = o->oPosZ;
                mb_trajectory_list[mb_trajectory_to_edit][1][0] = -1;
                mb_trajectory_edit_index++; 
            } else {
                if (gPlayer1Controller->buttonPressed & A_BUTTON) {
                    if (mb_trajectory_edit_index == MB_TRAJECTORY_LENGTH - 1) {
                        show_error("Maximum trajectory length reached! (max 50)");
                    // i fucking hate this, worst code ever. this hopefully won't have floating point inaccuracies
                    } else if (mb_trajectory_list[mb_trajectory_to_edit][mb_trajectory_edit_index - 1][1] == o->oPosX
                            && mb_trajectory_list[mb_trajectory_to_edit][mb_trajectory_edit_index - 1][2] == o->oPosY
                            && mb_trajectory_list[mb_trajectory_to_edit][mb_trajectory_edit_index - 1][3] == o->oPosZ) {
                        show_error("");
                    } else {
                        mb_trajectory_list[mb_trajectory_to_edit][mb_trajectory_edit_index][0] = mb_trajectory_edit_index;
                        mb_trajectory_list[mb_trajectory_to_edit][mb_trajectory_edit_index][1] = o->oPosX;
                        mb_trajectory_list[mb_trajectory_to_edit][mb_trajectory_edit_index][2] = o->oPosY;
                        mb_trajectory_list[mb_trajectory_to_edit][mb_trajectory_edit_index][3] = o->oPosZ;
                        mb_trajectory_list[mb_trajectory_to_edit][mb_trajectory_edit_index+1][0] = -1;
                        mb_trajectory_edit_index++;
                        play_place_sound(SOUND_MENU_CLICK_FILE_SELECT | SOUND_VIBRATO);
                        generate_object_preview();
                    }
                } else if (gPlayer1Controller->buttonPressed & B_BUTTON) {
                    if (mb_trajectory_edit_index <= 1) {
                        show_error("Nothing to delete!");
                    } else {
                        mb_trajectory_edit_index--;
                        mb_trajectory_list[mb_trajectory_to_edit][mb_trajectory_edit_index][0] = -1;
                        play_place_sound(SOUND_GENERAL_DOOR_INSERT_KEY | SOUND_VIBRATO);
                        generate_object_preview();
                    }
                }
            }

            if (gPlayer1Controller->buttonPressed & START_BUTTON) {
                if (mb_trajectory_edit_index == 1) {
                    show_error("Trajectory is too short!");
                } else {
                    gMenuState = MB_MAKE_MAIN;
                    generate_object_preview();
                    destroy_yellow_text();
                    toolbar_set_active(TRUE);
                }
            }

            update_boundary_wall();
            break;
        case MB_MAKE_SCREENSHOT:
            o->header.gfx.node.flags |= GRAPH_RENDER_INVISIBLE;
            freecam_camera_main();
            update_boundary_wall();
            break;
    }

    if (gMenuState == MB_MAKE_MAIN || gMenuState == MB_MAKE_SETTINGS || gMenuState == MB_MAKE_TOOLBOX) {
        struct Object *previewObj = cur_obj_nearest_object_with_behavior(bhvCurrPreviewObject);
        if (!previewObj) {
            s8 pos[3];
            vec3_set(pos, mb_cursor_pos[0], mb_cursor_pos[1], mb_cursor_pos[2]);

            if (mb_place_mode != MB_PM_TILE) {
                struct MBObjectInfo *info = &mb_object_type_list[mb_id_selection];
                spawn_preview_object(pos, mb_rot_selection, mb_param_selection, info, bhvCurrPreviewObject);
            } else if (mb_id_selection == TILE_TYPE_CULL) {
                spawn_preview_object(pos, mb_rot_selection, 0, &mb_object_type_list[OBJECT_TYPE_CULL_PREVIEW], bhvCurrPreviewObject);
            }
        }
    }

    if (gMenuState != MB_MAKE_SCREENSHOT) {
        mb_camera_foc[0] = lerp(mb_camera_foc[0], GRID_TO_POS(mb_cursor_pos[0]),  0.2f);
        mb_camera_foc[1] = lerp(mb_camera_foc[1], GRID_TO_POS(mb_cursor_pos[1]), 0.2f);
        mb_camera_foc[2] = lerp(mb_camera_foc[2], GRID_TO_POS(mb_cursor_pos[2]),  0.2f);

        vec3_copy(mb_camera_pos,mb_camera_foc);
        vec3_add(mb_camera_pos,cam_pos_offset);
    }
}

void play_mb_extra_music(u8 index) {
    if (seq_musicmenu_array[mb_lopt_seq[index]] != seq_musicmenu_array[mb_lopt_seq[0]]) {
        play_music(SEQ_PLAYER_LEVEL, SEQUENCE_ARGS(4, seq_musicmenu_array[mb_lopt_seq[index]]), 0);
    }
}

void stop_mb_extra_music(u8 index) {
    if (seq_musicmenu_array[mb_lopt_seq[index]] != seq_musicmenu_array[mb_lopt_seq[0]]) {
        stop_background_music(SEQUENCE_ARGS(4, seq_musicmenu_array[mb_lopt_seq[index]]));
    }
}
