#include "stairway_2ndfloor.h"

#include "../ui/room_screen.h"
#include "../engine/inventory.h"

namespace sg::rooms
{

namespace
{
    constexpr bn::string_view long_description =
        "The grand stairway continues here, alcoves lining the walls like silent sentinels.";
    constexpr bn::string_view short_description = "The stairway.";
}

void Stairway2ndFloor::describe(bn::string<768>& out, bool force_long)
{
    out.append((! force_long && visited) ? short_description : long_description);
    visited = true;
}

void Stairway2ndFloor::build_choices(bn::ivector<Choice>& out)
{
    out.push_back({ "Look", 0 });
    out.push_back({ "Up", 1 });
    out.push_back({ "Down", 2 });
    out.push_back({ "N", 3 });

    if(_figurine_found && ! _figurine_taken)
    {
        out.push_back({ "Take figurine", 4 });
    }

    if(! _searched)
    {
        out.push_back({ "Search", 5 });
    }
}

void Stairway2ndFloor::on_choice(int action_id, RoomScreen& screen)
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
        screen.show_text("The stairs continue up, further than this build reaches yet.");
        break;

    case 2:
        screen.goto_room(RoomId::Stairway1stFloor);
        break;

    case 3:
        screen.goto_room(RoomId::AudienceHall);
        break;

    case 4:
        Inventory::instance().add(ItemId::BASTET_FIGURINE);
        _figurine_taken = true;
        screen.show_text("You take the small bronze figurine of a cat-headed goddess.");
        break;

    case 5:
        _searched = true;

        if(_figurine_found)
        {
            screen.show_text("You've already found everything of note here.");
        }
        else
        {
            Result result = screen.roll(Attribute::LUCK, 10, true, "Search");

            if(result == Result::SUCCESS)
            {
                _figurine_found = true;
                screen.show_text("Wedged in an alcove, half-hidden behind a bust, you spot "
                                  "a small bronze figurine.");
            }
            else if(result == Result::FAIL || result == Result::CRITICAL_FAIL)
            {
                screen.show_text("You find nothing out of the ordinary.");
            }
        }

        break;

    default:
        break;
    }
}

uint16_t Stairway2ndFloor::save_state() const
{
    uint16_t bits = Room::save_state();
    if(_searched) bits |= 1 << 1;
    if(_figurine_found) bits |= 1 << 2;
    if(_figurine_taken) bits |= 1 << 3;
    return bits;
}

void Stairway2ndFloor::load_state(uint16_t bits)
{
    Room::load_state(bits);
    _searched = bits & (1 << 1);
    _figurine_found = bits & (1 << 2);
    _figurine_taken = bits & (1 << 3);
}

}
