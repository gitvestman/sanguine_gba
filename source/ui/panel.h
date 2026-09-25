#ifndef SG_UI_PANEL_H
#define SG_UI_PANEL_H

#include "bn_sprite_ptr.h"
#include "bn_vector.h"

// The solid backdrop behind the bottom text/choice band, so text stays
// legible over whatever room art is behind it. Built from tiled 64x32
// sprites (assets/sprites/panel_tile.bmp) rather than a second background
// layer, since GBA windowing would be needed to show only part of a second
// bg layer and this is much simpler for a flat-color strip.
namespace sg::ui
{

class Panel
{

public:
    Panel();

    // Phase 4g's title screen/intro hide this too — otherwise the bottom
    // band's solid backdrop clips 32px off the bottom of full-screen art
    // that has nothing else to do with the panel (the title card, the
    // castle menu art).
    void set_visible(bool visible);

private:
    bn::vector<bn::sprite_ptr, 4> _tiles;
};

}

#endif
