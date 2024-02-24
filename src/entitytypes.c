#include "entitytypes.h"
#include "entity.h"

static MobileProperties *init_player_properties();


void *prop_init_pointer(EntityType type, bool *mobile){
    switch (type){
    case PLAYER:
        *mobile = true;
        return init_player_properties();
        break;
    
    default:
        break;
    }

}

static MobileProperties *init_player_properties(){
    MobileProperties *ptr = malloc(sizeof(MobileProperties));



}
