#include "main_gate.h"

#include "../ui/room_screen.h"
#include "../engine/game_state.h"
#include "../engine/rng.h"
#include "courtyard.h"

namespace sg::rooms
{

namespace
{
    constexpr bn::string_view long_description =
        "The castle's main gate looms ahead, flanked by infernal-looking statues.";
    constexpr bn::string_view short_description = "The main gate.";
    constexpr bn::string_view portcullis_hint = "\n\nThe portcullis is lowered.";
}

void MainGate::describe(bn::string<768>& out, bool force_long)
{
    out.append((! force_long && visited) ? short_description : long_description);
    visited = true;

    if(! static_cast<Courtyard&>(room(RoomId::Courtyard)).portcullis_raised)
    {
        out.append(portcullis_hint);
    }
}

void MainGate::build_choices(bn::ivector<Choice>& out)
{
    out.push_back({ "Look", 0 });

    if(static_cast<Courtyard&>(room(RoomId::Courtyard)).portcullis_raised)
    {
        out.push_back({ "N", 1 });
    }
    else
    {
        out.push_back({ "Crawl north", 2 });
    }

    out.push_back({ "S", 3 });
}

void MainGate::on_choice(int action_id, RoomScreen& screen)
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
        screen.goto_room(RoomId::Courtyard);
        break;

    case 2:
    {
        Result result = screen.roll(Attribute::DEX, 10, true, "Crawl north");

        if(result == Result::SUCCESS)
        {
            screen.show_text("You squeeze under the lowered portcullis.");
            screen.goto_room(RoomId::Courtyard);
        }
        else if(result == Result::FAIL || result == Result::CRITICAL_FAIL)
        {
            int damage = rng().get_int(1, 7);
            GameState::instance().take_damage(damage);

            bn::string<160> text = "A spike catches your side as you squeeze under. You take ";
            text.append(bn::to_string<8>(damage));
            text.append(" damage.");
            screen.show_text(text);
        }

        break;
    }

    case 3:
        screen.goto_room(RoomId::ForestCrossing);
        break;

    default:
        break;
    }
}

}
