#include "overgrown_pond.h"

#include "../ui/room_screen.h"
#include "../engine/game_state.h"

namespace sg::rooms
{

namespace
{
    constexpr bn::string_view long_description =
        "A stagnant pond, overgrown with reeds. The dark water seems to dare you to come "
        "closer.";
    constexpr bn::string_view inspected_description =
        "Ripples disturb the surface now and then, though nothing is visible.";
    constexpr bn::string_view stone1_description =
        "Your stone vanishes with barely a splash. Something below stirred.";
    constexpr bn::string_view stone2_description =
        "A second stone sinks. The water churns, and a shape rises just beneath the surface.";
    constexpr bn::string_view creature_description =
        "Your third stone strikes something solid. A pale, bloated shape lunges from the "
        "water!";
}

void OvergrownPond::describe(bn::string<768>& out, bool force_long)
{
    if(_encounter == PondEncounter::RESOLVED)
    {
        out.append((! force_long && visited) ? bn::string_view("The overgrown pond.") : long_description);
        visited = true;
        return;
    }

    switch(_encounter)
    {

    case PondEncounter::INSPECTED:
        out.append(inspected_description);
        break;

    case PondEncounter::STONE_1:
        out.append(stone1_description);
        break;

    case PondEncounter::STONE_2:
        out.append(stone2_description);
        break;

    case PondEncounter::CREATURE:
        out.append(creature_description);
        break;

    case PondEncounter::START:
    case PondEncounter::RESOLVED:
    default:
        out.append(long_description);
        break;
    }

    visited = true;
}

void OvergrownPond::build_choices(bn::ivector<Choice>& out)
{
    out.push_back({ "Look", 0 });

    switch(_encounter)
    {

    case PondEncounter::START:
        out.push_back({ "Inspect pond", 1 });
        out.push_back({ "W", 10 });
        out.push_back({ "N", 11 });
        break;

    case PondEncounter::INSPECTED:
        out.push_back({ "Throw stone", 2 });
        out.push_back({ "W", 10 });
        out.push_back({ "N", 11 });
        break;

    case PondEncounter::STONE_1:
        out.push_back({ "Throw another stone", 3 });
        out.push_back({ "W", 10 });
        out.push_back({ "N", 11 });
        break;

    case PondEncounter::STONE_2:
        out.push_back({ "Throw third stone", 4 });
        out.push_back({ "W", 10 });
        out.push_back({ "N", 11 });
        break;

    case PondEncounter::CREATURE:
        out.push_back({ "Evade", 5 });
        break;

    case PondEncounter::RESOLVED:
        out.push_back({ "W", 10 });
        out.push_back({ "N", 11 });
        break;
    }
}

void OvergrownPond::on_choice(int action_id, RoomScreen& screen)
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
        _encounter = PondEncounter::INSPECTED;
        screen.show_text(inspected_description);
        break;

    case 2:
        _encounter = PondEncounter::STONE_1;
        screen.show_text(stone1_description);
        break;

    case 3:
        _encounter = PondEncounter::STONE_2;
        screen.show_text(stone2_description);
        break;

    case 4:
        _encounter = PondEncounter::CREATURE;
        screen.show_text(creature_description);
        break;

    case 5:
    {
        _encounter = PondEncounter::RESOLVED;
        Result result = screen.roll(Attribute::DEX, 10, false, "Evade");

        if(result == Result::SUCCESS)
        {
            GameState::instance().change_sanity(-1);
            screen.show_text("You scramble back from the water's edge. You lose 1 Sanity.");
        }
        else
        {
            GameState::instance().take_damage(1);
            GameState::instance().change_sanity(-1);
            screen.show_text("Cold, slick fingers rake your leg before you break free. You "
                              "take 1 damage and lose 1 Sanity.");
        }

        break;
    }

    case 10:
        screen.goto_room(RoomId::ForestCrossing);
        break;

    case 11:
        screen.goto_room(RoomId::Cemetery);
        break;

    default:
        break;
    }
}

uint16_t OvergrownPond::save_state() const
{
    uint16_t bits = Room::save_state();
    bits |= (uint16_t(_encounter) & 0x7) << 1;
    return bits;
}

void OvergrownPond::load_state(uint16_t bits)
{
    Room::load_state(bits);
    _encounter = PondEncounter((bits >> 1) & 0x7);
}

}
