#include "scientific_laboratory.h"

#include "../ui/room_screen.h"
#include "../engine/game_state.h"
#include "../engine/inventory.h"
#include "../engine/rng.h"

#include "bn_sprite_items_enemy_golem.h"

namespace sg::rooms
{

namespace
{
    constexpr bn::string_view long_description =
        "Arcane apparatus and coiled brass wire crowd this cluttered laboratory. The air "
        "crackles with static.";
    constexpr bn::string_view trap_description = "\nA loose wire sparks dangerously close to "
                                                  "a puddle at your feet.";
    constexpr bn::string_view short_description = "The scientific laboratory.";
}

void ScientificLaboratory::describe(bn::string<768>& out, bool force_long)
{
    out.append((! force_long && visited) ? short_description : long_description);
    visited = true;

    if(! _trap_sprung)
    {
        out.append(trap_description);
    }
    else if(! enemy(EnemyId::Golem).is_dead() && ! _golem.resolved())
    {
        out.append("\n\nA hulking construct of stone and iron stands motionless in the "
                    "corner, watching you.");
    }
}

void ScientificLaboratory::build_choices(bn::ivector<Choice>& out)
{
    if(! _trap_sprung)
    {
        out.push_back({ "Dodge the lightning", 1 });
        return;
    }

    out.push_back({ "Look", 0 });
    out.push_back({ "E", 2 });

    if(! _found_cog)
    {
        out.push_back({ "Search", 3 });
    }

    if(! enemy(EnemyId::Golem).is_dead() && ! _golem.resolved())
    {
        if(! _golem_approached)
        {
            out.push_back({ "Address the golem", 4 });
            out.push_back({ "Threaten the golem", 5 });
        }
        else
        {
            out.push_back({ "Talk to the golem", 6 });
        }
    }
}

void ScientificLaboratory::on_choice(int action_id, RoomScreen& screen)
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
        Result result = screen.roll(Attribute::DEX, 10, false, "Dodge the lightning");

        if(result == Result::SUCCESS)
        {
            screen.show_text("You dodge clear as a spark arcs across the puddle.");
        }
        else
        {
            int damage = rng().get_int(1, 7);
            GameState::instance().take_damage(damage);

            bn::string<160> text = "The spark catches you. You take ";
            text.append(bn::to_string<8>(damage));
            text.append(" damage.");
            screen.show_text(text);
        }

        _trap_sprung = true;

        if(! enemy(EnemyId::Golem).is_dead())
        {
            screen.show_text("As the sparks die down, a hulking construct of stone and iron "
                              "stirs in the corner.");
        }

        break;
    }

    case 2:
        screen.goto_room(RoomId::Skybridge);
        break;

    case 3:
    {
        _found_cog = true;
        Result result = screen.roll(Attribute::INT_, 12, false, "Search");

        if(result == Result::SUCCESS)
        {
            Inventory::instance().add(ItemId::COG);
            screen.show_text("Among the brass fittings, you find a small precision cog.");
        }
        else
        {
            screen.show_text("You find nothing more of use.");
        }

        break;
    }

    case 4:
        _golem_approached = true;
        screen.show_text("\"I mean you no harm,\" you say. The golem's amber eyes track "
                          "you, but it doesn't move.");
        _golem.talk(screen, "It waits.", &bn::sprite_items::enemy_golem);
        break;

    case 5:
        _golem_approached = true;
        screen.show_text("\"Stand aside,\" you warn it. The golem's eyes flare brighter.");
        _golem.talk(screen, "It waits.", &bn::sprite_items::enemy_golem);
        break;

    case 6:
        _golem.talk(screen, "It waits.", &bn::sprite_items::enemy_golem);
        break;

    default:
        break;
    }
}

uint16_t ScientificLaboratory::save_state() const
{
    uint16_t bits = Room::save_state();
    if(_trap_sprung) bits |= 1 << 1;
    if(_golem_approached) bits |= 1 << 2;
    if(_found_cog) bits |= 1 << 3;
    if(_golem.resolved()) bits |= 1 << 4;
    return bits;
}

void ScientificLaboratory::load_state(uint16_t bits)
{
    Room::load_state(bits);
    _trap_sprung = bits & (1 << 1);
    _golem_approached = bits & (1 << 2);
    _found_cog = bits & (1 << 3);
    _golem.set_resolved(bits & (1 << 4));
}

}
