#pragma once

#ifndef MB_COMPATIBILITY_H
#define MB_COMPATIBILITY_H

// Type includes
#include "mb_types.h"

// Version 1.0 definitions
// These can be updated to legacy structs whenever the memory layout changes.
#define Version_1_0_Save struct MBLevelSaveHeader
#define Version_1_0_Obj struct MBObject
#define Version_1_0_Tile struct MBTile

void mb_upgrade_to_1_1(Version_1_0_Save *save, Version_1_0_Tile *tile_data, Version_1_0_Obj *obj_data);
void mb_perform_file_upgrade(struct MBLevelSaveHeader *save, void *tile_data, void *obj_data);

#endif