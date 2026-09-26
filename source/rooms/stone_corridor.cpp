#include "stone_corridor.h"

#include "../ui/room_screen.h"
#include "cellar_hall.h"

namespace sg::rooms
{

namespace
{
    constexpr bn::string_view long_description =
        "A cold stone corridor. Doors line both sides, two east and two west. The north door "
        "has no handle, just a strange metal face.";
    constexpr bn::string_view short_description = "The stone corridor.";
}

void StoneCorridor::describe(bn::string<768>& out, bool force_long)
{
    out.append((! force_long && visited) ? short_description : long_description);
    visited = true;
}

void StoneCorridor::build_choices(bn::ivector<Choice>& out)
{
    out.push_back({ "Look", 0 });

    out.push_back(puzzle_door_open ? Choice{ "N", 10 } : Choice{ "Examine door", 8 });

    if(cellar_hall().north_secret_door_found)
    {
        out.push_back({ "S", 9 });
    }
    else
    {
        out.push_back({ "Search", 7 });
    }

    out.push_back({ "NE", 11 });
    out.push_back({ "E", 12 });
    out.push_back({ "NW", 13 });
    out.push_back({ "W", 14 });
}

void StoneCorridor::on_choice(int action_id, RoomScreen& screen)
{
    switch(action_id)
    {

    case 0:
    {
        bn::string<768> text;
        describe(text, true);
        screen.show_text(text);
        break;
    }

    case 8:
    {
        _examined_puzzle_door = true;
        screen.show_text("Eight zodiac sigils ring a green gemstone. They must be pressed in "
                          "the right order.");

        Result result = screen.roll(Attribute::INT_, 12, true, "Zodiac order");

        if(result == Result::SUCCESS)
        {
            puzzle_door_open = true;
            screen.show_text("The gemstone flares green. The door grinds open to the north.");
        }
        else if(result == Result::FAIL || result == Result::CRITICAL_FAIL)
        {
            screen.show_text("Nothing happens. Perhaps a different order.");
        }

        break;
    }

    case 7:
    {
        Result result = screen.roll(Attribute::INT_, 10, true, "Search");

        if(result == Result::SUCCESS)
        {
            cellar_hall().north_secret_door_found = true;
            screen.show_text("The south wall looks different. You press it and a hidden "
                              "passage swings open.");
        }
        else if(result == Result::FAIL || result == Result::CRITICAL_FAIL)
        {
            screen.show_text("You find nothing out of the ordinary.");
        }

        break;
    }

    case 9:
        screen.goto_room(RoomId::CellarHall);
        break;

    case 10:
        screen.goto_room(RoomId::StairwayBasement);
        break;

    case 11:
        if(item_unlocks_path(ItemId::IRON_KEY))
        {
            screen.goto_room(RoomId::AlchemicalLaboratory);
        }
        else
        {
            screen.show_text("The door is locked. You need a key.");
        }
        break;

    case 12:
        if(item_unlocks_path(ItemId::IRON_KEY))
        {
            screen.goto_room(RoomId::HomunculusWorkshop);
        }
        else
        {
            screen.show_text("The door is locked. You need a key.");
        }
        break;

    case 13:
        if(item_unlocks_path(ItemId::IRON_KEY))
        {
            screen.goto_room(RoomId::ArmoryRoom);
        }
        else
        {
            screen.show_text("The door is locked. You need a key.");
        }
        break;

    case 14:
        if(item_unlocks_path(ItemId::IRON_KEY))
        {
            screen.goto_room(RoomId::TheMachine);
        }
        else
        {
            screen.show_text("The door is locked. You need a key.");
        }
        break;

    default:
        break;
    }
}

void StoneCorridor::build_ambient(bn::ivector<AmbientDecor>& out) const
{
    out.push_back({ AmbientKind::TORCH, -78, -20 });
    out.push_back({ AmbientKind::TORCH, 75, -20 });
}

uint16_t StoneCorridor::save_state() const
{
    uint16_t bits = Room::save_state();
    if(puzzle_door_open) bits |= 1 << 1;
    if(_examined_puzzle_door) bits |= 1 << 2;
    return bits;
}

void StoneCorridor::load_state(uint16_t bits)
{
    Room::load_state(bits);
    puzzle_door_open = bits & (1 << 1);
    _examined_puzzle_door = bits & (1 << 2);
}

}
