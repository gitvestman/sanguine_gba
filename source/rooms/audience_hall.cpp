#include "audience_hall.h"

#include "../ui/room_screen.h"

namespace sg::rooms
{

namespace
{
    constexpr bn::string_view long_description =
        "A vast, freezing audience hall, its throne draped in cobwebs at the far end.";
    constexpr bn::string_view short_description = "The audience hall.";
}

void AudienceHall::describe(bn::string<768>& out, bool force_long)
{
    out.append((! force_long && visited) ? short_description : long_description);
    visited = true;
}

void AudienceHall::build_choices(bn::ivector<Choice>& out)
{
    out.push_back({ "Look", 0 });
    out.push_back({ "N", 1 });
    out.push_back({ "E", 2 });
    out.push_back({ "W", 3 });
    out.push_back({ "S", 4 });

    if(! _found_hole)
    {
        out.push_back({ "Search", 5 });
    }
    else if(! _switch_triggered && ! _finger_attempted)
    {
        out.push_back({ "Insert finger into hole", 6 });
    }
}

void AudienceHall::on_choice(int action_id, RoomScreen& screen)
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
        screen.goto_room(RoomId::FancyCorridor01);
        break;

    case 2:
        screen.goto_room(RoomId::HallOfPortraits);
        break;

    case 3:
        screen.goto_room(RoomId::TrophyRoom);
        break;

    case 4:
        screen.goto_room(RoomId::Stairway2ndFloor);
        break;

    case 5:
        _found_hole = true;
        screen.show_text("Behind the throne, you find a small hole carved into the "
                          "armrest, just wide enough for a finger.");
        break;

    case 6:
    {
        _finger_attempted = true;
        Result result = screen.roll(Attribute::STR, 10, false, "Insert finger");

        if(result == Result::SUCCESS)
        {
            _switch_triggered = true;
            screen.show_text("Something inside clicks. A distant grinding of stone answers "
                              "from elsewhere.");
        }
        else
        {
            screen.show_text("Nothing happens, no matter how you wiggle your finger.");
        }

        break;
    }

    default:
        break;
    }
}

void AudienceHall::build_ambient(bn::ivector<AmbientDecor>& out) const
{
    out.push_back({ AmbientKind::CANDLE, -78, -30 });
    out.push_back({ AmbientKind::CANDLE, -68, -30 });
    out.push_back({ AmbientKind::CANDLE, -58, -30 });
    out.push_back({ AmbientKind::CANDLE, 58, -30 });
    out.push_back({ AmbientKind::CANDLE, 68, -30 });
    out.push_back({ AmbientKind::CANDLE, 78, -30 });
}

uint16_t AudienceHall::save_state() const
{
    uint16_t bits = Room::save_state();
    if(_found_hole) bits |= 1 << 1;
    if(_finger_attempted) bits |= 1 << 2;
    if(_switch_triggered) bits |= 1 << 3;
    return bits;
}

void AudienceHall::load_state(uint16_t bits)
{
    Room::load_state(bits);
    _found_hole = bits & (1 << 1);
    _finger_attempted = bits & (1 << 2);
    _switch_triggered = bits & (1 << 3);
}

}
