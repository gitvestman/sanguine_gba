#include "courtyard.h"

#include "../ui/room_screen.h"
#include "../engine/game_state.h"
#include "../engine/inventory.h"
#include "../engine/rng.h"

namespace sg::rooms
{

namespace
{
    constexpr bn::string_view long_description =
        "A muddy stone courtyard around an old well. Doors lead north and west; a lowered "
        "portcullis blocks the south gate.";
    constexpr bn::string_view short_description = "The courtyard.";
}

void Courtyard::describe(bn::string<768>& out, bool force_long)
{
    out.append((! force_long && visited) ? short_description : long_description);
    visited = true;
}

void Courtyard::build_choices(bn::ivector<Choice>& out)
{
    out.push_back({ "Look", 0 });
    out.push_back({ "N", 1 });
    out.push_back({ "W", 2 });

    if(portcullis_raised)
    {
        out.push_back({ "S", 3 });
    }
    else
    {
        out.push_back({ "Crawl south", 6 });
        out.push_back({ "Raise portcullis", 7 });
    }

    if(! _well_inspected)
    {
        out.push_back({ "Inspect well", 4 });
    }

    if(! _searched)
    {
        out.push_back({ "Search", 5 });
    }
}

void Courtyard::on_choice(int action_id, RoomScreen& screen)
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
        screen.goto_room(RoomId::Kitchen);
        break;

    case 3:
        screen.goto_room(RoomId::MainGate);
        break;

    case 4:
    {
        _well_inspected = true;
        Result result = screen.roll(Attribute::STR, 12, false, "Pull the rope");

        if(result == Result::SUCCESS)
        {
            Inventory::instance().add(ItemId::HEALING_TINCTURE);
            screen.show_text("You haul up the bucket. A healing tincture rests inside, "
                              "wrapped in oilcloth.");
        }
        else
        {
            screen.show_text("The rope frays and snaps. Whatever was down there stays "
                              "down there.");
        }

        break;
    }

    case 5:
        _searched = true;
        screen.show_text("Rotting crates and a rusted cart wheel. Nothing worth taking.");
        break;

    case 6:
    {
        Result result = screen.roll(Attribute::DEX, 10, true, "Crawl south");

        if(result == Result::SUCCESS)
        {
            screen.show_text("You squeeze under the lowered portcullis.");
            screen.goto_room(RoomId::MainGate);
        }
        else if(result == Result::FAIL || result == Result::CRITICAL_FAIL)
        {
            int damage = rng().get_int(1, 7);
            GameState::instance().take_damage(damage);

            bn::string<160> text = "A spike catches your side as you squeeze under. You take ";
            text.append(bn::to_string<8>(damage));
            text.append(" damage.");
            screen.show_text(text);
        }

        break;
    }

    case 7:
    {
        Result result = screen.roll(Attribute::STR, 12, false, "Raise portcullis");

        if(result == Result::SUCCESS)
        {
            portcullis_raised = true;
            screen.show_text("The windlass groans, and the portcullis grinds upward for good.");
        }
        else
        {
            int damage = rng().get_int(1, 7);
            GameState::instance().take_damage(damage);

            bn::string<160> text = "The crank slips and the handle strikes you. You take ";
            text.append(bn::to_string<8>(damage));
            text.append(" damage.");
            screen.show_text(text);
        }

        break;
    }

    default:
        break;
    }
}

uint16_t Courtyard::save_state() const
{
    uint16_t bits = Room::save_state();
    if(_searched) bits |= 1 << 1;
    if(_well_inspected) bits |= 1 << 2;
    if(portcullis_raised) bits |= 1 << 3;
    return bits;
}

void Courtyard::load_state(uint16_t bits)
{
    Room::load_state(bits);
    _searched = bits & (1 << 1);
    _well_inspected = bits & (1 << 2);
    portcullis_raised = bits & (1 << 3);
}

}
