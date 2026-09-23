#ifndef SG_COMBAT_H
#define SG_COMBAT_H

#include "enemy.h"

// Ported from the fighting branch of gamestate_singleton.gd's choose() and
// get_player_fight_choices(). Only Hand to Hand, fleeing, and the two
// consumables obtainable on this floor (Restorative Mixture, Healing
// Tincture) are ported — the weapon options (Silver Dagger, Flintlock,
// Rosary, ...) all need items no Basement room grants, so they're
// unreachable in this slice regardless.
namespace sg
{

class RoomScreen;

enum class CombatResult
{
    WON,
    FLED
};

class Combat
{

public:
    // Blocks until the fight ends. Resets the enemy's hp first: Godot's
    // enemy hp is session-persistent across encounters (enemy_states), but
    // both of this slice's encounters are one-time, gated by room state, so
    // a full reset per encounter looks the same without that machinery —
    // the one visible difference is that fleeing and re-entering the fight
    // gives a fresh, undamaged enemy instead of picking up where it left
    // off.
    static CombatResult fight(RoomScreen& screen, EnemyId enemy_id);
};

}

#endif
