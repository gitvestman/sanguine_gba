#ifndef SG_CONSTANTS_H
#define SG_CONSTANTS_H

// Ported from D:\Workspace\Godot\sanguine\singletons\constants_singleton.gd and
// gamestate_singleton.gd. Only the pieces the Basement vertical slice actually
// uses are ported; combat/dialogue/save-specific enums are added as later
// phases need them.

namespace sg
{

enum class Attribute
{
    NONE,
    INT_,
    STR,
    DEX,
    HEALTH,
    SANITY,
    LUCK,
    HP,
    DC
};

constexpr int attribute_count = int(Attribute::DC) + 1;

enum class Difficulty
{
    DEBUG,
    EASY,
    NORMAL,
    HARD,
    NIGHTMARE
};

enum class Result
{
    SUCCESS,
    FAIL,
    CANCEL,
    CRITICAL_FAIL
};

constexpr int max_stat = 20;

}

#endif
