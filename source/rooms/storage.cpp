#include "storage.h"

#include "../ui/room_screen.h"

namespace sg::rooms
{

namespace
{
    constexpr bn::string_view long_description =
        "A cramped storeroom of moldy furniture and cobwebs. A strange metal pedestal stands "
        "in the corner.";
    constexpr bn::string_view short_description = "The storage room.";
}

void Storage::describe(bn::string<768>& out, bool force_long)
{
    out.append((! force_long && visited) ? short_description : long_description);
    visited = true;
}

void Storage::build_choices(bn::ivector<Choice>& out)
{
    out.push_back({ "Look", 0 });
    out.push_back({ "S", 1 });

    if(! _searched)
    {
        out.push_back({ "Search", 2 });
    }
}

void Storage::on_choice(int action_id, RoomScreen& screen)
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
        screen.goto_room(RoomId::Kitchen);
        break;

    case 2:
        _searched = true;
        Inventory::instance().add(ItemId::LOCKPICKS);
        screen.show_text("Tucked in a drawer, you find a set of lockpicks.");
        break;

    default:
        break;
    }
}

uint8_t Storage::save_state() const
{
    uint8_t bits = Room::save_state();
    if(_searched) bits |= 1 << 1;
    return bits;
}

void Storage::load_state(uint8_t bits)
{
    Room::load_state(bits);
    _searched = bits & (1 << 1);
}

}
