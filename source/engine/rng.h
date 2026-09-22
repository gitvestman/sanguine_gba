#ifndef SG_RNG_H
#define SG_RNG_H

#include "bn_random.h"

// One shared RNG for dice rolls and the small randi_range(...)-style rolls
// rooms use directly (trap damage, sanity loss, ...) — mirrors Godot's
// single global RNG (randomize() in gamestate_singleton.gd's _ready()).
namespace sg
{

bn::random& rng();

}

#endif
