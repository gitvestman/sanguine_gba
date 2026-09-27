#ifndef SG_SETTINGS_H
#define SG_SETTINGS_H

#include "bn_fixed.h"

// Persisted device-level preferences (music/SFX volume) — distinct from
// SaveGame's 3 save slots, since these describe how loud the game should
// be, not game progress; they apply regardless of which slot (if any) is
// active, and outlive any single playthrough. Lives in its own fixed SRAM
// block, right after the save slots' reserved span (see
// SaveGame::slot_count/slot_size) so the two can never collide.
namespace sg
{

class Settings
{

public:
    static Settings& instance();

    // Coarse 0..max_volume_steps levels (25% each) rather than a fine-
    // grained slider — plenty of range for a GBA volume setting, and simple
    // to store as a single byte per channel.
    static constexpr int max_volume_steps = 4;

    int music_volume_step() const
    {
        return _music_volume_step;
    }

    int sfx_volume_step() const
    {
        return _sfx_volume_step;
    }

    // The current music volume as a [0..1] fraction (step / max_volume_steps)
    // — for call sites like bn::music::play() that take a volume directly,
    // so they don't each need to redo this division themselves.
    bn::fixed music_volume() const;

    // Changes take effect immediately (see apply()) and are NOT
    // auto-persisted — call save() once the player is done adjusting
    // (RoomScreen::show_settings_menu() does this on exit), same shape as
    // SaveGame's own explicit save() call.
    void set_music_volume_step(int step);
    void set_sfx_volume_step(int step);

    // Pushes the current volumes to bn::music/bn::sound — call once at
    // boot right after load(), and again after any set_*_volume_step()
    // call (both set_*_volume_step() and load() already do this
    // themselves, so this is only exposed for main.cpp's initial call
    // before anything else has touched audio).
    void apply() const;

    // Restores from SRAM, or leaves the just-constructed defaults (both
    // channels at max) if nothing was ever saved yet.
    void load();

    void save() const;

private:
    Settings() = default;

    int _music_volume_step = max_volume_steps;
    int _sfx_volume_step = max_volume_steps;
};

}

#endif
