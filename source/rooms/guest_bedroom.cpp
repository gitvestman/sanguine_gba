#include "guest_bedroom.h"

#include "../ui/room_screen.h"

namespace sg::rooms
{

namespace
{
    constexpr bn::string_view long_description =
        "A lavish guest bedroom, its heavy curtains drawn tight against the moonlight.";
    constexpr bn::string_view short_description = "The guest bedroom.";
}

void GuestBedroom::describe(bn::string<768>& out, bool force_long)
{
    out.append((! force_long && visited) ? short_description : long_description);
    visited = true;
}

void GuestBedroom::build_choices(bn::ivector<Choice>& out)
{
    out.push_back({ "Look", 0 });
    out.push_back({ "W", 1 });
    out.push_back({ "Inspect footprints", 2 });
}

void GuestBedroom::on_choice(int action_id, RoomScreen& screen)
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
        screen.show_text("Faint, bare footprints track across the dusty floor toward the "
                          "curtains, and no further.");
        break;

    default:
        break;
    }
}

void GuestBedroom::build_ambient(bn::ivector<AmbientDecor>& out) const
{
    out.push_back({ AmbientKind::CANDLE, 64, -24 });
}

}
