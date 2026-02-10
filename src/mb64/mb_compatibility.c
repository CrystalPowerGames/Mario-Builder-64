#include "mb_main.h"
#include "mb_compatibility.h"

#include "game/puppyprint.h"

// Upgrades the current version to 1.1
// TODO: Remove later once a better function is created (one that upgrades to any version specified)
// In the future we can have a lot of helper functions to assist with version reordering
void mb_upgrade_to_1_1(Version_1_0_Save *save, Version_1_0_Tile *tile_data, Version_1_0_Obj *obj_data) {
    //Puppyprint log
    append_puppyprint_log("Upgrading save file from version 1.0 to 1.1");

    //Objects
    for (s32 i = 0; i < save->object_count; i++) {
        //Switch case because the old syntax sucked
        switch (obj_data[i].type)
        {
            case 92: //Conveyor
                obj_data[i].bparam = 1; //Full block conveyor
            break;
            case 70: //Moneybag
                obj_data[i].imbue = IMBUE_THREE_COINS; //Imbuing changes
            break;
            case 58: //Boo
                obj_data[i].imbue = IMBUE_ONE_COIN; //Imbuing changes
            break;
            case 35: //Badge
                //Removes the star radar badge
                if (obj_data[i].bparam == 11) {
                    obj_data[i].type = OBJECT_TYPE_GREEN_COIN; // Replace with a green coin
                    obj_data[i].bparam = 0;
                } else if (obj_data[i].bparam > 11) {
                    obj_data[i].bparam--;
                }
            break;
        }
    }

    //Tiles
    for (s32 i = 0; i < save->tile_count; i++) {
        if (tile_data[i].type >= 12) { // 1.0 ID - Upper Gentle Slope
            tile_data[i].type += 2; // Addition of two new tile shapes
        }
    }
}


void mb_perform_file_upgrade(struct MBLevelSaveHeader *save, void *tile_data, void *obj_data) {
    if (save->version < 1) {
        mb_upgrade_to_1_1(save, tile_data, obj_data);
        save->version = 1;
    }
    // more fields can be added in later updates...
}
