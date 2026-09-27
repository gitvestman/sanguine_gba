#include "music.h"

#include "bn_music.h"
#include "bn_music_items.h"

#include "settings.h"

namespace sg
{

void play_default_theme()
{
    bn::music::play(bn::music_items::default_theme, Settings::instance().music_volume());
}

}
