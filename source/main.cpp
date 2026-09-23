#include "bn_core.h"
#include "bn_bg_palettes.h"

#include "ui/room_screen.h"

int main()
{
    bn::core::init();

    // Background palette index 0 is always drawn as this backdrop color.
    // tools/convert_room_bg.py puts each room's near-black border fill on
    // index 0, so this matches it.
    bn::bg_palettes::set_transparent_color(bn::color(1, 1, 1));

    sg::RoomScreen screen;
    screen.run(sg::RoomId::CellarHall);
}
