#include "bn_core.h"
#include "bn_bg_palettes.h"

#include "ui/room_screen.h"
#include "engine/rng.h"

int main()
{
    bn::core::init();

    // The GBA has no hardware random source and bn::random starts from a
    // fixed seed, so without this every playthrough rolls the same numbers.
    // Advancing it every frame makes each roll depend on the exact frame
    // the player pressed a button on.
    bn::core::set_update_callback([] { sg::rng().update(); });

    // Background palette index 0 is always drawn as this backdrop color.
    // tools/convert_room_bg.py puts each room's near-black border fill on
    // index 0, so this matches it.
    bn::bg_palettes::set_transparent_color(bn::color(1, 1, 1));

    // RoomScreen holds the background, text box, choice list, both
    // sidebars and the dice view — a few KB. As a plain local here, that
    // entire block would sit on the stack for main()'s whole lifetime
    // (main never returns while the game runs), eating into the stack
    // headroom every deeper call (combat -> dice roll -> text/choice
    // updates) needs. `static` puts it in BSS instead.
    static sg::RoomScreen screen;
    screen.run(sg::RoomId::CellarHall);
}
