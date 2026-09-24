#include "fancy_corridor_01.h"

#include "../ui/room_screen.h"

namespace sg::rooms
{

namespace
{
    constexpr bn::string_view long_description =
        "A fancy corridor lined with faded tapestries. Doors lead in every direction.";
    constexpr bn::string_view short_description = "The fancy corridor.";
}

void FancyCorridor01::describe(bn::string<768>& out, bool force_long)
{
    out.append((! force_long && visited) ? short_description : long_description);
    visited = true;
}

void FancyCorridor01::build_choices(bn::ivector<Choice>& out)
{
    out.push_back({ "Look", 0 });
    out.push_back({ "N", 1 });
    out.push_back({ "NW", 2 });
    out.push_back({ "SW", 3 });
    out.push_back({ "S", 4 });
    out.push_back({ "SE", 5 });
    out.push_back({ "E", 6 });
    out.push_back({ "NE", 7 });
}

void FancyCorridor01::on_choice(int action_id, RoomScreen& screen)
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
        screen.show_text("The passage further north isn't open in this build yet.");
        break;

    case 2:
        screen.goto_room(RoomId::Bathroom);
        break;

    case 3:
        screen.goto_room(RoomId::ChildrensRoom);
        break;

    case 4:
        screen.goto_room(RoomId::AudienceHall);
        break;

    case 5:
        screen.goto_room(RoomId::BedChamber01);
        break;

    case 6:
        screen.goto_room(RoomId::GreatChamber);
        break;

    case 7:
        screen.goto_room(RoomId::GuestBedroom);
        break;

    default:
        break;
    }
}

}
