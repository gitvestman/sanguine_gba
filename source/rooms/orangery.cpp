#include "orangery.h"

#include "../ui/room_screen.h"

namespace sg::rooms
{

namespace
{
    constexpr bn::string_view long_description =
        "A shattered greenhouse choked with overgrown plants that sway without any wind.";
    constexpr bn::string_view short_description = "The orangery.";
}

void Orangery::describe(bn::string<768>& out, bool force_long)
{
    out.append((! force_long && visited) ? short_description : long_description);
    visited = true;
}

void Orangery::build_choices(bn::ivector<Choice>& out)
{
    out.push_back({ "Look", 0 });
    out.push_back({ "E", 1 });

    if(! _searched)
    {
        out.push_back({ "Search", 2 });
    }
}

void Orangery::on_choice(int action_id, RoomScreen& screen)
{
    switch(action_id)
    {

    case 0:
    {
        bn::string<768> text;
        describe(text, true);
        screen.show_text(text);
        break;
    }

    case 1:
        screen.goto_room(RoomId::GreatHall);
        break;

    case 2:
        _searched = true;
        screen.show_text("Cracked pots and tangled vines. Nothing worth taking.");
        break;

    default:
        break;
    }
}

uint8_t Orangery::save_state() const
{
    uint8_t bits = Room::save_state();
    if(_searched) bits |= 1 << 1;
    return bits;
}

void Orangery::load_state(uint8_t bits)
{
    Room::load_state(bits);
    _searched = bits & (1 << 1);
}

}
