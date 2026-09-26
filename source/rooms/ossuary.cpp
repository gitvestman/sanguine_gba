#include "ossuary.h"

#include "../ui/room_screen.h"
#include "chapel.h"

namespace sg::rooms
{

namespace
{
    constexpr bn::string_view long_description =
        "A silent stone chamber. Old bones, pulled from their alcoves, litter the floor. An "
        "iron gate to the east, and stairs up to a chapel.";
    constexpr bn::string_view short_description = "The ossuary.";
}

void Ossuary::describe(bn::string<768>& out, bool force_long)
{
    out.append((! force_long && visited) ? short_description : long_description);
    visited = true;
}

void Ossuary::build_choices(bn::ivector<Choice>& out)
{
    out.push_back({ "Look", 0 });
    out.push_back({ "E", 1 });

    // Only a real way back once the gate above is actually open — Ossuary
    // is also reachable directly from CellarHall, so without this check a
    // player could climb up through a gate they never solved (the source
    // project has no "up" choice here at all; the gate is otherwise a
    // one-way door, only opened from Chapel's side).
    if(static_cast<Chapel&>(room(RoomId::Chapel)).gate_unlocked())
    {
        out.push_back({ "Up", 5 });
    }

    if(! _search_skeleton_key)
    {
        out.push_back({ "Search bones", 4 });
    }
}

void Ossuary::on_choice(int action_id, RoomScreen& screen)
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
        screen.goto_room(RoomId::CellarHall);
        break;

    case 5:
        static_cast<Chapel&>(room(RoomId::Chapel)).set_entered_from_ossuary(true);
        screen.goto_room(RoomId::Chapel);
        break;

    case 4:
    {
        Result result = screen.roll(Attribute::INT_, 12, true, "Search bones");

        if(result == Result::SUCCESS)
        {
            Inventory::instance().add(ItemId::SKULL_KEY);
            _search_skeleton_key = true;
            screen.show_text("You find a skull key.");
        }
        else if(result == Result::FAIL || result == Result::CRITICAL_FAIL)
        {
            _search_skeleton_key = true;
            screen.show_text("You find nothing.");
        }

        break;
    }

    default:
        break;
    }
}

uint16_t Ossuary::save_state() const
{
    uint16_t bits = Room::save_state();
    if(_search_skeleton_key) bits |= 1 << 1;
    return bits;
}

void Ossuary::load_state(uint16_t bits)
{
    Room::load_state(bits);
    _search_skeleton_key = bits & (1 << 1);
}

}
