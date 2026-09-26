#include "enemy.h"

#include "bn_string.h"

#include "game_state.h"
#include "rng.h"
#include "../ui/room_screen.h"

#include "bn_sprite_items_enemy_dungeon_creatures.h"
#include "bn_sprite_items_enemy_homunculus.h"
#include "bn_sprite_items_enemy_rat.h"
#include "bn_sprite_items_enemy_hungry_bats.h"
#include "bn_sprite_items_enemy_raven.h"
#include "bn_sprite_items_enemy_mad_gnome.h"
#include "bn_sprite_items_enemy_zombie.h"
#include "bn_sprite_items_enemy_countess.h"
#include "bn_sprite_items_enemy_golem.h"
#include "bn_sprite_items_enemy_night_raven.h"

namespace sg
{

void Enemy::do_action(RoomScreen& screen) const
{
    GameState& game_state = GameState::instance();
    int roll = rng().get_int(1, 21) + to_hit + game_state.enemy_to_hit_bonus();
    int player_dc = game_state.stat(Attribute::DC);

    bn::string<160> text;
    text.append(action_text);
    text.append("\n\nRoll: ");
    text.append(bn::to_string<8>(roll));
    text.append(" vs ");
    text.append(bn::to_string<8>(player_dc));

    if(roll > player_dc)
    {
        int hit_damage = rng().get_int(1, damage + 1) + game_state.enemy_damage_bonus();

        if(hit_damage < 1)
        {
            hit_damage = 1;
        }

        game_state.take_damage(hit_damage);
        text.append("\n\nAnd hits for ");
        text.append(bn::to_string<8>(hit_damage));
        text.append(" damage.");
    }
    else
    {
        text.append("\n\nAnd misses.");
    }

    screen.show_text(text);
}

namespace
{
    Enemy dungeon_creatures{
        "the dungeon creatures",
        "Small, deformed humanoids with sharp teeth and claws, driven by hunger.",
        "The creatures swarm you, biting and scratching.",
        5, 3, 6, 12, true
    };

    Enemy homunculus{
        "the homunculus",
        "A small, misshapen homunculus, bubbling with acidic spit.",
        "The homunculus slashes with magical energy.",
        8, 4, 6, 12, false
    };

    Enemy rat{
        "the rat",
        "A large, mangy rat, driven bold by hunger.",
        "The rat lunges and sinks its teeth into your leg!",
        2, 3, 2, 10, false
    };

    Enemy hungry_bats{
        "the hungry bats",
        "A cloud of bats, agitated and hungry for blood.",
        "The hungry bats swarm you!",
        5, 4, 4, 12, false
    };

    Enemy raven{
        "the raven",
        "A raven with eyes gleaming an unnatural red malevolence.",
        "The raven swoops at you with sharp talons!",
        3, 3, 2, 10, false
    };

    Enemy mad_gnome{
        "the mad gnome",
        "A wild-eyed gnome, clutching a rusty blade with white-knuckled fury.",
        "The mad gnome attacks with a rusty blade.",
        6, 3, 6, 10, false
    };

    Enemy zombie{
        "the zombie",
        "A rotting corpse stands before you.",
        "The zombie swipes at you with rotting claws.",
        10, 4, 6, 10, true
    };

    // Ported with a real, but much simpler, fight than the source's
    // per-attack reaction-roll system (nine named attacks, each its own
    // flavor text and a player counter-strike on a successful defense) —
    // that needs a whole extra combat mode (defensive reaction rolls)
    // Combat::fight doesn't have. She's still the toughest fight in this
    // build (highest HP/damage/DC of anything ported so far).
    Enemy countess{
        "the Countess",
        "A pale, imperious woman in a black gown, her eyes utterly without warmth.",
        "The Countess lunges with unnatural speed.",
        15, 5, 8, 12, true
    };

    Enemy golem{
        "the golem",
        "A hulking construct of stone and iron, its eyes glowing faint amber.",
        "The golem swings a massive stone fist.",
        10, 5, 6, 14, false
    };

    Enemy night_raven{
        "the night raven",
        "A raven larger than any natural bird, its feathers unnaturally black.",
        "The night raven tears at you with talons like knives.",
        7, 5, 6, 14, false
    };
}

Enemy& enemy(EnemyId id)
{
    switch(id)
    {

    case EnemyId::DungeonCreatures:
        return dungeon_creatures;

    case EnemyId::Homunculus:
        return homunculus;

    case EnemyId::Rat:
        return rat;

    case EnemyId::HungryBats:
        return hungry_bats;

    case EnemyId::Raven:
        return raven;

    case EnemyId::MadGnome:
        return mad_gnome;

    case EnemyId::Zombie:
        return zombie;

    case EnemyId::Countess:
        return countess;

    case EnemyId::Golem:
        return golem;

    case EnemyId::NightRaven:
    default:
        return night_raven;
    }
}

const bn::sprite_item& enemy_sprite_item(EnemyId id)
{
    switch(id)
    {

    case EnemyId::DungeonCreatures:
        return bn::sprite_items::enemy_dungeon_creatures;

    case EnemyId::Homunculus:
        return bn::sprite_items::enemy_homunculus;

    case EnemyId::Rat:
        return bn::sprite_items::enemy_rat;

    case EnemyId::HungryBats:
        return bn::sprite_items::enemy_hungry_bats;

    case EnemyId::Raven:
        return bn::sprite_items::enemy_raven;

    case EnemyId::MadGnome:
        return bn::sprite_items::enemy_mad_gnome;

    case EnemyId::Zombie:
        return bn::sprite_items::enemy_zombie;

    case EnemyId::Countess:
        return bn::sprite_items::enemy_countess;

    case EnemyId::Golem:
        return bn::sprite_items::enemy_golem;

    case EnemyId::NightRaven:
    default:
        return bn::sprite_items::enemy_night_raven;
    }
}

EnemyAnim enemy_anim_kind(EnemyId id)
{
    switch(id)
    {

    case EnemyId::Rat:
        return EnemyAnim::RAT_FLOOR;

    case EnemyId::Raven:
    case EnemyId::HungryBats:
        return EnemyAnim::FIGURE_EIGHT;

    default:
        return EnemyAnim::NONE;
    }
}

}
