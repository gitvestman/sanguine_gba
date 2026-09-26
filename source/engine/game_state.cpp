#include "game_state.h"

#include "bn_algorithm.h"
#include "bn_sound_items.h"

namespace sg
{

namespace
{
    // Plays a short SFX whenever a stat mutator actually moves the value
    // (post-clamp, so e.g. healing at full HP stays silent).
    void play_stat_change_sfx(int before, int after)
    {
        if(after > before)
        {
            bn::sound_items::stat_gain.play();
        }
        else if(after < before)
        {
            bn::sound_items::stat_loss.play();
        }
    }
}

int convert_stat_to_bonus(int stat)
{
    if(stat >= 18) return 4;
    if(stat >= 16) return 3;
    if(stat >= 14) return 2;
    if(stat >= 12) return 1;
    if(stat >= 10) return 0;
    if(stat >= 8) return -1;
    if(stat >= 6) return -2;
    if(stat >= 5) return -3;
    if(stat >= 3) return -4;
    return -5;
}

namespace
{
    // RollNeededPerDifficulty / EnemyDamageBonusPerDifficulty from
    // gamestate_singleton.gd, indexed by Difficulty.
    constexpr int roll_needed_per_difficulty[] = { -6, -2, 0, 2, 4 };
    constexpr int enemy_damage_bonus_per_difficulty[] = { -2, -1, 0, 1, 2 };
}

GameState& GameState::instance()
{
    static GameState state;
    return state;
}

GameState::GameState()
{
    reset_to_dario();
}

void GameState::reset_to_dario()
{
    _stats[int(Attribute::INT_)] = 12;
    _stats[int(Attribute::DEX)] = 8;
    _stats[int(Attribute::STR)] = 12;
    _stats[int(Attribute::HP)] = 14;
    _stats[int(Attribute::LUCK)] = 10;
    _stats[int(Attribute::SANITY)] = 8;
    _stats[int(Attribute::DC)] = 12 + convert_stat_to_bonus(_stats[int(Attribute::DEX)]);
    _stats[int(Attribute::HEALTH)] = 0;
    _stats[int(Attribute::NONE)] = 0;
    _difficulty = Difficulty::EASY;
}

int GameState::stat(Attribute attribute) const
{
    return _stats[int(attribute)];
}

void GameState::set_stat(Attribute attribute, int value)
{
    int before = _stats[int(attribute)];
    int after = bn::clamp(value, 0, max_stat);
    _stats[int(attribute)] = after;
    play_stat_change_sfx(before, after);
}

void GameState::load_stat(Attribute attribute, int value)
{
    _stats[int(attribute)] = bn::clamp(value, 0, max_stat);
}

int GameState::roll_needed(int modifier) const
{
    return modifier + roll_needed_per_difficulty[int(_difficulty)];
}

void GameState::take_damage(int damage)
{
    set_stat(Attribute::HP, stat(Attribute::HP) - damage);
}

void GameState::change_sanity(int delta)
{
    set_stat(Attribute::SANITY, stat(Attribute::SANITY) + delta);
}

void GameState::change_luck(int delta)
{
    set_stat(Attribute::LUCK, stat(Attribute::LUCK) + delta);
}

void GameState::change_dex(int delta)
{
    set_stat(Attribute::DEX, stat(Attribute::DEX) + delta);
}

int GameState::enemy_to_hit_bonus() const
{
    return roll_needed_per_difficulty[int(_difficulty)];
}

int GameState::enemy_damage_bonus() const
{
    return enemy_damage_bonus_per_difficulty[int(_difficulty)];
}

}
