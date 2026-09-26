#include "forest_crossing.h"

#include "../ui/room_screen.h"
#include "../engine/combat.h"

namespace sg::rooms
{

namespace
{
    constexpr bn::string_view long_description =
        "The road splits here: a narrow path west through the trees, and a wide road north "
        "toward the castle.";
    constexpr bn::string_view short_description = "The forest crossing.";
    constexpr bn::string_view path_hint = "\n\nTo the east, a hidden path leads through the "
                                          "bushes.";
    constexpr bn::string_view raven_blurb = "\n\nA raven with unnatural red eyes circles "
                                            "overhead.";
}

void ForestCrossing::describe(bn::string<768>& out, bool force_long)
{
    out.append((! force_long && visited) ? short_description : long_description);
    visited = true;

    if(_found_path)
    {
        out.append(path_hint);
    }

    if(! enemy(EnemyId::Raven).is_dead())
    {
        out.append(raven_blurb);
    }
}

void ForestCrossing::on_enter(RoomScreen& screen)
{
    if(! enemy(EnemyId::Raven).is_dead())
    {
        screen.show_enemy(EnemyId::Raven);
    }
}

void ForestCrossing::build_choices(bn::ivector<Choice>& out)
{
    out.push_back({ "Look", 0 });
    out.push_back({ "N", 1 });
    out.push_back({ "W", 2 });
    out.push_back({ "S", 3 });

    if(_found_path)
    {
        out.push_back({ "E", 4 });
    }
    else if(! _search_attempted)
    {
        out.push_back({ "Search bushes", 5 });
    }

    if(! enemy(EnemyId::Raven).is_dead())
    {
        out.push_back({ "Attack raven", 6 });
    }
}

void ForestCrossing::on_choice(int action_id, RoomScreen& screen)
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
        screen.goto_room(RoomId::MainGate);
        break;

    case 2:
        screen.goto_room(RoomId::GroundskeeperCottage);
        break;

    case 3:
        screen.goto_room(RoomId::OutsideGates);
        break;

    case 4:
        screen.goto_room(RoomId::OvergrownPond);
        break;

    case 5:
    {
        Result result = screen.roll(Attribute::INT_, 10, false, "Search bushes");

        if(result == Result::SUCCESS)
        {
            _found_path = true;
            screen.show_text("Parting the bushes, you find a hidden path heading east.");
        }
        else
        {
            _search_attempted = true;
            screen.show_text("You find nothing in the bushes.");
        }

        break;
    }

    case 6:
        Combat::fight(screen, EnemyId::Raven);

        if(! enemy(EnemyId::Raven).is_dead())
        {
            screen.show_enemy(EnemyId::Raven);
        }

        break;

    default:
        break;
    }
}

uint16_t ForestCrossing::save_state() const
{
    uint16_t bits = Room::save_state();
    if(_found_path) bits |= 1 << 1;
    if(_search_attempted) bits |= 1 << 2;
    return bits;
}

void ForestCrossing::load_state(uint16_t bits)
{
    Room::load_state(bits);
    _found_path = bits & (1 << 1);
    _search_attempted = bits & (1 << 2);
}

}
