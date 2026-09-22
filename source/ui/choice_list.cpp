#include "choice_list.h"

#include "bn_keypad.h"

namespace sg::ui
{

namespace
{
    constexpr bn::fixed text_x = -112;
    // Same 32px bottom panel band as TextBox (screen y:[48,80) — see
    // ui/panel.cpp), laid out as a horizontal flow (left to right, wrapping
    // to a new row when a row would overflow the panel width) rather than
    // one item per row, so a handful of short choices ("Look around",
    // "Door east", ...) don't burn through the whole band's height.
    constexpr bn::fixed text_top_y = 48;
    constexpr bn::fixed row_height = 8;
    constexpr bn::fixed half_glyph_height = 4;
    constexpr int max_width_px = 224;
    constexpr int item_gap_px = 10;
}

ChoiceList::ChoiceList(bn::sprite_text_generator& generator) :
    _generator(generator)
{
}

void ChoiceList::set_options(const bn::ivector<bn::string<40>>& labels)
{
    _labels.clear();

    for(const bn::string<40>& label : labels)
    {
        _labels.push_back(label);
    }

    _selected = 0;
    _regenerate();
}

void ChoiceList::clear()
{
    _labels.clear();
    _sprites.clear();
}

void ChoiceList::_regenerate()
{
    _sprites.clear();
    _generator.set_left_alignment();

    bn::fixed cursor_x = text_x;
    int row = 0;

    for(int i = 0; i < _labels.size(); ++i)
    {
        bn::string<40> line = (i == _selected) ? bn::string<40>("> ") : bn::string<40>("  ");
        line.append(_labels[i]);

        int item_width = _generator.width(line);

        if(cursor_x != text_x && cursor_x + item_width > text_x + max_width_px)
        {
            cursor_x = text_x;
            ++row;
        }

        bn::fixed y = text_top_y + (row_height * row) + half_glyph_height;
        _generator.generate(cursor_x, y, line, _sprites);

        cursor_x += item_width + item_gap_px;
    }
}

bn::optional<int> ChoiceList::update()
{
    if(_labels.empty())
    {
        return bn::optional<int>();
    }

    bool changed = false;

    if(bn::keypad::right_pressed() || bn::keypad::down_pressed())
    {
        _selected = (_selected + 1) % _labels.size();
        changed = true;
    }
    else if(bn::keypad::left_pressed() || bn::keypad::up_pressed())
    {
        _selected = (_selected - 1 + _labels.size()) % _labels.size();
        changed = true;
    }

    if(changed)
    {
        _regenerate();
    }

    if(bn::keypad::a_pressed())
    {
        int chosen = _selected;
        clear();
        return chosen;
    }

    return bn::optional<int>();
}

}
