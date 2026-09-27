#ifndef SG_MUSIC_H
#define SG_MUSIC_H

// Small shared helper around the one ambient in-game track (Phase 5c) so
// every place that interrupts it for something else (currently just the
// Countess's boss theme — see Combat::fight/FancyCorridor01/DinnerHall) can
// resume the exact same thing afterward without each repeating
// bn::music::play(bn::music_items::default_theme, Settings::instance().
// music_volume()) itself. If per-room tracks are ever added, this is the
// one place that needs to become "whichever track the current room wants"
// instead of always the same one.
namespace sg
{

void play_default_theme();

}

#endif
