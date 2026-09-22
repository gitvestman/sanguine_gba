#ifndef SG_UI_TEXT_BOX_H
#define SG_UI_TEXT_BOX_H

#include "bn_vector.h"
#include "bn_string.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_text_generator.h"

// Paginated story-text box: the GBA has no RichTextLabel/scrollback the way
// Sanguine's PC version does, so this deliberately simplifies to the genre's
// usual modal "press A to continue" idiom (see IMPLEMENTATION_PLAN.md's Text
// rendering section) instead of a persistent scrolling log.
namespace sg::ui
{

class TextBox
{

public:
    // 3 content lines + 1 hint line fill the 32px bottom panel band exactly
    // (see ui/panel.cpp) — smaller than the vertical slice's first pass so
    // more of the room art stays visible; long descriptions just take a few
    // more "A: More..." pages to get through.
    static constexpr int max_lines_per_page = 3;
    static constexpr int max_total_lines = 48;

    explicit TextBox(bn::sprite_text_generator& generator);

    // Word-wraps and paginates `text`, then displays its first page.
    void show(const bn::string_view& text);

    // True while there is a page on screen waiting for an A press.
    bool active() const
    {
        return _active;
    }

    // Call once per frame while active(). Advances to the next page (or
    // closes the box on the last page) when A is pressed.
    void update();

private:
    void _show_page(int page_index);

    bn::sprite_text_generator& _generator;
    bn::vector<bn::string<48>, max_total_lines> _lines;
    bn::vector<bn::sprite_ptr, 96> _sprites;
    int _line_count = 0;
    int _page = 0;
    int _page_count = 0;
    bool _active = false;
};

}

#endif
