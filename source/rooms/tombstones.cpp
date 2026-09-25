#include "tombstones.h"

#include "../ui/room_screen.h"

namespace sg::rooms
{

namespace
{
    constexpr bn::string_view long_description =
        "A solitary hill among the graves. Two headstones stand here, and a third empty "
        "plot waits.";
    constexpr bn::string_view short_description = "The tombstones.";
}

void Tombstones::describe(bn::string<768>& out, bool force_long)
{
    out.append((! force_long && visited) ? short_description : long_description);
    visited = true;
}

void Tombstones::build_choices(bn::ivector<Choice>& out)
{
    out.push_back({ "Look", 0 });
    out.push_back({ "W", 1 });

    if(! _searched)
    {
        out.push_back({ "Search", 2 });
    }

    out.push_back({ "Inspect names", 3 });
}

void Tombstones::on_choice(int action_id, RoomScreen& screen)
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
        screen.goto_room(RoomId::Cemetery);
        break;

    case 2:
    {
        Result result = screen.roll(Attribute::INT_, 10, false, "Search");

        if(result == Result::SUCCESS)
        {
            screen.show_text("Beneath the moss, a faint inscription hints at a family long "
                              "forgotten.");
        }
        else
        {
            screen.show_text("You find nothing beyond weathered stone.");
        }

        _searched = true;
        break;
    }

    case 3:
        screen.show_text("The stones are humble but well-made. Both names are still clearly "
                          "legible: Olivia and Elena Lupescu.");
        break;

    default:
        break;
    }
}

uint16_t Tombstones::save_state() const
{
    uint16_t bits = Room::save_state();
    if(_searched) bits |= 1 << 1;
    return bits;
}

void Tombstones::load_state(uint16_t bits)
{
    Room::load_state(bits);
    _searched = bits & (1 << 1);
}

}
