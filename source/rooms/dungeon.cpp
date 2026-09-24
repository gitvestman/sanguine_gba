#include "dungeon.h"

#include "../ui/room_screen.h"
#include "../engine/inventory.h"
#include "../engine/combat.h"

namespace sg::rooms
{

namespace
{
    constexpr bn::string_view long_description =
        "A cold stone hallway of locked cells. Something moves in the dark.";
    constexpr bn::string_view short_description = "The dungeon.";
    constexpr bn::string_view creatures_blurb =
        "Eyes catch the light, watching you hungrily from the shadows.";
}

void Dungeon::describe(bn::string<768>& out, bool force_long)
{
    bool first_visit = ! visited || force_long;
    out.append((! force_long && visited) ? short_description : long_description);
    visited = true;

    if(first_visit && ! _creatures_resolved)
    {
        out.append(" ");
        out.append(creatures_blurb);
    }
}

void Dungeon::build_choices(bn::ivector<Choice>& out)
{
    switch(_encounter)
    {

    case DungeonEncounter::INTRO:
        out.push_back({ "Look", 0 });
        out.push_back({ "W", 1 });

        if(! _dungeon_search)
        {
            out.push_back({ "Search", 2 });
        }

        break;

    case DungeonEncounter::SEARCH:
        out.push_back({ "Look", 0 });
        out.push_back({ "W", 1 });

        if(! _creatures_resolved)
        {
            out.push_back({ "Free creatures", 3 });
        }

        if(! _wooden_door_opened)
        {
            out.push_back({ "Open door", 4 });
        }

        break;

    case DungeonEncounter::FREE_CREATURES:
        out.push_back({ "Let them go", 5 });
        out.push_back({ "Step back", 6 });
        out.push_back({ "Chase them out", 7 });
        break;

    case DungeonEncounter::CHASE_OUT:
        out.push_back({ "Defend", 8 });
        break;
    }
}

void Dungeon::on_choice(int action_id, RoomScreen& screen)
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
        screen.goto_room(RoomId::CellarHall);
        break;

    case 2:
        _dungeon_search = true;
        _encounter = DungeonEncounter::SEARCH;
        Inventory::instance().add(ItemId::IRON_KEY);
        screen.show_text(
            "Most cells hold hideous, deformed creatures. You find a set of iron keys on "
            "the wall, and one cell with a solid wooden door instead of bars.\n\n"
            "You hear chains rattle behind it.");
        break;

    case 3:
        _encounter = DungeonEncounter::FREE_CREATURES;
        screen.show_text("Pitying them, you unlock the cells. Some creatures cower; others "
                          "hold their ground.");
        break;

    case 4:
        _wooden_door_opened = true;
        // Prisoner dialogue isn't ported yet (Phase 2+); stub the opening
        // beat and stop there rather than dropping into a broken talk state.
        screen.show_text("The iron key fits. Inside, a gaunt, chained prisoner squints at "
                          "you. (His conversation isn't part of this build yet.)");
        break;

    case 5:
        _encounter = DungeonEncounter::SEARCH;
        _creatures_resolved = true;
        screen.show_text("Some creatures flee past you in a panic. You leave the rest to "
                          "their fate.");
        break;

    case 6:
        _encounter = DungeonEncounter::SEARCH;
        _creatures_resolved = true;
        screen.show_text("You step back. One creature sniffs at you, then bolts away with "
                          "the others in pursuit.");
        break;

    case 7:
        _encounter = DungeonEncounter::CHASE_OUT;
        screen.show_text("You step into a cell to drive them out. Cornered, one of the "
                          "largest lashes out.");
        break;

    case 8:
        if(Combat::fight(screen, EnemyId::DungeonCreatures) == CombatResult::WON)
        {
            _encounter = DungeonEncounter::SEARCH;
            _creatures_resolved = true;
            screen.show_text("The rest of the creatures scatter, and the cells empty out.");
        }
        // On a flee, _encounter stays CHASE_OUT — "Defend" is offered again.
        break;

    default:
        break;
    }
}

uint8_t Dungeon::save_state() const
{
    uint8_t bits = Room::save_state();
    bits |= (uint8_t(_encounter) & 0x3) << 1;
    if(_dungeon_search) bits |= 1 << 3;
    if(_creatures_resolved) bits |= 1 << 4;
    if(_wooden_door_opened) bits |= 1 << 5;
    return bits;
}

void Dungeon::load_state(uint8_t bits)
{
    Room::load_state(bits);
    _encounter = DungeonEncounter((bits >> 1) & 0x3);
    _dungeon_search = bits & (1 << 3);
    _creatures_resolved = bits & (1 << 4);
    _wooden_door_opened = bits & (1 << 5);
}

}
