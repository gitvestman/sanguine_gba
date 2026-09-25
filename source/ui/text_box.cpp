#include "text_box.h"

#include "bn_keypad.h"
#include "bn_algorithm.h"

namespace sg::ui
{

namespace
{
    constexpr bn::fixed text_x = -112;
    constexpr bn::fixed line_height = 8;
    constexpr int max_width_px = 224;

    using Line = bn::string<48>;

    void wrap_paragraph(const bn::sprite_text_generator& generator, bn::string_view paragraph,
                         bn::ivector<Line>& out)
    {
        if(paragraph.empty())
        {
            out.emplace_back();
            return;
        }

        Line current;
        const int capacity = current.max_size();

        bn::string_view remaining = paragraph;
        while(! remaining.empty())
        {
            int space = remaining.find(' ');
            bn::string_view word = (space == bn::string_view::npos) ? remaining : remaining.substr(0, space);

            // Guard against a single "word" longer than the line buffer itself
            // (shouldn't happen with real prose, but must never overrun the
            // fixed-capacity buffer below).
            if(word.size() > capacity)
            {
                word = word.substr(0, capacity);
            }

            int needed = word.size() + (current.empty() ? 0 : 1);
            bool fits = current.size() + needed <= capacity;

            if(fits)
            {
                Line candidate = current;
                if(! candidate.empty())
                {
                    candidate.push_back(' ');
                }
                candidate.append(word);
                fits = generator.width(candidate) <= max_width_px;

                if(fits)
                {
                    current = candidate;
                }
            }

            if(! fits)
            {
                if(! current.empty())
                {
                    out.push_back(current);
                    current.clear();
                }

                current.append(word);
            }

            remaining = (space == bn::string_view::npos) ? bn::string_view() : remaining.substr(space + 1);
        }

        out.push_back(current);
    }

    void wrap_text(const bn::sprite_text_generator& generator, bn::string_view text, bn::ivector<Line>& out)
    {
        bn::string_view remaining = text;
        while(true)
        {
            int nl = remaining.find('\n');
            bn::string_view paragraph = (nl == bn::string_view::npos) ? remaining : remaining.substr(0, nl);
            wrap_paragraph(generator, paragraph, out);

            if(nl == bn::string_view::npos)
            {
                break;
            }

            remaining = remaining.substr(nl + 1);
        }
    }
}

TextBox::TextBox(bn::sprite_text_generator& generator, int lines_per_page, bn::fixed top_y) :
    _generator(generator),
    _lines_per_page(lines_per_page),
    _top_y(top_y)
{
}

void TextBox::show(const bn::string_view& text)
{
    _lines.clear();
    wrap_text(_generator, text, _lines);
    _line_count = _lines.size();

    _page_count = bn::max((_line_count + _lines_per_page - 1) / _lines_per_page, 1);
    _page = 0;
    _active = true;
    _show_page(_page);
}

void TextBox::_show_page(int page_index)
{
    _sprites.clear();
    _generator.set_left_alignment();

    // generate()'s y is the vertical CENTER of the glyph row (confirmed from
    // bn_sprite_text_generator.cpp's painters), not its top — generate_top_left
    // isn't used anywhere in Butano's own examples/games and its coordinate
    // frame doesn't match plain generate()'s, so this adds half the 8px font
    // height itself instead of relying on it.
    constexpr bn::fixed half_glyph_height = 4;

    int first_line = page_index * _lines_per_page;
    int last_line = bn::min(first_line + _lines_per_page, _line_count);

    for(int i = first_line; i < last_line; ++i)
    {
        bn::fixed y = _top_y + bn::fixed((i - first_line) * int(line_height)) + half_glyph_height;
        _generator.generate(text_x, y, _lines[i], _sprites);
    }

    bn::fixed hint_y = _top_y + bn::fixed(_lines_per_page * int(line_height)) + half_glyph_height;
    bn::string_view hint = (page_index + 1 < _page_count) ? "> A: More..." : "> A: Continue";
    _generator.generate(text_x, hint_y, hint, _sprites);
}

void TextBox::update()
{
    if(! _active)
    {
        return;
    }

    if(bn::keypad::a_pressed())
    {
        if(_page + 1 < _page_count)
        {
            ++_page;
            _show_page(_page);
        }
        else
        {
            _sprites.clear();
            _active = false;
        }
    }
}

}
