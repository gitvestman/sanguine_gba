#ifndef SG_ENEMY_H
#define SG_ENEMY_H

#include "bn_string_view.h"

// Ported from D:\Workspace\Godot\sanguine\enemies\enemy.gd. Every enemy
// there overrides do_action() only to call the shared attack_player() with
// its own flavor text — there's no other per-enemy behavior for the two
// enemies this vertical slice fights (dungeon_creatures_hostile.gd,
// homunculus_hostile.gd), so unlike Room this is one concrete data-only
// class instead of a base class + subclass-per-enemy: no virtual dispatch
// has anything to dispatch to.
namespace sg
{

class RoomScreen;

enum class EnemyId
{
    DungeonCreatures,
    Homunculus,

    COUNT
};

class Enemy
{

public:
    bn::string_view name;
    bn::string_view description;
    bn::string_view action_text;
    int base_hp;
    int to_hit;
    int damage;
    int dc;
    bool undead;

    int hp = base_hp;

    bool is_dead() const
    {
        return hp <= 0;
    }

    void take_player_damage(int d)
    {
        hp -= d;

        if(hp < 0)
        {
            hp = 0;
        }
    }

    void reset_hp()
    {
        hp = base_hp;
    }

    // Rolls d20 + to_hit + the difficulty's enemy-to-hit bonus against the
    // player's DC; on a hit, deals 1..damage (+ the difficulty's enemy
    // damage bonus, minimum 1) and shows the result. Shared by every enemy
    // — see Enemy.attack_player in enemy.gd.
    void do_action(RoomScreen& screen) const;
};

Enemy& enemy(EnemyId id);

}

#endif
