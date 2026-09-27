#ifndef SG_ENEMY_H
#define SG_ENEMY_H

#include "bn_string_view.h"
#include "bn_sprite_item.h"
#include "bn_optional.h"
#include "bn_music_item.h"

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
    Rat,
    HungryBats,
    Raven,
    MadGnome,
    Zombie,
    Countess,
    Golem,
    NightRaven,

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

// The sprite shown overlaid on the room background during Combat::fight —
// matches the source project's SpriteAnimations.*() enemy art overlay.
const bn::sprite_item& enemy_sprite_item(EnemyId id);

// A per-enemy boss theme (see countess_hostile.gd's own `music` field) —
// Combat::fight() plays this instead of nothing for the fight's duration,
// and nullopt for every enemy without one (most of them; a plain
// bn::optional rather than a table entry per EnemyId, since so few need it).
bn::optional<bn::music_item> enemy_music_item(EnemyId id);

// Phase 4i: a handful of small critters get a per-instance overlay
// animation in the source project (utils/sprite_animations.gd) instead of
// sitting still — a floor-dashing waypoint walk (2x scale) for the rat, and
// a looping figure-eight flight path for the raven/hungry bats. Everything
// else (Zombie, Golem, Countess, MadGnome, GhostlyBride's portrait, ...) is
// static in the source too, so this only needs three cases.
enum class EnemyAnim
{
    NONE,
    RAT_FLOOR,
    FIGURE_EIGHT
};

EnemyAnim enemy_anim_kind(EnemyId id);

}

#endif
