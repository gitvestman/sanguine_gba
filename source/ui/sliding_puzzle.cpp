#include "sliding_puzzle.h"

#include "bn_keypad.h"
#include "bn_core.h"
#include "bn_sprite_item.h"

#include "bn_sprite_items_puzzle_tile1.h"
#include "bn_sprite_items_puzzle_tile2.h"
#include "bn_sprite_items_puzzle_tile3.h"
#include "bn_sprite_items_puzzle_tile4.h"
#include "bn_sprite_items_puzzle_tile5.h"
#include "bn_sprite_items_puzzle_tile6.h"
#include "bn_sprite_items_puzzle_tile7.h"

#include "../engine/rng.h"
#include "room_screen.h"

namespace sg::ui
{

namespace
{
    // Same vertical budget as the stats sidebar (-80..48, 128px), centered
    // horizontally between the two 32px side panels.
    constexpr bn::fixed cell_size = 32;
    constexpr bn::fixed grid_left = -32;
    constexpr bn::fixed grid_top = -80;

    bn::fixed cell_x(int col)
    {
        return grid_left + cell_size * col + cell_size / 2;
    }

    bn::fixed cell_y(int row)
    {
        return grid_top + cell_size * row + cell_size / 2;
    }

    // The true solved layout, per the source project's puzzle.gd
    // (solved_state = [1, 0, 2, 3, 4, 5, 6, 7]) — confirmed by diffing
    // graphics/puzzle/komplett.png (the assembled picture) against each
    // tile{N}.png fragment cell by cell: tile1 sits top-left, the gap is
    // top-RIGHT (index 1, not the last cell), and tile2..tile7 fill the
    // rest in reading order after that. Getting this wrong doesn't just
    // look off — a sliding puzzle's legal moves only ever reach permutations
    // in the same parity class as whatever state you scramble from, so
    // scrambling from the wrong "solved" layout (as this code used to,
    // assuming the gap was last) made the actual correct picture
    // unreachable, not merely mislabeled.
    constexpr uint8_t solved_layout[8] = { 1, 0, 2, 3, 4, 5, 6, 7 };

    bn::sprite_item tile_item(int tile_id)
    {
        switch(tile_id)
        {

        case 1:
            return bn::sprite_items::puzzle_tile1;

        case 2:
            return bn::sprite_items::puzzle_tile2;

        case 3:
            return bn::sprite_items::puzzle_tile3;

        case 4:
            return bn::sprite_items::puzzle_tile4;

        case 5:
            return bn::sprite_items::puzzle_tile5;

        case 6:
            return bn::sprite_items::puzzle_tile6;

        case 7:
        default:
            return bn::sprite_items::puzzle_tile7;
        }
    }
}

void SlidingPuzzle::_scramble_if_needed()
{
    if(_initialized)
    {
        return;
    }

    _cells.clear();

    for(uint8_t v : solved_layout)
    {
        _cells.push_back(v);
    }

    int empty_index = 1;

    // Perform a long run of random *valid* slides from the solved state
    // rather than a random permutation — an 8-piece sliding puzzle is only
    // solvable from exactly half of all permutations, and this guarantees
    // a solvable (if occasionally already-solved, harmlessly) result
    // without needing a parity check.
    for(int i = 0; i < 80; ++i)
    {
        bn::vector<int, 4> neighbors;
        int row = empty_index / columns;
        int col = empty_index % columns;

        if(row > 0) neighbors.push_back(empty_index - columns);
        if(row < rows - 1) neighbors.push_back(empty_index + columns);
        if(col > 0) neighbors.push_back(empty_index - 1);
        if(col < columns - 1) neighbors.push_back(empty_index + 1);

        int swap_with = neighbors[rng().get_int(0, neighbors.size())];
        _cells[empty_index] = _cells[swap_with];
        _cells[swap_with] = 0;
        empty_index = swap_with;
    }

    _initialized = true;
}

void SlidingPuzzle::_create_sprites()
{
    if(! _tile_sprites.empty())
    {
        return;
    }

    // One sprite per tile id (1..7), indexed by id - 1 — fixed identity
    // that _reposition() moves around as cells swap, rather than one
    // sprite per grid slot (whose meaning would silently invert the moment
    // the empty slot's scan position changes).
    for(int tile_id = 1; tile_id <= cell_count - 1; ++tile_id)
    {
        _tile_sprites.push_back(tile_item(tile_id).create_sprite(0, 0));
    }

    _reposition();
}

void SlidingPuzzle::_reposition()
{
    for(int i = 0; i < cell_count; ++i)
    {
        int tile_id = _cells[i];

        if(tile_id != 0)
        {
            int row = i / columns;
            int col = i % columns;
            _tile_sprites[tile_id - 1].set_position(cell_x(col), cell_y(row));
        }
    }
}

void SlidingPuzzle::_update_cursor(bn::sprite_text_generator& generator)
{
    _cursor_sprites.clear();
    generator.set_left_alignment();

    int row = _selected / columns;
    int col = _selected % columns;
    bn::fixed marker_x = cell_x(col) - cell_size / 2 - 2;
    bn::fixed marker_y = cell_y(row) - 2;
    generator.generate(marker_x, marker_y, ">", _cursor_sprites);
}

bool SlidingPuzzle::_solved() const
{
    for(int i = 0; i < cell_count; ++i)
    {
        if(_cells[i] != solved_layout[i])
        {
            return false;
        }
    }

    return true;
}

bool SlidingPuzzle::play(RoomScreen& screen)
{
    bn::sprite_text_generator& generator = screen.text_generator();
    _scramble_if_needed();
    _create_sprites();
    _reposition();
    _update_cursor(generator);

    while(true)
    {
        bool moved = false;

        if(bn::keypad::right_pressed() && _selected % columns < columns - 1)
        {
            ++_selected;
            moved = true;
        }
        else if(bn::keypad::left_pressed() && _selected % columns > 0)
        {
            --_selected;
            moved = true;
        }
        else if(bn::keypad::down_pressed() && _selected / columns < rows - 1)
        {
            _selected += columns;
            moved = true;
        }
        else if(bn::keypad::up_pressed() && _selected / columns > 0)
        {
            _selected -= columns;
            moved = true;
        }

        if(moved)
        {
            _update_cursor(generator);
        }

        if(bn::keypad::a_pressed())
        {
            int empty_index = -1;

            for(int i = 0; i < cell_count; ++i)
            {
                if(_cells[i] == 0)
                {
                    empty_index = i;
                    break;
                }
            }

            int row = _selected / columns;
            int col = _selected % columns;
            int empty_row = empty_index / columns;
            int empty_col = empty_index % columns;
            int col_diff = col - empty_col < 0 ? empty_col - col : col - empty_col;
            int row_diff = row - empty_row < 0 ? empty_row - row : row - empty_row;
            bool adjacent = (row == empty_row && col_diff == 1) || (col == empty_col && row_diff == 1);

            if(adjacent)
            {
                _cells[empty_index] = _cells[_selected];
                _cells[_selected] = 0;
                _reposition();

                if(_solved())
                {
                    _tile_sprites.clear();
                    _cursor_sprites.clear();
                    bn::core::update();
                    return true;
                }
            }
        }

        if(bn::keypad::b_pressed())
        {
            _tile_sprites.clear();
            _cursor_sprites.clear();
            bn::core::update();
            return false;
        }

        bn::core::update();
    }
}

}
