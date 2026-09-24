#include "kitchen_entrance.h"

#include "../ui/room_screen.h"

namespace sg::rooms
{

namespace
{
    constexpr bn::string_view long_description =
        "A sturdy oak door sits in the stone wall, flanked by moldering crates. A servant's "
        "entrance, by the look of it.";
    constexpr bn::string_view short_description = "The kitchen entrance.";
}

void KitchenEntrance::describe(bn::string<768>& out, bool force_long)
{
    out.append((! force_long && visited) ? short_description : long_description);
    visited = true;
}

void KitchenEntrance::build_choices(bn::ivector<Choice>& out)
{
    out.push_back({ "Look", 0 });
    out.push_back({ "S", 1 });
    out.push_back({ "Use door", 2 });
}

void KitchenEntrance::on_choice(int action_id, RoomScreen& screen)
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
        screen.goto_room(RoomId::GroundskeeperCottage);
        break;

    case 2:
        screen.goto_room(RoomId::Kitchen);
        break;

    default:
        break;
    }
}

}
