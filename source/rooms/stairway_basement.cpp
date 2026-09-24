#include "stairway_basement.h"

#include "../ui/room_screen.h"
#include "stone_corridor.h"

namespace sg::rooms
{

namespace
{
    constexpr bn::string_view long_description =
        "A cold, damp stairway into the castle's underbelly. Torchlight struggles against the "
        "dark.";
    constexpr bn::string_view short_description = "The basement stairway.";
}

void StairwayBasement::describe(bn::string<768>& out, bool force_long)
{
    out.append((! force_long && visited) ? short_description : long_description);
    visited = true;
}

void StairwayBasement::build_choices(bn::ivector<Choice>& out)
{
    out.push_back({ "Look", 0 });
    out.push_back({ "Up", 4 });

    out.push_back(static_cast<StoneCorridor&>(room(RoomId::StoneCorridor)).puzzle_door_open
                       ? Choice{ "S", 2 }
                       : Choice{ "Inspect door", 3 });

    out.push_back({ "Search", 1 });
}

void StairwayBasement::on_choice(int action_id, RoomScreen& screen)
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
        screen.show_text("You find nothing.");
        break;

    case 2:
        screen.goto_room(RoomId::StoneCorridor);
        break;

    case 3:
        screen.show_text("A heavy door blocks the way south. It can only open from the "
                          "other side.");
        break;

    case 4:
        screen.goto_room(RoomId::Stairway1stFloor);
        break;

    default:
        break;
    }
}

}
