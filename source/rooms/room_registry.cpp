#include "../engine/room.h"

#include "bn_regular_bg_items_cellar_hall.h"
#include "bn_regular_bg_items_ossuary.h"
#include "bn_regular_bg_items_dungeon.h"
#include "bn_regular_bg_items_torture_chamber.h"
#include "bn_regular_bg_items_stone_corridor.h"
#include "bn_regular_bg_items_armory.h"
#include "bn_regular_bg_items_alchemical_laboratory.h"
#include "bn_regular_bg_items_stairway_basement.h"
#include "bn_regular_bg_items_the_machine.h"
#include "bn_regular_bg_items_homunculus_workshop.h"

#include "cellar_hall.h"
#include "ossuary.h"
#include "dungeon.h"
#include "torture_chamber.h"
#include "stone_corridor.h"
#include "armory_room.h"
#include "alchemical_laboratory.h"
#include "stairway_basement.h"
#include "the_machine.h"
#include "homunculus_workshop.h"

// Wires up RoomId <-> Room instance <-> converted background asset. Each
// room is a function-local static singleton (Meyer's singleton), so
// cross-room state reads (e.g. StoneCorridor reading CellarHall's
// north_secret_door_found) just go through room()/the typed accessors in
// each room's own header.
namespace sg
{

Room& room(RoomId id)
{
    static rooms::CellarHall cellar_hall;
    static rooms::Ossuary ossuary;
    static rooms::Dungeon dungeon;
    static rooms::TortureChamber torture_chamber;
    static rooms::StoneCorridor stone_corridor;
    static rooms::ArmoryRoom armory_room;
    static rooms::AlchemicalLaboratory alchemical_laboratory;
    static rooms::StairwayBasement stairway_basement;
    static rooms::TheMachine the_machine;
    static rooms::HomunculusWorkshop homunculus_workshop;

    switch(id)
    {

    case RoomId::CellarHall:
        return cellar_hall;

    case RoomId::Ossuary:
        return ossuary;

    case RoomId::Dungeon:
        return dungeon;

    case RoomId::TortureChamber:
        return torture_chamber;

    case RoomId::StoneCorridor:
        return stone_corridor;

    case RoomId::ArmoryRoom:
        return armory_room;

    case RoomId::AlchemicalLaboratory:
        return alchemical_laboratory;

    case RoomId::StairwayBasement:
        return stairway_basement;

    case RoomId::TheMachine:
        return the_machine;

    case RoomId::HomunculusWorkshop:
        return homunculus_workshop;

    default:
        return cellar_hall;
    }
}

const bn::regular_bg_item& background_item(RoomId id)
{
    switch(id)
    {

    case RoomId::CellarHall:
        return bn::regular_bg_items::cellar_hall;

    case RoomId::Ossuary:
        return bn::regular_bg_items::ossuary;

    case RoomId::Dungeon:
        return bn::regular_bg_items::dungeon;

    case RoomId::TortureChamber:
        return bn::regular_bg_items::torture_chamber;

    case RoomId::StoneCorridor:
        return bn::regular_bg_items::stone_corridor;

    case RoomId::ArmoryRoom:
        return bn::regular_bg_items::armory;

    case RoomId::AlchemicalLaboratory:
        return bn::regular_bg_items::alchemical_laboratory;

    case RoomId::StairwayBasement:
        return bn::regular_bg_items::stairway_basement;

    case RoomId::TheMachine:
        return bn::regular_bg_items::the_machine;

    case RoomId::HomunculusWorkshop:
        return bn::regular_bg_items::homunculus_workshop;

    default:
        return bn::regular_bg_items::cellar_hall;
    }
}

}
