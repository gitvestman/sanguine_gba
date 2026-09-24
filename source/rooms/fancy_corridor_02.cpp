#include "fancy_corridor_02.h"

#include "../ui/room_screen.h"

namespace sg::rooms
{

namespace
{
    constexpr bn::string_view long_description =
        "The topmost corridor, its ceiling lost in shadow far overhead. Doors lead in every "
        "direction.";
    constexpr bn::string_view short_description = "The third floor corridor.";
}

void FancyCorridor02::describe(bn::string<768>& out, bool force_long)
{
    out.append((! force_long && visited) ? short_description : long_description);
    visited = true;
}

void FancyCorridor02::build_choices(bn::ivector<Choice>& out)
{
    out.push_back({ "Look", 0 });
    out.push_back({ "N", 1 });
    out.push_back({ "NW", 2 });
    out.push_back({ "W", 3 });
    out.push_back({ "S", 4 });
    out.push_back({ "NE", 5 });
    out.push_back({ "E", 6 });
}

void FancyCorridor02::on_choice(int action_id, RoomScreen& screen)
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
        screen.goto_room(RoomId::Observatory);
        break;

    case 2:
        screen.goto_room(RoomId::Skybridge);
        break;

    case 3:
        screen.goto_room(RoomId::ToymakersWorkshop);
        break;

    case 4:
        screen.goto_room(RoomId::FancyCorridor01);
        break;

    case 5:
        screen.goto_room(RoomId::Reliquary);
        break;

    case 6:
        screen.goto_room(RoomId::DrawingRoom);
        break;

    default:
        break;
    }
}

}
