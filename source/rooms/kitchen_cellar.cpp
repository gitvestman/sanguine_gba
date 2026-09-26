#include "kitchen_cellar.h"

#include "../ui/room_screen.h"
#include "../engine/combat.h"

namespace sg::rooms
{

namespace
{
    constexpr bn::string_view long_description =
        "A narrow cellar corridor beneath the kitchen, doors leading north and south.";
    constexpr bn::string_view short_description = "The kitchen cellar.";
    constexpr bn::string_view rat_blurb = "A rat skitters across the floor, baring its teeth.";
}

bool KitchenCellar::_rat_active() const
{
    return _rat_appeared && ! enemy(EnemyId::Rat).is_dead();
}

void KitchenCellar::describe(bn::string<768>& out, bool force_long)
{
    out.append((! force_long && visited) ? short_description : long_description);
    visited = true;

    if(_rat_active())
    {
        out.append("\n\n");
        out.append(rat_blurb);
    }
}

void KitchenCellar::on_enter(RoomScreen& screen)
{
    if(_rat_active())
    {
        screen.show_enemy(EnemyId::Rat);
    }
}

void KitchenCellar::build_choices(bn::ivector<Choice>& out)
{
    out.push_back({ "Look", 0 });
    out.push_back({ "E", 1 });
    out.push_back({ "S", 2 });
    out.push_back({ "N", 3 });

    if(_rat_active())
    {
        out.push_back({ "Attack rat", 4 });
    }
}

void KitchenCellar::on_choice(int action_id, RoomScreen& screen)
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
        screen.goto_room(RoomId::Kitchen);
        break;

    case 2:
        screen.goto_room(RoomId::WineCellar);
        break;

    case 3:
        screen.goto_room(RoomId::Larder);
        break;

    case 4:
        Combat::fight(screen, EnemyId::Rat);

        if(_rat_active())
        {
            screen.show_enemy(EnemyId::Rat);
        }

        break;

    default:
        break;
    }
}

void KitchenCellar::build_ambient(bn::ivector<AmbientDecor>& out) const
{
    out.push_back({ AmbientKind::TORCH, -78, -20 });
    out.push_back({ AmbientKind::TORCH, 75, -20 });
}

uint16_t KitchenCellar::save_state() const
{
    uint16_t bits = Room::save_state();
    if(_rat_appeared) bits |= 1 << 1;
    return bits;
}

void KitchenCellar::load_state(uint16_t bits)
{
    Room::load_state(bits);
    _rat_appeared = bits & (1 << 1);
}

}
