#ifndef SG_UI_CHOICE_LIST_H
#define SG_UI_CHOICE_LIST_H

#include "bn_vector.h"
#include "bn_string.h"
#include "bn_sprite_ptr.h"
#include "bn_optional.h"
#include "bn_sprite_text_generator.h"

// D-pad-navigable menu drawn in the same bottom band as TextBox (never shown
// at the same time — see RoomScreen), laid out as a horizontal flow (see
// _regenerate) rather than one choice per row, to keep the band short. The
// selected item is marked with a leading "> " instead of a separate cursor
// sprite, since with left-aligned single-glyph-prefix text that's simplest
// and needs no extra art. Left/right (and up/down as aliases) move the
// selection; A confirms.
namespace sg::ui
{

class ChoiceList
{

public:
    // Must be >= the largest choice list any room ever builds, plus the
    // global "Inv" entry RoomScreen::run() appends to every room (FancyCorridor01
    // is currently the worst case: Look + 7 directions + "Talk to the
    // Countess" + Inv = 10) — RoomScreen's own per-frame buffers match this
    // as the true upper bound; going over silently crashes (bn::vector
    // push_back asserts) rather than dropping choices.
    static constexpr int max_options = 10;

    explicit ChoiceList(bn::sprite_text_generator& generator);

    void set_options(const bn::ivector<bn::string<40>>& labels);

    void clear();

    bool active() const
    {
        return ! _labels.empty();
    }

    // Returns the chosen index once A is pressed this frame, else nullopt.
    // Call once per frame while active().
    bn::optional<int> update();

private:
    void _regenerate();

    bn::sprite_text_generator& _generator;
    bn::vector<bn::string<40>, max_options> _labels;
    bn::vector<bn::sprite_ptr, max_options * 8> _sprites;
    int _selected = 0;
};

}

#endif
