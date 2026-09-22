#include "the_machine.h"

#include "../ui/room_screen.h"

namespace sg::rooms
{

namespace
{
    constexpr bn::string_view long_description =
        "A strange machine fills the room: levers, gears, and meters behind glass. Worn but "
        "intact.";
    constexpr bn::string_view short_description = "The machine.";
}

void TheMachine::describe(bn::string<768>& out, bool force_long)
{
    out.append((! force_long && visited) ? short_description : long_description);
    visited = true;
}

void TheMachine::build_choices(bn::ivector<Choice>& out)
{
    out.push_back({ "Look", 0 });
    out.push_back({ "E", 1 });
    out.push_back({ "Use machine", 2 });

    if(Inventory::instance().has(ItemId::COG) && ! _machine_fixed)
    {
        out.push_back({ "Add Cog", 3 });
    }
}

void TheMachine::on_choice(int action_id, RoomScreen& screen)
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
        screen.goto_room(RoomId::StoneCorridor);
        break;

    case 2:
        if(_machine_fixed)
        {
            if(! _machine_used)
            {
                _machine_used = true;
                Inventory::instance().add(ItemId::HEALING_TINCTURE);
                screen.show_text("The machine hums to life. A healing tincture drops out.");
            }
            else
            {
                screen.show_text("The machine hums. It has already yielded its secret.");
            }
        }
        else
        {
            screen.show_text("Nothing happens -- a brass gear is missing.");
        }
        break;

    case 3:
        Inventory::instance().remove(ItemId::COG);
        _machine_fixed = true;
        screen.show_text("You push the cogwheel into place. The machine activates, ending "
                          "with a heavy thud from a drawer below.");
        break;

    default:
        break;
    }
}

}
