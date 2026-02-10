#pragma once

#ifndef MB_TYPES_H
#define MB_TYPES_H

#define NUM_MATERIALS_PER_THEME 10

// Terrain polygon
struct MBTerrainPoly {
    s8 vtx[4][3];
    u8 faceDir;
    u8 faceshape;
    u8 growthType;
    s8 (*altuvs)[4][2];
};

// Boundary quad
struct MBBoundaryQuad {
    s8 vtx[4][3];
    s8 u[2];
    s8 v[2];
    u8 uYScale; // Scale U by Y instead of width
    u8 vYScale; // Scale V by Y instead of width
    u8 flipUvs;
};

// Terrain
struct MBTerrain {
    u8 numQuads;
    u8 numTris;
    struct MBTerrainPoly *quads;
    struct MBTerrainPoly *tris;
};

// Tile
struct MBTile {
    u32 x : 6, y : 6, z : 6, type : 5, mat : 4, rot : 2, waterlogged : 1;
};

// Object
struct MBObject {
    u8 bparam;
    u8 x;
    u8 y;
    u8 z;
    u8 type;
    u8 rot;
    u8 imbue;
    u8 pad;
};

// Grid Object
struct MBGridObject {
    u16 type : 5;
    u16 mat : 4;
    u16 rot : 2;
    u16 waterlogged : 1;
};

// Display Function pointer
typedef void (*DisplayFunc)(s32);

// Object Infofmation
struct MBObjectInfo {
    char *name;
    Gfx *btn;
    const BehaviorScript *behavior;
    f32 yOffset;
    u16 modelId;
    u8 flags;
    u8 occupy;
    u8 numCoins;
    u8 numExtraObjects;
    f32 scale;
    const struct Animation *const *anim;
    DisplayFunc dispFunc;
    u32 soundBits;
};

// Types of materials
enum MBMaterialTypes {
    MAT_OPAQUE,        // Opaque
    MAT_DECAL,         // Opaque; only used for VP screen when used as a block type
    MAT_CUTOUT,        // Transparent
    MAT_CUTOUT_NOCULL, // Transparent
    MAT_TRANSPARENT,   // Transparent
    MAT_SCREEN,        // Used for override when processing vplex screens
};

// Represents material texture and collision
struct MBMaterial {
    Gfx *gfx;
    u8 type;
    u8 vertical;
    TerrainData col;
    char *name; // Only used for Custom Theme menu
};

// Represents a material as a top texture with optional side decal
struct MBMaterialTop {
    u8 mat;
    Gfx *decaltex;
};

// Defines materials of a full block
struct MBTilemapDef {
    u8 mat;
    u8 topmat;
    char *name;
};

// Theme
struct MBTheme {
    struct MBTilemapDef mats[NUM_MATERIALS_PER_THEME];
    u8 fence;
    u8 pole;
    u8 bars;
    u8 water;
};

// Terrain info
struct MBTerrainInfo {
    char *name;
    Gfx *button;
    struct MBTerrain *terrain;
};

// Template
struct MBTemplate {
    u8 music[2]; // Vanilla, btcm
    u32 envfx : 3;
    u32 bg : 4;
    u32 theme : 4;
    u32 boundaryMat : 4;
    u32 boundaryHeight : 6;
    u32 boundary : 3;
    u32 water : 6;

    u32 spawnHeight : 6;
    u32 platform : 1; // spawns 3 blocks below spawn height
    u32 platformMat : 4;
};

//Button settings
struct MBSettingsButton {
    char *str;
    u8 *value;
    char **nametable;
    u8 size;
    char *(*nameFunc)(s32, char *);
    void (*changedFunc)(void);
};

//UI button
struct MBUIButton {
    u32 placeMode : 2;
    u32 multiObj : 1;
    u32 paramCount : 8;

    union {
        u32 id;
        u8 *idList;
    };
    union {
        char *name;
        char **names;
    };
};

//Dialog topic
struct MBDialogTopic {
    char *name;
    u8 dialogId;
};

//Dialog subject
struct MBDialogSubject {
    char *name;
    struct MBDialogTopic *topicList;
    u8 topicListSize;
};

// Instructions for when the object is defeated
struct MBImbueModel {
    s16 model;
    u8 billboarded : 1;
    u8 doShrink : 1;
    u8 doMove : 1;
    f32 scale;
    s16 spin;
};

// The objects/model info for when the object is defeated.
struct MBImbueData {
    u32 coins;
    u32 model;
    u32 color;
};

// Contents of an Exclaimation box
struct MBExclamationBoxContents {
    u8 behParams;
    ModelID16 model;
    const BehaviorScript *behavior;
    u8 animState;
    u8 doRespawn;
    u8 numCoins;
};

#endif