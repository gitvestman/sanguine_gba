#include "fancy_corridor_01.h"

#include "../ui/room_screen.h"

#include "bn_sprite_items_enemy_countess.h"

namespace sg::rooms
{

namespace
{
    constexpr bn::string_view long_description =
        "A fancy corridor lined with faded tapestries. Doors lead in every direction.";
    constexpr bn::string_view short_description = "The fancy corridor.";

    // False once she's gone (dead, let go peacefully, or off waiting in
    // DinnerHall for a dinner she was invited to) — any of those means
    // she's no longer lingering in this corridor.
    bool countess_active(const dialogue::Countess& countess)
    {
        return ! enemy(EnemyId::Countess).is_dead() && ! countess.resolved() &&
               ! countess.dinner_pending();
    }
}

void FancyCorridor01::describe(bn::string<768>& out, bool force_long)
{
    out.append((! force_long && visited) ? short_description : long_description);
    visited = true;

    if(countess_active(_countess) && _countess_met)
    {
        out.append("\n\nThe Countess still lingers here, watching you.");
    }
}

void FancyCorridor01::on_enter(RoomScreen& screen)
{
    if(countess_active(_countess) && ! _countess_met)
    {
        _countess_met = true;
        screen.show_text("A pale woman in a black gown stands in the corridor, as if she'd "
                          "been expecting you.");
        _countess.talk(screen, "\"Good evening,\" she says.", &bn::sprite_items::enemy_countess);
    }
}

void FancyCorridor01::build_choices(bn::ivector<Choice>& out)
{
    out.push_back({ "Look", 0 });
    out.push_back({ "N", 1 });
    out.push_back({ "NW", 2 });
    out.push_back({ "SW", 3 });
    out.push_back({ "S", 4 });
    out.push_back({ "SE", 5 });
    out.push_back({ "E", 6 });
    out.push_back({ "NE", 7 });

    if(countess_active(_countess) && _countess_met)
    {
        out.push_back({ "Talk to the Countess", 8 });
    }
}

void FancyCorridor01::on_choice(int action_id, RoomScreen& screen)
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
        screen.goto_room(RoomId::FancyCorridor02);
        break;

    case 2:
        screen.goto_room(RoomId::Bathroom);
        break;

    case 3:
        screen.goto_room(RoomId::ChildrensRoom);
        break;

    case 4:
        screen.goto_room(RoomId::AudienceHall);
        break;

    case 5:
        screen.goto_room(RoomId::BedChamber01);
        break;

    case 6:
        screen.goto_room(RoomId::GreatChamber);
        break;

    case 7:
        screen.goto_room(RoomId::GuestBedroom);
        break;

    case 8:
        _countess.talk(screen, "\"Back again?\" she asks.", &bn::sprite_items::enemy_countess);
        break;

    default:
        break;
    }
}

uint16_t FancyCorridor01::save_state() const
{
    uint16_t bits = Room::save_state();
    if(_countess_met) bits |= 1 << 1;
    if(_countess.resolved()) bits |= 1 << 2;
    if(_countess.dinner_pending()) bits |= 1 << 3;
    return bits;
}

void FancyCorridor01::load_state(uint16_t bits)
{
    Room::load_state(bits);
    _countess_met = bits & (1 << 1);
    _countess.set_resolved(bits & (1 << 2));
    _countess.set_dinner_pending(bits & (1 << 3));
}

}
