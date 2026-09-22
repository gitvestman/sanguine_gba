#include "alchemical_laboratory.h"

#include "../ui/room_screen.h"
#include "../engine/game_state.h"

namespace sg::rooms
{

namespace
{
    constexpr bn::string_view long_description =
        "Greenish fumes fill this warm stone room. A workbench holds a small alchemy lab.";
    constexpr bn::string_view short_description = "The alchemical laboratory.";
}

void AlchemicalLaboratory::describe(bn::string<768>& out, bool force_long)
{
    out.append((! force_long && visited) ? short_description : long_description);
    visited = true;
}

void AlchemicalLaboratory::build_choices(bn::ivector<Choice>& out)
{
    out.push_back({ "Look", 0 });
    out.push_back({ "SW", 1 });

    if(! _progress_forward)
    {
        out.push_back({ "Investigate", 2 });
    }
    else if(! _found_potion)
    {
        out.push_back({ "Progress forward", 3 });
    }
}

void AlchemicalLaboratory::on_choice(int action_id, RoomScreen& screen)
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
        _progress_forward = true;
        screen.show_text("You step into the fumes. Every breath is a struggle.");
        break;

    case 3:
    {
        Result result = screen.roll(Attribute::STR, 10, true, "Progress forward");

        if(result == Result::SUCCESS)
        {
            Inventory::instance().add(ItemId::RESTORATIVE_MIXTURE);
            _found_potion = true;
            screen.show_text("You grab a test tube and rush back: a restorative mixture.");
        }
        else if(result == Result::FAIL || result == Result::CRITICAL_FAIL)
        {
            _found_potion = true;
            GameState::instance().take_damage(1);
            screen.show_text("The acidic mist is too much. You stumble back (-1 HP).");
        }

        break;
    }

    default:
        break;
    }
}

}
