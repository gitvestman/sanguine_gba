#include "groundskeeper_cottage.h"

#include "../ui/room_screen.h"

namespace sg::rooms
{

namespace
{
    constexpr bn::string_view long_description =
        "A small rustic cottage, candlelight glowing through its windows. The road splits "
        "west, east and north.";
    constexpr bn::string_view short_description = "The groundskeeper's cottage.";
}

void GroundskeeperCottage::describe(bn::string<768>& out, bool force_long)
{
    out.append((! force_long && visited) ? short_description : long_description);
    visited = true;
}

void GroundskeeperCottage::build_choices(bn::ivector<Choice>& out)
{
    out.push_back({ "Look", 0 });
    out.push_back({ "N", 1 });
    out.push_back({ "E", 2 });
    out.push_back({ "W", 3 });
    out.push_back({ "Talk to groundskeeper", 4 });
}

void GroundskeeperCottage::on_choice(int action_id, RoomScreen& screen)
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
        screen.goto_room(RoomId::KitchenEntrance);
        break;

    case 2:
        screen.goto_room(RoomId::ForestCrossing);
        break;

    case 3:
        screen.goto_room(RoomId::Garden);
        break;

    case 4:
        screen.show_text("The groundskeeper mutters about lost tools and locked gates, then "
                          "falls silent again.");
        break;

    default:
        break;
    }
}

}
