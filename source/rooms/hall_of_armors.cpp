#include "hall_of_armors.h"

#include "../ui/room_screen.h"
#include "../engine/game_state.h"
#include "../engine/rng.h"
#include "chapel.h"

namespace sg::rooms
{

namespace
{
    constexpr bn::string_view long_description =
        "Rows of armored suits line the hall, each aiming a loaded crossbow. Something here "
        "feels rigged to strike.";
    constexpr bn::string_view short_description = "The hall of armor.";
    constexpr bn::string_view trap_hint = "\n\nYou know exactly where the tripwire is now.";
}

void HallOfArmors::describe(bn::string<768>& out, bool force_long)
{
    out.append((! force_long && visited) ? short_description : long_description);
    visited = true;

    if(_trap_found && ! _trap_sprung)
    {
        out.append(trap_hint);
    }
}

void HallOfArmors::build_choices(bn::ivector<Choice>& out)
{
    out.push_back({ "Look", 0 });

    if(! _trap_sprung && ! _trap_found && ! _searched)
    {
        out.push_back({ "Search", 1 });
    }

    out.push_back({ "N", 2 });
    out.push_back({ "S", 3 });
}

void HallOfArmors::on_choice(int action_id, RoomScreen& screen)
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
        _searched = true;
        Result result = screen.roll(Attribute::INT_, 10, false, "Search");

        if(result == Result::SUCCESS)
        {
            _trap_found = true;
            screen.show_text("You spot a near-invisible tripwire strung ankle-high between "
                              "two suits of armor.");
        }
        else
        {
            screen.show_text("You find nothing out of the ordinary.");
        }

        break;
    }

    case 2:
        _maybe_trigger_trap(1, screen);
        static_cast<Chapel&>(room(RoomId::Chapel)).set_entered_from_ossuary(false);
        screen.goto_room(RoomId::Chapel);
        break;

    case 3:
        _maybe_trigger_trap(2, screen);
        screen.goto_room(RoomId::GreatHall);
        break;

    default:
        break;
    }
}

void HallOfArmors::_maybe_trigger_trap(int exit_side, RoomScreen& screen)
{
    if(_trap_sprung || _entered_from == 0 || _entered_from == exit_side)
    {
        return;
    }

    if(_trap_found)
    {
        screen.show_text("You step carefully over the tripwire.");
    }
    else
    {
        int damage = rng().get_int(1, 7);
        GameState::instance().take_damage(damage);

        bn::string<160> text = "A crossbow bolt fires from the wall! You take ";
        text.append(bn::to_string<8>(damage));
        text.append(" damage.");
        screen.show_text(text);
    }

    _trap_sprung = true;
}

uint16_t HallOfArmors::save_state() const
{
    uint16_t bits = Room::save_state();
    if(_trap_found) bits |= 1 << 1;
    if(_trap_sprung) bits |= 1 << 2;
    if(_searched) bits |= 1 << 3;
    return bits;
}

void HallOfArmors::load_state(uint16_t bits)
{
    Room::load_state(bits);
    _trap_found = bits & (1 << 1);
    _trap_sprung = bits & (1 << 2);
    _searched = bits & (1 << 3);
}

}
