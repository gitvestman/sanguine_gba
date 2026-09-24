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
#include "bn_regular_bg_items_forest_crossing.h"
#include "bn_regular_bg_items_groundskeeper_cottage.h"
#include "bn_regular_bg_items_garden.h"
#include "bn_regular_bg_items_overgrown_pond.h"
#include "bn_regular_bg_items_main_gate.h"
#include "bn_regular_bg_items_kitchen_entrance.h"
#include "bn_regular_bg_items_cemetery.h"
#include "bn_regular_bg_items_mausoleum.h"
#include "bn_regular_bg_items_tombstones.h"
#include "bn_regular_bg_items_cliff_tombs.h"
#include "bn_regular_bg_items_fancy_corridor_01.h"
#include "bn_regular_bg_items_bedchamber_01.h"
#include "bn_regular_bg_items_great_chamber.h"
#include "bn_regular_bg_items_library.h"
#include "bn_regular_bg_items_hall_of_portraits.h"
#include "bn_regular_bg_items_audience_hall.h"
#include "bn_regular_bg_items_treasury.h"
#include "bn_regular_bg_items_bathroom.h"
#include "bn_regular_bg_items_childrens_room.h"
#include "bn_regular_bg_items_stairway_2ndfloor.h"
#include "bn_regular_bg_items_trophy_room.h"
#include "bn_regular_bg_items_guest_bedroom.h"

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
#include "forest_crossing.h"
#include "groundskeeper_cottage.h"
#include "garden.h"
#include "overgrown_pond.h"
#include "main_gate.h"
#include "kitchen_entrance.h"
#include "cemetery.h"
#include "mausoleum.h"
#include "tombstones.h"
#include "cliff_tombs.h"
#include "fancy_corridor_01.h"
#include "bedchamber_01.h"
#include "great_chamber.h"
#include "library.h"
#include "hall_of_portraits.h"
#include "audience_hall.h"
#include "treasury.h"
#include "bathroom.h"
#include "childrens_room.h"
#include "stairway_2ndfloor.h"
#include "trophy_room.h"
#include "guest_bedroom.h"

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
    static rooms::ForestCrossing forest_crossing;
    static rooms::GroundskeeperCottage groundskeeper_cottage;
    static rooms::Garden garden;
    static rooms::OvergrownPond overgrown_pond;
    static rooms::MainGate main_gate;
    static rooms::KitchenEntrance kitchen_entrance;
    static rooms::Cemetery cemetery;
    static rooms::Mausoleum mausoleum;
    static rooms::Tombstones tombstones;
    static rooms::CliffTombs cliff_tombs;
    static rooms::FancyCorridor01 fancy_corridor_01;
    static rooms::BedChamber01 bedchamber_01;
    static rooms::GreatChamber great_chamber;
    static rooms::Library library;
    static rooms::HallOfPortraits hall_of_portraits;
    static rooms::AudienceHall audience_hall;
    static rooms::Treasury treasury;
    static rooms::Bathroom bathroom;
    static rooms::ChildrensRoom childrens_room;
    static rooms::Stairway2ndFloor stairway_2ndfloor;
    static rooms::TrophyRoom trophy_room;
    static rooms::GuestBedroom guest_bedroom;

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

    case RoomId::ForestCrossing:
        return forest_crossing;

    case RoomId::GroundskeeperCottage:
        return groundskeeper_cottage;

    case RoomId::Garden:
        return garden;

    case RoomId::OvergrownPond:
        return overgrown_pond;

    case RoomId::MainGate:
        return main_gate;

    case RoomId::KitchenEntrance:
        return kitchen_entrance;

    case RoomId::Cemetery:
        return cemetery;

    case RoomId::Mausoleum:
        return mausoleum;

    case RoomId::Tombstones:
        return tombstones;

    case RoomId::CliffTombs:
        return cliff_tombs;

    case RoomId::FancyCorridor01:
        return fancy_corridor_01;

    case RoomId::BedChamber01:
        return bedchamber_01;

    case RoomId::GreatChamber:
        return great_chamber;

    case RoomId::Library:
        return library;

    case RoomId::HallOfPortraits:
        return hall_of_portraits;

    case RoomId::AudienceHall:
        return audience_hall;

    case RoomId::Treasury:
        return treasury;

    case RoomId::Bathroom:
        return bathroom;

    case RoomId::ChildrensRoom:
        return childrens_room;

    case RoomId::Stairway2ndFloor:
        return stairway_2ndfloor;

    case RoomId::TrophyRoom:
        return trophy_room;

    case RoomId::GuestBedroom:
        return guest_bedroom;

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

    case RoomId::ForestCrossing:
        return bn::regular_bg_items::forest_crossing;

    case RoomId::GroundskeeperCottage:
        return bn::regular_bg_items::groundskeeper_cottage;

    case RoomId::Garden:
        return bn::regular_bg_items::garden;

    case RoomId::OvergrownPond:
        return bn::regular_bg_items::overgrown_pond;

    case RoomId::MainGate:
        return bn::regular_bg_items::main_gate;

    case RoomId::KitchenEntrance:
        return bn::regular_bg_items::kitchen_entrance;

    case RoomId::Cemetery:
        return bn::regular_bg_items::cemetery;

    case RoomId::Mausoleum:
        return bn::regular_bg_items::mausoleum;

    case RoomId::Tombstones:
        return bn::regular_bg_items::tombstones;

    case RoomId::CliffTombs:
        return bn::regular_bg_items::cliff_tombs;

    case RoomId::FancyCorridor01:
        return bn::regular_bg_items::fancy_corridor_01;

    case RoomId::BedChamber01:
        return bn::regular_bg_items::bedchamber_01;

    case RoomId::GreatChamber:
        return bn::regular_bg_items::great_chamber;

    case RoomId::Library:
        return bn::regular_bg_items::library;

    case RoomId::HallOfPortraits:
        return bn::regular_bg_items::hall_of_portraits;

    case RoomId::AudienceHall:
        return bn::regular_bg_items::audience_hall;

    case RoomId::Treasury:
        return bn::regular_bg_items::treasury;

    case RoomId::Bathroom:
        return bn::regular_bg_items::bathroom;

    case RoomId::ChildrensRoom:
        return bn::regular_bg_items::childrens_room;

    case RoomId::Stairway2ndFloor:
        return bn::regular_bg_items::stairway_2ndfloor;

    case RoomId::TrophyRoom:
        return bn::regular_bg_items::trophy_room;

    case RoomId::GuestBedroom:
        return bn::regular_bg_items::guest_bedroom;

    default:
        return bn::regular_bg_items::cellar_hall;
    }
}

}
