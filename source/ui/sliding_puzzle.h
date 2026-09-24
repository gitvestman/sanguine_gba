#ifndef SG_UI_SLIDING_PUZZLE_H
#define SG_UI_SLIDING_PUZZLE_H

#include "bn_vector.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_text_generator.h"

// A classic 8-tile (2x4) sliding puzzle, ported loosely from
// D:\Workspace\Godot\sanguine\rooms\puzzle.gd's mouse-driven tile grid — a
// D-pad cursor + slide-into-the-gap mechanic instead of mouse clicks, since
// the GBA has no pointer. Chapel is the only room that uses this so far
// (the gate puzzle).
//
// Progress persists only for the current play session (the arrangement
// lives in this object, owned by Chapel, not written to SRAM) — closing
// the puzzle and reopening it later in the same session resumes where you
// left off, but a save/reload always restarts it scrambled. Good enough
// for a side puzzle; revisit if that ever feels wrong in play.
namespace sg
{
class RoomScreen;
}

namespace sg::ui
{

class SlidingPuzzle
{

public:
    // Blocks until the player solves it (returns true) or backs out with B
    // (returns false, keeping progress for next time). Chapel can own this
    // as a plain default-constructed member — constructed before
    // RoomScreen exists (room registry singletons are built by
    // SaveGame::load_or_new_game(), which runs before main.cpp's
    // RoomScreen), so it can't hold a text_generator reference from
    // construction; play() borrows screen's instead, only for the
    // duration of the call.
    bool play(RoomScreen& screen);

private:
    static constexpr int columns = 2;
    static constexpr int rows = 4;
    static constexpr int cell_count = columns * rows;

    void _scramble_if_needed();
    void _create_sprites();
    void _reposition();
    void _update_cursor(bn::sprite_text_generator& generator);
    bool _solved() const;

    // _cells[i] is the tile id (1..7) occupying grid cell i, or 0 for the
    // empty slot; index = row * columns + col.
    bn::vector<uint8_t, cell_count> _cells;
    bn::vector<bn::sprite_ptr, cell_count - 1> _tile_sprites;
    bn::vector<bn::sprite_ptr, 4> _cursor_sprites;
    int _selected = 0;
    bool _initialized = false;
};

}

#endif
