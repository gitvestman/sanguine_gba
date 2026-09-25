#include "cliff_tombs.h"

#include "../ui/room_screen.h"
#include "../engine/game_state.h"
#include "../engine/rng.h"

namespace sg::rooms
{

namespace
{
    constexpr bn::string_view long_description =
        "A row of tombs built into the cliff face, each carved with Christian iconography "
        "around an iron gate.";
    constexpr bn::string_view short_description = "The cliff tombs.";
    constexpr bn::string_view found_hidden_hint = "\n\nOne gate hangs loose - a hidden way "
                                                  "through, back into the castle's cellar.";
}

void CliffTombs::describe(bn::string<768>& out, bool force_long)
{
    out.append((! force_long && visited) ? short_description : long_description);
    visited = true;

    if(_found_hidden)
    {
        out.append(found_hidden_hint);
    }
}

void CliffTombs::build_choices(bn::ivector<Choice>& out)
{
    if(_high_on_cliff)
    {
        out.push_back({ "Continue climbing", 1 });
        out.push_back({ "Climb down", 2 });
        return;
    }

    out.push_back({ "Look", 0 });
    out.push_back({ "S", 3 });

    if(_found_hidden)
    {
        out.push_back({ "N", 4 });
    }
    else if(! _search_attempted)
    {
        out.push_back({ "Search", 5 });
    }

    if(! _climb_resolved)
    {
        out.push_back({ "Climb cliff", 6 });
    }
}

void CliffTombs::on_choice(int action_id, RoomScreen& screen)
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
    {
        Result result = screen.roll(Attribute::DEX, 10, false, "Continue climbing");

        if(result == Result::SUCCESS)
        {
            _high_on_cliff = false;
            _climb_resolved = true;
            screen.show_text("You haul yourself up to a narrow window, high on the cliff "
                              "face, and climb through.");
            screen.goto_room(RoomId::Bathroom);
        }
        else
        {
            GameState& gs = GameState::instance();
            gs.take_damage(gs.stat(Attribute::HP));
            screen.show_text("Your grip slips. You fall.");
        }

        break;
    }

    case 2:
        _high_on_cliff = false;
        _climb_resolved = true;
        screen.show_text("You climb back down, thinking better of it.");
        break;

    case 3:
        screen.goto_room(RoomId::Cemetery);
        break;

    case 4:
        screen.goto_room(RoomId::CellarHall);
        break;

    case 5:
    {
        _search_attempted = true;
        Result result = screen.roll(Attribute::INT_, 10, true, "Search");

        if(result == Result::SUCCESS)
        {
            _found_hidden = true;
            screen.show_text("Behind a loose gate, a passage leads down into darkness.");
        }
        else if(result == Result::FAIL || result == Result::CRITICAL_FAIL)
        {
            screen.show_text("Just old bones and cold stone.");
        }

        break;
    }

    case 6:
    {
        Result result = screen.roll(Attribute::STR, 10, false, "Climb cliff");

        if(result == Result::SUCCESS)
        {
            _high_on_cliff = true;
            screen.show_text("You find handholds in the cliff face and start climbing.");
        }
        else
        {
            _climb_resolved = true;
            screen.show_text("You can't find purchase. You'll never manage to climb this.");
        }

        break;
    }

    default:
        break;
    }
}

uint16_t CliffTombs::save_state() const
{
    uint16_t bits = Room::save_state();
    if(_found_hidden) bits |= 1 << 1;
    if(_search_attempted) bits |= 1 << 2;
    if(_climb_resolved) bits |= 1 << 3;
    if(_high_on_cliff) bits |= 1 << 4;
    return bits;
}

void CliffTombs::load_state(uint16_t bits)
{
    Room::load_state(bits);
    _found_hidden = bits & (1 << 1);
    _search_attempted = bits & (1 << 2);
    _climb_resolved = bits & (1 << 3);
    _high_on_cliff = bits & (1 << 4);
}

}
