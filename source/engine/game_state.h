#ifndef SG_GAME_STATE_H
#define SG_GAME_STATE_H

#include "constants.h"

// Ported from D:\Workspace\Godot\sanguine\singletons\gamestate_singleton.gd —
// only the character-stat / roll-math / damage subset the Basement vertical
// slice exercises. Inventory lives in inventory.h; save/load and the rest of
// the stat surface (INT/DC changes, personal goals, escape mode, ...) are not
// part of this slice yet.
namespace sg
{

// Bonus a raw D&D-style stat (3..20) grants to a roll. Mirrors
// GameState.convert_stat_to_bonus exactly.
int convert_stat_to_bonus(int stat);

class GameState
{

public:
    static GameState& instance();

    // Starting stats match character_id 0 (Dario) from GameState.set_character.
    void reset_to_dario();

    int stat(Attribute attribute) const;
    void set_stat(Attribute attribute, int value);

    Difficulty difficulty() const
    {
        return _difficulty;
    }

    // Mirrors GameState.RollNeeded(modifier): the difficulty-adjusted DC a
    // player must meet or beat with (d20 roll + attribute bonus). Attribute
    // bonus itself is applied separately to the roll, not here — see dice.h.
    int roll_needed(int modifier) const;

    void take_damage(int damage);
    void change_sanity(int delta);
    void change_luck(int delta);

    bool is_dead() const
    {
        return stat(Attribute::HP) <= 0;
    }

    // Bonus to an enemy's to-hit roll / damage for the current difficulty —
    // mirrors GameState.enemy_to_hit_bonus/enemy_damage_bonus, the
    // difficulty's own two-sided scaling (harder also hits harder).
    int enemy_to_hit_bonus() const;
    int enemy_damage_bonus() const;

private:
    GameState();

    int _stats[attribute_count];
    Difficulty _difficulty;
};

}

#endif
