#include "trophy_room.h"

#include "../ui/room_screen.h"
#include "audience_hall.h"

namespace sg::rooms
{

namespace
{
    constexpr bn::string_view long_description =
        "Mounted heads and antlers line the walls, a hunter's grim collection frozen in "
        "time.";
    constexpr bn::string_view short_description = "The trophy room.";
}

void TrophyRoom::describe(bn::string<768>& out, bool force_long)
{
    out.append((! force_long && visited) ? short_description : long_description);
    visited = true;
}

void TrophyRoom::build_choices(bn::ivector<Choice>& out)
{
    out.push_back({ "Look", 0 });
    out.push_back({ "E", 1 });

    if(_found_secret_door)
    {
        out.push_back({ "N", 2 });
    }

    if(! _found_secret_door && ! _search_attempted)
    {
        out.push_back({ "Search", 3 });
    }
    else if(! _found_secret_door && _tried_antlers &&
            static_cast<AudienceHall&>(room(RoomId::AudienceHall)).switch_triggered())
    {
        out.push_back({ "Search", 4 });
    }
}

void TrophyRoom::on_choice(int action_id, RoomScreen& screen)
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
        screen.goto_room(RoomId::AudienceHall);
        break;

    case 2:
        screen.goto_room(RoomId::Treasury);
        break;

    case 3:
    {
        _search_attempted = true;
        Result result = screen.roll(Attribute::INT_, 10, false, "Search");

        if(result == Result::SUCCESS)
        {
            if(static_cast<AudienceHall&>(room(RoomId::AudienceHall)).switch_triggered())
            {
                _found_secret_door = true;
                screen.show_text("A set of antlers swings loose at your touch, hinged like a "
                                  "lever. A section of wall grinds open to the north.");
            }
            else
            {
                _tried_antlers = true;
                screen.show_text("One set of antlers wobbles slightly at your touch, as if "
                                  "hinged — but nothing else happens.");
            }
        }
        else
        {
            screen.show_text("Just old trophies, dusty and still.");
        }

        break;
    }

    case 4:
        _found_secret_door = true;
        screen.show_text("This time, the antlers give way. A section of wall grinds open to "
                          "the north.");
        break;

    default:
        break;
    }
}

uint16_t TrophyRoom::save_state() const
{
    uint16_t bits = Room::save_state();
    if(_search_attempted) bits |= 1 << 1;
    if(_tried_antlers) bits |= 1 << 2;
    if(_found_secret_door) bits |= 1 << 3;
    return bits;
}

void TrophyRoom::load_state(uint16_t bits)
{
    Room::load_state(bits);
    _search_attempted = bits & (1 << 1);
    _tried_antlers = bits & (1 << 2);
    _found_secret_door = bits & (1 << 3);
}

}
