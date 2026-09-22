#include "homunculus_workshop.h"

#include "../ui/room_screen.h"

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
            // Godot starts a fight with the Homunculus here; combat isn't
            // ported yet (Phase 2), so it's resolved as a scare instead.
            screen.show_text(
                "The sigil is wrong, and the embryo leaps at you, snarling! (A fight would "
                "begin here -- not part of this build yet.) You shove the jar away and back "
                "out, shaken.");
        }

        break;
    }

    default:
        break;
    }
}

}
