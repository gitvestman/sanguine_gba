#include "skybridge.h"

#include "../ui/room_screen.h"
#include "../engine/game_state.h"

namespace sg::rooms
{

namespace
{
    constexpr bn::string_view long_description =
        "A narrow stone bridge arcs between two towers, high above the courtyard. A "
        "ferocious wind howls across it.";
    constexpr bn::string_view trap_description = "\nThe wind threatens to tear you from the "
                                                  "railing entirely.";
    constexpr bn::string_view short_description = "The skybridge.";
}

void Skybridge::describe(bn::string<768>& out, bool force_long)
{
    out.append((! force_long && visited) ? short_description : long_description);
    visited = true;

    if(! _trap_survived)
    {
        out.append(trap_description);
    }
}

void Skybridge::build_choices(bn::ivector<Choice>& out)
{
    if(! _trap_survived)
    {
        out.push_back({ "Hold on", 1 });
        return;
    }

    out.push_back({ "Look", 0 });
    out.push_back({ "To the tower", 2 });
    out.push_back({ "E", 3 });
}

void Skybridge::on_choice(int action_id, RoomScreen& screen)
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
    {
        Result result = screen.roll(Attribute::STR, 12, false, "Hold on");

        if(result == Result::SUCCESS)
        {
            _trap_survived = true;
            screen.show_text("You grip the railing with white knuckles and wait out the "
                              "worst of the gust.");
        }
        else
        {
            GameState& gs = GameState::instance();
            gs.take_damage(gs.stat(Attribute::HP));
            screen.show_text("The wind rips your hands from the railing. You fall.");
        }

        break;
    }

    case 2:
        screen.goto_room(RoomId::ScientificLaboratory);
        break;

    case 3:
        screen.goto_room(RoomId::FancyCorridor02);
        break;

    default:
        break;
    }
}

uint16_t Skybridge::save_state() const
{
    uint16_t bits = Room::save_state();
    if(_trap_survived) bits |= 1 << 1;
    return bits;
}

void Skybridge::load_state(uint16_t bits)
{
    Room::load_state(bits);
    _trap_survived = bits & (1 << 1);
}

}
