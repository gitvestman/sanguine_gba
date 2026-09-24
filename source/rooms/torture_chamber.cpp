#include "torture_chamber.h"

#include "../ui/room_screen.h"
#include "../engine/game_state.h"
#include "../engine/rng.h"

namespace sg::rooms
{

namespace
{
    constexpr bn::string_view long_description =
        "An unbearable stench of decay. Three mangled corpses lie among rusted instruments "
        "of torture.";
    constexpr bn::string_view short_description = "The torture chamber.";
    constexpr bn::string_view trap_description = "You have no choice but to push through the stench.";
}

void TortureChamber::describe(bn::string<768>& out, bool force_long)
{
    out.append((! force_long && visited) ? short_description : long_description);
    visited = true;

    if(! _trap_sprung)
    {
        out.append("\n");
        out.append(trap_description);
    }
}

void TortureChamber::build_choices(bn::ivector<Choice>& out)
{
    if(! _trap_sprung)
    {
        out.push_back({ "Endure", 1 });
        return;
    }

    out.push_back({ "Look", 0 });
    out.push_back({ "W", 1 });
}

void TortureChamber::on_choice(int action_id, RoomScreen& screen)
{
    if(_trap_sprung)
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
            screen.goto_room(RoomId::CellarHall);
            break;
        }

        return;
    }

    if(action_id == 1)
    {
        Result result = screen.roll(Attribute::STR, 10, false, "Endure");

        if(result == Result::SUCCESS)
        {
            screen.show_text("You withstand the fetor without losing your mind.");
        }
        else
        {
            int loss = rng().get_int(1, 5);
            GameState::instance().change_sanity(-loss);

            bn::string<160> text = "The stench overwhelms you.\n\nYou lose ";
            text.append(bn::to_string<8>(loss));
            text.append(" Sanity.");
            screen.show_text(text);
        }

        _trap_sprung = true;
    }
}

uint16_t TortureChamber::save_state() const
{
    uint16_t bits = Room::save_state();
    if(_trap_sprung) bits |= 1 << 1;
    return bits;
}

void TortureChamber::load_state(uint16_t bits)
{
    Room::load_state(bits);
    _trap_sprung = bits & (1 << 1);
}

}
