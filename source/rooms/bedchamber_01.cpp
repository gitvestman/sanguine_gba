#include "bedchamber_01.h"

#include "../ui/room_screen.h"
#include "../engine/combat.h"
#include "../engine/game_state.h"

namespace sg::rooms
{

namespace
{
    constexpr bn::string_view long_description =
        "A once-grand bedchamber, its curtains rotted to lace. Something shifts under the "
        "sheets.";
    constexpr bn::string_view short_description = "The bedchamber.";
    constexpr bn::string_view passage_hint = "\n\nA gap behind the wardrobe leads south.";
}

void BedChamber01::describe(bn::string<768>& out, bool force_long)
{
    bool resolved = enemy(EnemyId::Zombie).is_dead() || _peaceful;

    if(resolved)
    {
        out.append((! force_long && visited) ? short_description : long_description);
        visited = true;

        if(_passage_found)
        {
            out.append(passage_hint);
        }

        return;
    }

    switch(_encounter)
    {

    case ZombieEncounter::ANNOUNCED:
        out.append("A gaunt shape rises from the bed. \"Who dares wake me?\"");
        break;

    default:
        out.append(long_description);
        break;
    }

    visited = true;
}

void BedChamber01::build_choices(bn::ivector<Choice>& out)
{
    bool resolved = enemy(EnemyId::Zombie).is_dead() || _peaceful;

    if(resolved)
    {
        out.push_back({ "Look", 0 });
        out.push_back({ "W", 1 });

        if(_passage_found)
        {
            out.push_back({ "S", 2 });
        }
        else
        {
            out.push_back({ "Search", 3 });
        }

        return;
    }

    switch(_encounter)
    {

    case ZombieEncounter::START:
        out.push_back({ "Announce yourself", 4 });
        out.push_back({ "W", 1 });
        break;

    case ZombieEncounter::ANNOUNCED:
    case ZombieEncounter::HORROR_SUCCESS:
    case ZombieEncounter::HORROR_FAIL:
        if(_encounter == ZombieEncounter::ANNOUNCED)
        {
            out.push_back({ "Endure the horror", 5 });
            out.push_back({ "Flee", 6 });
        }
        else
        {
            out.push_back({ "Attack", 7 });
            out.push_back({ "\"Begone, undead!\"", 7 });
            out.push_back({ "Stay quiet", 8 });
        }

        break;

    case ZombieEncounter::STILL:
        out.push_back({ "Attack", 7 });
        out.push_back({ "\"Begone, undead!\"", 7 });
        out.push_back({ "Let it embrace you", 9 });
        break;

    case ZombieEncounter::EMBRACED:
        out.push_back({ "Endure the stench", 10 });
        break;
    }
}

void BedChamber01::on_choice(int action_id, RoomScreen& screen)
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
        screen.goto_room(RoomId::FancyCorridor01);
        break;

    case 2:
        screen.goto_room(RoomId::Library);
        break;

    case 3:
    {
        Result result = screen.roll(Attribute::INT_, 10, false, "Search");

        if(result == Result::SUCCESS)
        {
            _passage_found = true;
            screen.show_text("Behind the wardrobe, you find a gap leading south.");
        }
        else
        {
            screen.show_text("You find nothing more.");
        }

        break;
    }

    case 4:
        _encounter = ZombieEncounter::ANNOUNCED;
        screen.show_text("You clear your throat. A gaunt shape rises from the bed.");
        break;

    case 5:
    {
        Result result = screen.roll(Attribute::INT_, 10, false, "Endure the horror");

        if(result == Result::SUCCESS)
        {
            _encounter = ZombieEncounter::HORROR_SUCCESS;
            screen.show_text("You force yourself to look upon it without flinching.");
        }
        else
        {
            GameState::instance().change_sanity(-1);
            _encounter = ZombieEncounter::HORROR_FAIL;
            screen.show_text("Its hollow, rotting face is too much. You lose 1 Sanity.");
        }

        break;
    }

    case 6:
        _encounter = ZombieEncounter::START;
        screen.show_text("You back out of the room, leaving it be for now.");
        break;

    case 7:
        Combat::fight(screen, EnemyId::Zombie);
        break;

    case 8:
        _encounter = ZombieEncounter::STILL;
        screen.show_text("You stand perfectly still. It seems to lose interest, for now.");
        break;

    case 9:
        _encounter = ZombieEncounter::EMBRACED;
        screen.show_text("Cold, rotting arms wrap around you in a mockery of comfort.");
        break;

    case 10:
    {
        Result result = screen.roll(Attribute::STR, 10, false, "Endure the stench");

        if(result == Result::SUCCESS)
        {
            _peaceful = true;
            GameState::instance().change_luck(1);
            screen.show_text("You bear the stench and stillness until it settles, "
                              "seemingly at peace. You feel luckier for it.");
        }
        else
        {
            GameState::instance().take_damage(1);
            screen.show_text("Your stomach turns and you flinch. It reacts violently!");
            Combat::fight(screen, EnemyId::Zombie);
        }

        break;
    }

    default:
        break;
    }
}

uint16_t BedChamber01::save_state() const
{
    uint16_t bits = Room::save_state();
    if(_passage_found) bits |= 1 << 1;
    if(_peaceful) bits |= 1 << 2;
    bits |= (uint16_t(_encounter) & 0x7) << 3;
    return bits;
}

void BedChamber01::load_state(uint16_t bits)
{
    Room::load_state(bits);
    _passage_found = bits & (1 << 1);
    _peaceful = bits & (1 << 2);
    _encounter = ZombieEncounter((bits >> 3) & 0x7);
}

}
