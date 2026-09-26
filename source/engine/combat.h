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
    FLED,

    // The enemy fled instead of the player — see `flee_after_hits` below.
    // Not dead (is_dead() stays false), so the caller decides what "gone"
    // means for that specific encounter (FancyCorridor01's Countess just
    // marks herself resolved(), same as if she'd been let go peacefully).
    ENEMY_FLED
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
    //
    // `flee_after_hits`, when > 0, makes the enemy flee on what would have
    // been its next turn once the player has landed that many direct hits
    // (misses don't count) — mirrors the Countess's countess_hostile.gd
    // check_corridor_flee(), which only fires while she's fought in
    // FancyCorridor01 specifically (her DinnerHall fight is a real fight to
    // the death, so that call site passes 0).
    static CombatResult fight(RoomScreen& screen, EnemyId enemy_id, int flee_after_hits = 0);
};

}

#endif
