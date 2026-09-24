#ifndef SG_ESCAPE_H
#define SG_ESCAPE_H

#include "bn_optional.h"

#include "enemy.h"

// Ported from the escape/pursuit half of gamestate_singleton.gd (Phase 4f).
// The personal-goal half of is_escaping() is dropped entirely: every
// character's goal check in the source is a stub that always returns false
// (none were ever designed), and this build has no character-select screen
// to hang a goal off of anyway — so here, escape starts (and can only
// start) the moment the Countess dies.
namespace sg
{

bool is_escaping();

// A random still-living enemy that can join the chase, or nullopt if
// nothing's left alive to pursue. Excludes Raven, same as the source's
// get_living_pursuers (rooted/place-bound enemies don't chase) — the other
// two exclusions there (Orangery Plants, the pond creature) were never
// built as real fights in this port, so there's nothing to exclude for
// them.
bn::optional<EnemyId> pick_pursuer();

}

#endif
