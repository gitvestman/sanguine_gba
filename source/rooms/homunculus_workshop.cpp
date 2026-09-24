#include "homunculus_workshop.h"

#include "../ui/room_screen.h"
#include "../engine/combat.h"

namespace sg::rooms
{

namespace
{
    constexpr bn::string_view long_description =
        "A foul workshop. A workbench holds strange fluids and a jar with a small, unformed "
        "embryo.";
    constexpr bn::string_view short_description = "The homunculus workshop.";
}

void HomunculusWorkshop::describe(bn::string<768>& out, bool force_long)
{
    out.append((! force_long && visited) ? short_description : long_description);
    visited = true;
}

void HomunculusWorkshop::build_choices(bn::ivector<Choice>& out)
{
    out.push_back({ "Look", 0 });
    out.push_back({ "W", 1 });

    if(! _progress_forward)
    {
        out.push_back({ "Search", 2 });
    }
    else if(! _found_artifact)
    {
        out.push_back({ "Ignore", 2 });
        out.push_back({ "Create homunculus", 3 });
    }
}

void HomunculusWorkshop::on_choice(int action_id, RoomScreen& screen)
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
        if(! _progress_forward)
        {
            _progress_forward = true;
            screen.show_text(
                "You find notes on creating a homunculus -- a small, loyal guardian. It "
                "cannot be fully trusted, but everything needed is right here.");
        }
        else
        {
            _found_artifact = true;
            screen.show_text("It cannot be trusted. You leave it be, for now.");
        }
        break;

    case 3:
    {
        Result result = screen.roll(Attribute::INT_, 10, false, "Create homunculus");

        if(result == Result::SUCCESS)
        {
            _found_artifact = true;
            Inventory::instance().add(ItemId::HOMUNCULUS);
            screen.show_text(
                "You feed the embryo your blood and chant the words. It grows into a small, "
                "formidable creature that watches you, waiting for a command.");
        }
        else
        {
            _found_artifact = true;
            screen.show_text("The sigil is wrong, and the embryo leaps at you, snarling!");

            if(Combat::fight(screen, EnemyId::Homunculus) == CombatResult::WON)
            {
                screen.show_text("You shove the twitching remains back into the jar.");
            }
            else
            {
                screen.show_text("You back out of the workshop, shaken.");
            }
        }

        break;
    }

    default:
        break;
    }
}

uint8_t HomunculusWorkshop::save_state() const
{
    uint8_t bits = Room::save_state();
    if(_progress_forward) bits |= 1 << 1;
    if(_found_artifact) bits |= 1 << 2;
    return bits;
}

void HomunculusWorkshop::load_state(uint8_t bits)
{
    Room::load_state(bits);
    _progress_forward = bits & (1 << 1);
    _found_artifact = bits & (1 << 2);
}

}
