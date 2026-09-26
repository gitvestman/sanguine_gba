#include "great_hall.h"

#include "../ui/room_screen.h"
#include "hall_of_armors.h"

namespace sg::rooms
{

namespace
{
    constexpr bn::string_view long_description =
        "A vast entrance hall, its grandeur faded under dust and cobwebs. Doors lead north, "
        "northwest and south; a stairway northeast.";
    constexpr bn::string_view short_description = "The great hall.";
}

void GreatHall::describe(bn::string<768>& out, bool force_long)
{
    out.append((! force_long && visited) ? short_description : long_description);
    visited = true;
}

void GreatHall::build_choices(bn::ivector<Choice>& out)
{
    out.push_back({ "Look", 0 });
    out.push_back({ "NW", 1 });
    out.push_back({ "N", 2 });
    out.push_back({ "NE", 3 });
    out.push_back({ "W", 4 });
    out.push_back({ "S", 5 });
}

void GreatHall::on_choice(int action_id, RoomScreen& screen)
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
        screen.goto_room(RoomId::DinnerHall);
        break;

    case 2:
        static_cast<HallOfArmors&>(room(RoomId::HallOfArmors)).enter_from(2);
        screen.goto_room(RoomId::HallOfArmors);
        break;

    case 3:
        screen.goto_room(RoomId::Stairway1stFloor);
        break;

    case 4:
        screen.goto_room(RoomId::Orangery);
        break;

    case 5:
        screen.goto_room(RoomId::Courtyard);
        break;

    default:
        break;
    }
}

void GreatHall::build_ambient(bn::ivector<AmbientDecor>& out) const
{
    out.push_back({ AmbientKind::CANDLE, -65, -25 });
    out.push_back({ AmbientKind::CANDLE, 65, -25 });
}

}
