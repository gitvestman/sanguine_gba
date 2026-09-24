#include "stairway_1stfloor.h"

#include "../ui/room_screen.h"

namespace sg::rooms
{

namespace
{
    constexpr bn::string_view long_description =
        "The castle's grand central stairway, worn smooth by countless footsteps. Paths lead "
        "up, down, and south.";
    constexpr bn::string_view short_description = "The grand stairway.";
}

void Stairway1stFloor::describe(bn::string<768>& out, bool force_long)
{
    out.append((! force_long && visited) ? short_description : long_description);
    visited = true;
}

void Stairway1stFloor::build_choices(bn::ivector<Choice>& out)
{
    out.push_back({ "Look", 0 });
    out.push_back({ "Up", 1 });
    out.push_back({ "Down", 2 });
    out.push_back({ "S", 3 });

    if(! _searched)
    {
        out.push_back({ "Search", 4 });
    }
}

void Stairway1stFloor::on_choice(int action_id, RoomScreen& screen)
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

    case 1:
        screen.goto_room(RoomId::Stairway2ndFloor);
        break;

    case 2:
        screen.goto_room(RoomId::StairwayBasement);
        break;

    case 3:
        screen.goto_room(RoomId::GreatHall);
        break;

    case 4:
        _searched = true;
        screen.show_text("Dust and silence. Nothing worth taking.");
        break;

    default:
        break;
    }
}

uint16_t Stairway1stFloor::save_state() const
{
    uint16_t bits = Room::save_state();
    if(_searched) bits |= 1 << 1;
    return bits;
}

void Stairway1stFloor::load_state(uint16_t bits)
{
    Room::load_state(bits);
    _searched = bits & (1 << 1);
}

}
