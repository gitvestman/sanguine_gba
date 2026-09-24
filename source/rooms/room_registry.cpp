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
#include "bn_regular_bg_items_great_hall.h"
#include "bn_regular_bg_items_courtyard.h"
#include "bn_regular_bg_items_kitchen.h"
#include "bn_regular_bg_items_dinner_hall.h"
#include "bn_regular_bg_items_hall_of_armors.h"
#include "bn_regular_bg_items_music_room.h"
#include "bn_regular_bg_items_chapel.h"
#include "bn_regular_bg_items_storage.h"
#include "bn_regular_bg_items_kitchen_cellar.h"
#include "bn_regular_bg_items_wine_cellar.h"
#include "bn_regular_bg_items_orangery.h"
#include "bn_regular_bg_items_larder.h"
#include "bn_regular_bg_items_stairway_1stfloor.h"

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
#include "great_hall.h"
#include "courtyard.h"
#include "kitchen.h"
#include "dinner_hall.h"
#include "hall_of_armors.h"
#include "music_room.h"
#include "chapel.h"
#include "storage.h"
#include "kitchen_cellar.h"
#include "wine_cellar.h"
#include "orangery.h"
#include "larder.h"
#include "stairway_1stfloor.h"

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
    static rooms::GreatHall great_hall;
    static rooms::Courtyard courtyard;
    static rooms::Kitchen kitchen;
    static rooms::DinnerHall dinner_hall;
    static rooms::HallOfArmors hall_of_armors;
    static rooms::MusicRoom music_room;
    static rooms::Chapel chapel;
    static rooms::Storage storage;
    static rooms::KitchenCellar kitchen_cellar;
    static rooms::WineCellar wine_cellar;
    static rooms::Orangery orangery;
    static rooms::Larder larder;
    static rooms::Stairway1stFloor stairway_1stfloor;

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

    case RoomId::GreatHall:
        return great_hall;

    case RoomId::Courtyard:
        return courtyard;

    case RoomId::Kitchen:
        return kitchen;

    case RoomId::DinnerHall:
        return dinner_hall;

    case RoomId::HallOfArmors:
        return hall_of_armors;

    case RoomId::MusicRoom:
        return music_room;

    case RoomId::Chapel:
        return chapel;

    case RoomId::Storage:
        return storage;

    case RoomId::KitchenCellar:
        return kitchen_cellar;

    case RoomId::WineCellar:
        return wine_cellar;

    case RoomId::Orangery:
        return orangery;

    case RoomId::Larder:
        return larder;

    case RoomId::Stairway1stFloor:
        return stairway_1stfloor;

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

    case RoomId::GreatHall:
        return bn::regular_bg_items::great_hall;

    case RoomId::Courtyard:
        return bn::regular_bg_items::courtyard;

    case RoomId::Kitchen:
        return bn::regular_bg_items::kitchen;

    case RoomId::DinnerHall:
        return bn::regular_bg_items::dinner_hall;

    case RoomId::HallOfArmors:
        return bn::regular_bg_items::hall_of_armors;

    case RoomId::MusicRoom:
        return bn::regular_bg_items::music_room;

    case RoomId::Chapel:
        return bn::regular_bg_items::chapel;

    case RoomId::Storage:
        return bn::regular_bg_items::storage;

    case RoomId::KitchenCellar:
        return bn::regular_bg_items::kitchen_cellar;

    case RoomId::WineCellar:
        return bn::regular_bg_items::wine_cellar;

    case RoomId::Orangery:
        return bn::regular_bg_items::orangery;

    case RoomId::Larder:
        return bn::regular_bg_items::larder;

    case RoomId::Stairway1stFloor:
        return bn::regular_bg_items::stairway_1stfloor;

    default:
        return bn::regular_bg_items::cellar_hall;
    }
}

}
