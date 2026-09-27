#include "settings.h"

#include <cstdint>

#include "bn_fixed.h"
#include "bn_music.h"
#include "bn_sound.h"
#include "bn_sram.h"

#include "save_game.h"

namespace sg
{

namespace
{
    constexpr uint32_t magic = 0x53'45'54'01; // "SET" + version 1

    struct SettingsData
    {
        uint32_t magic;
        uint8_t music_volume_step;
        uint8_t sfx_volume_step;
    };

    static_assert(sizeof(SettingsData) <= SaveGame::slot_size,
                  "Settings data has grown past its reserved SRAM slot size");

    // Right after the save slots' own reserved span, so the two blocks can
    // never overlap even if SaveData ever grows (up to slot_size).
    int settings_offset()
    {
        return SaveGame::slot_count * SaveGame::slot_size;
    }

    bn::fixed step_to_volume(int step)
    {
        return bn::fixed(step) / bn::fixed(Settings::max_volume_steps);
    }
}

Settings& Settings::instance()
{
    static Settings settings;
    return settings;
}

void Settings::set_music_volume_step(int step)
{
    _music_volume_step = step;
    apply();
}

void Settings::set_sfx_volume_step(int step)
{
    _sfx_volume_step = step;
    apply();
}

void Settings::apply() const
{
    // bn::music::set_volume() only affects whatever's currently playing —
    // and BN_BASIC_ASSERTs if nothing is (there's no music yet, Phase 5b),
    // so it must be skipped entirely until then. Once real tracks exist,
    // every bn::music::play() call site must also pass
    // step_to_volume(*)-equivalent itself, not rely on this alone, for the
    // same reason: this only ever adjusts an already-playing track.
    // bn::sound::set_master_volume() has no such restriction — it's a
    // persistent global multiplier, so it already covers every existing
    // and future bn::sound::play() call with no further wiring needed.
    if(bn::music::playing())
    {
        bn::music::set_volume(step_to_volume(_music_volume_step));
    }

    bn::sound::set_master_volume(step_to_volume(_sfx_volume_step));
}

void Settings::load()
{
    SettingsData data{};
    bn::sram::read_offset(data, settings_offset());

    if(data.magic == magic)
    {
        _music_volume_step = data.music_volume_step;
        _sfx_volume_step = data.sfx_volume_step;
    }

    apply();
}

void Settings::save() const
{
    SettingsData data{};
    data.magic = magic;
    data.music_volume_step = uint8_t(_music_volume_step);
    data.sfx_volume_step = uint8_t(_sfx_volume_step);

    bn::sram::write_offset(data, settings_offset());
}

}
