#include "enemy.h"

#include "bn_string.h"

#include "game_state.h"
#include "rng.h"
#include "../ui/room_screen.h"

#include "bn_sprite_items_enemy_dungeon_creatures.h"
#include "bn_sprite_items_enemy_homunculus.h"
#include "bn_sprite_items_enemy_rat.h"
#include "bn_sprite_items_enemy_hungry_bats.h"

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
    default:
        return hungry_bats;
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
    default:
        return bn::sprite_items::enemy_hungry_bats;
    }
}

}
