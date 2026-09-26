#include "guest_bedroom.h"

#include "../ui/room_screen.h"
#include "../engine/combat.h"
#include "fancy_corridor_01.h"

namespace sg::rooms
{

namespace
{
    constexpr bn::string_view long_description =
        "A lavish guest bedroom, its heavy curtains drawn tight against the moonlight.";
    constexpr bn::string_view short_description = "The guest bedroom.";
    constexpr bn::string_view hiding_blurb =
        "\n\nA faint trail of dark droplets leads from the doorway to the bed. The heavy "
        "curtains are drawn shut, but you can see a pale hand resting on the edge. The scent "
        "of old roses and something metallic fills the room. She is here.";

    dialogue::Countess& countess()
    {
        return static_cast<FancyCorridor01&>(room(RoomId::FancyCorridor01)).countess();
    }

    bool countess_hiding()
    {
        return countess().hiding_in_guest_bedroom() && ! enemy(EnemyId::Countess).is_dead();
    }
}

void GuestBedroom::describe(bn::string<768>& out, bool force_long)
{
    out.append((! force_long && visited) ? short_description : long_description);
    visited = true;

    if(countess_hiding())
    {
        out.append(hiding_blurb);
    }
}

void GuestBedroom::build_choices(bn::ivector<Choice>& out)
{
    out.push_back({ "Look", 0 });
    out.push_back({ "W", 1 });

    if(countess_hiding())
    {
        out.push_back({ "Confront the Countess", 3 });
    }

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

    case 3:
        screen.show_text("You tear open the bed curtains. The Countess shrieks, caught off "
                          "guard in her resting place. Her eyes blaze crimson as she lunges "
                          "at you from the bed, fangs bared!");
        Combat::fight(screen, EnemyId::Countess);
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
