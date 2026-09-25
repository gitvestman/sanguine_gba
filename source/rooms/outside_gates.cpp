#include "outside_gates.h"

#include "../ui/room_screen.h"
#include "../engine/escape.h"
#include "../engine/combat.h"

namespace sg::rooms
{

namespace
{
    constexpr bn::string_view long_description =
        "A muddy, neglected road leads you to a gate, its torches offering welcome light "
        "against the looming pines. The opening is narrow, barely wide enough to walk "
        "through - but it's the only way in.";
    constexpr bn::string_view short_description = "The outer gates.";
    constexpr bn::string_view escape_description =
        "\n\nA coach waits on the road beyond. Something stands between you and it.";
    constexpr bn::string_view escape_clear_description =
        "\n\nA coach waits on the road beyond, and nothing stands in your way.";
}

void OutsideGates::describe(bn::string<768>& out, bool force_long)
{
    out.append((! force_long && visited) ? short_description : long_description);
    visited = true;

    if(is_escaping())
    {
        out.append(_guard != EnemyId::COUNT && ! enemy(_guard).is_dead() ? escape_description
                                                                         : escape_clear_description);
    }
}

void OutsideGates::on_enter(RoomScreen&)
{
    if(is_escaping() && ! _guard_assigned)
    {
        _guard_assigned = true;
        bn::optional<EnemyId> pursuer = pick_pursuer();
        _guard = pursuer.has_value() ? pursuer.value() : EnemyId::COUNT;
    }
}

void OutsideGates::build_choices(bn::ivector<Choice>& out)
{
    if(is_escaping())
    {
        if(_guard != EnemyId::COUNT && ! enemy(_guard).is_dead())
        {
            out.push_back({ "Fight your way through", 2 });
        }
        else
        {
            out.push_back({ "Approach the coach", 1 });
        }

        out.push_back({ "N", 3 });
        return;
    }

    out.push_back({ "Look", 0 });
    out.push_back({ "N", 3 });
}

void OutsideGates::on_choice(int action_id, RoomScreen& screen)
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
        screen.win();
        break;

    case 2:
        Combat::fight(screen, _guard);
        break;

    case 3:
        screen.goto_room(RoomId::ForestCrossing);
        break;

    default:
        break;
    }
}

uint16_t OutsideGates::save_state() const
{
    uint16_t bits = Room::save_state();
    if(_guard_assigned) bits |= 1 << 1;
    bits |= (uint16_t(_guard) & 0xF) << 2;
    return bits;
}

void OutsideGates::load_state(uint16_t bits)
{
    Room::load_state(bits);
    _guard_assigned = bits & (1 << 1);
    _guard = EnemyId((bits >> 2) & 0xF);
}

}
