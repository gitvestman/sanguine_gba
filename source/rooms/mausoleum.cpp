#include "mausoleum.h"

#include "../ui/room_screen.h"
#include "../engine/inventory.h"
#include "../engine/rng.h"

namespace sg::rooms
{

namespace
{
    constexpr bn::string_view long_description =
        "A small chamber built around a stone sarcophagus, cobwebs thick in every corner.";
    constexpr bn::string_view short_description = "The mausoleum.";
}

void Mausoleum::describe(bn::string<768>& out, bool force_long)
{
    out.append((! force_long && visited) ? short_description : long_description);
    visited = true;
}

void Mausoleum::build_choices(bn::ivector<Choice>& out)
{
    out.push_back({ "Look", 0 });
    out.push_back({ "E", 1 });

    if(! _inspected)
    {
        out.push_back({ "Inspect sarcophagus", 2 });
    }

    if(_inspected && ! _push_success)
    {
        out.push_back({ "Push lid", 3 });
    }

    if(_item_found && ! _item_taken)
    {
        out.push_back({ "Take item", 4 });
        out.push_back({ "Leave it", 5 });
    }
}

void Mausoleum::on_choice(int action_id, RoomScreen& screen)
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
        _inspected = true;
        screen.show_text("The sarcophagus lid sits slightly ajar, as if disturbed long ago.");
        break;

    case 3:
    {
        Result result = screen.roll(Attribute::STR, 10, false, "Push lid");
        _push_success = true;

        if(result == Result::SUCCESS)
        {
            if(rng().get_int(0, 2) == 0)
            {
                screen.show_text("The lid grinds aside. Inside, only dust and old bones.");
            }
            else
            {
                _item_found = true;
                screen.show_text("The lid grinds aside. Something glints among the bones — a "
                                  "silver dagger.");
            }
        }
        else
        {
            screen.show_text("The lid won't budge. You give up before it crushes your "
                              "fingers.");
        }

        break;
    }

    case 4:
        Inventory::instance().add(ItemId::SILVER_DAGGER);
        _item_taken = true;
        screen.show_text("You take the silver dagger.");
        break;

    case 5:
        screen.show_text("You leave it be.");
        break;

    default:
        break;
    }
}

uint16_t Mausoleum::save_state() const
{
    uint16_t bits = Room::save_state();
    if(_inspected) bits |= 1 << 1;
    if(_push_success) bits |= 1 << 2;
    if(_item_found) bits |= 1 << 3;
    if(_item_taken) bits |= 1 << 4;
    return bits;
}

void Mausoleum::load_state(uint16_t bits)
{
    Room::load_state(bits);
    _inspected = bits & (1 << 1);
    _push_success = bits & (1 << 2);
    _item_found = bits & (1 << 3);
    _item_taken = bits & (1 << 4);
}

}
