#ifndef SG_UI_PANEL_H
#define SG_UI_PANEL_H

#include "bn_sprite_ptr.h"
#include "bn_vector.h"

// The solid backdrop behind the bottom text/choice band, so text stays
// legible over whatever room art is behind it. Built from 4 tiled 64x64
// sprites (assets/sprites/panel_tile.bmp) rather than a second background
// layer, since GBA windowing would be needed to show only part of a second
// bg layer and this is much simpler for a flat-color strip.
namespace sg::ui
{

class Panel
{

public:
    Panel();

private:
    bn::vector<bn::sprite_ptr, 4> _tiles;
};

}

#endif
