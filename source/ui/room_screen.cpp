#include "room_screen.h"

#include "bn_core.h"
#include "bn_string.h"

#include "common_variable_8x8_sprite_font.h"

#include "../engine/dice.h"

namespace sg
{

RoomScreen::RoomScreen() :
    _text_generator(common::variable_8x8_sprite_font),
    _text_box(_text_generator),
    _choices(_text_generator)
{
}

void RoomScreen::show_text(const bn::string_view& text)
{
    _text_box.show(text);

    while(_text_box.active())
    {
        _text_box.update();
        bn::core::update();
    }
}

Result RoomScreen::roll(Attribute attribute, int dc, bool cancelable, const bn::string_view& title)
{
    return Dice::roll_check(_text_box, _choices, attribute, dc, cancelable, title);
}

void RoomScreen::goto_room(RoomId next, const bn::string_view& transition_text)
{
    if(! transition_text.empty())
    {
        show_text(transition_text);
    }

    _enter_room(next);
}

void RoomScreen::_enter_room(RoomId id)
{
    _current_room = id;

    // The bottom 32px panel band (see ui/panel.cpp) covers part of the art;
    // shifting the background up centers the remaining 180x140 room
    // painting in the visible 128px strip above the panel instead of in the
    // full 160px screen, so it's clipped by roughly the same amount at the
    // top (by the screen edge) as it is at the bottom (by the panel).
    _background = background_item(id).create_bg(0, -16);

    Room& r = room(id);
    bn::string<768> description;
    r.describe(description);
    show_text(description);
    r.on_enter(*this);
}

void RoomScreen::run(RoomId start)
{
    _enter_room(start);

    while(true)
    {
        Room& r = room(_current_room);

        bn::vector<Choice, 8> choices;
        r.build_choices(choices);

        bn::vector<bn::string<40>, 8> labels;
        for(const Choice& choice : choices)
        {
            labels.push_back(choice.text);
        }

        _choices.set_options(labels);

        int picked = -1;
        while(picked < 0)
        {
            bn::optional<int> result = _choices.update();

            if(result.has_value())
            {
                picked = result.value();
            }

            bn::core::update();
        }

        r.on_choice(choices[picked].action_id, *this);
    }
}

}
