#include "cellar_hall.h"

#include "bn_string.h"

#include "../ui/room_screen.h"
#include "../engine/game_state.h"
#include "../engine/rng.h"
#include "cliff_tombs.h"

namespace sg::rooms
{

namespace
{
    constexpr bn::string_view long_description =
        "A damp, crumbling hallway. Cold wind blows through cracks in the stone. Doors: one "
        "west, two east.";
    constexpr bn::string_view short_description = "The cellar hall.";
    constexpr bn::string_view trap_description = "Loose stones suddenly rumble down from the ceiling!";
}

void CellarHall::describe(bn::string<768>& out, bool force_long)
{
    out.append((! force_long && visited) ? short_description : long_description);
    visited = true;

    if(! _trap_sprung)
    {
        out.append("\n");
        out.append(trap_description);
    }
}

void CellarHall::build_choices(bn::ivector<Choice>& out)
{
    if(! _trap_sprung)
    {
        out.push_back({ "Dodge", 1 });
        return;
    }

    out.push_back({ "Look", 0 });

    if(north_secret_door_found)
    {
        out.push_back({ "N", 10 });
    }

    out.push_back({ "W", 11 });
    out.push_back({ "NE", 12 });
    out.push_back({ "E", 13 });
    out.push_back({ "S", 14 });

    if(! north_secret_door_found)
    {
        out.push_back({ "Search", 7 });
    }
}

void CellarHall::on_choice(int action_id, RoomScreen& screen)
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
        Result result = screen.roll(Attribute::DEX, 10, false, "Dodge");

        if(result == Result::SUCCESS)
        {
            screen.show_text("You dodge the falling rubble.");
        }
        else
        {
            int damage = rng().get_int(1, 7);
            GameState::instance().take_damage(damage);

            bn::string<160> text = "A rock strikes your head.\n\nYou take ";
            text.append(bn::to_string<8>(damage));
            text.append(" damage.");
            screen.show_text(text);
        }

        _trap_sprung = true;
        break;
    }

    case 10:
        screen.goto_room(RoomId::StoneCorridor);
        break;

    case 11:
        if(item_unlocks_path(ItemId::IRON_KEY))
        {
            screen.goto_room(RoomId::Ossuary);
        }
        else
        {
            screen.show_text("The door is locked. You need a key.");
        }
        break;

    case 12:
        if(item_unlocks_path(ItemId::IRON_KEY))
        {
            screen.goto_room(RoomId::TortureChamber);
        }
        else
        {
            screen.show_text("The door is locked. You need a key.");
        }
        break;

    case 13:
        screen.goto_room(RoomId::Dungeon);
        break;

    case 14:
        static_cast<CliffTombs&>(room(RoomId::CliffTombs)).set_found_hidden();
        screen.goto_room(RoomId::CliffTombs);
        break;

    case 7:
    {
        Result result = screen.roll(Attribute::INT_, 10, true, "Search");

        if(result == Result::SUCCESS)
        {
            north_secret_door_found = true;
            screen.show_text("The north wall looks different. You press it and a hidden "
                              "passage swings open.");
        }
        else if(result == Result::FAIL || result == Result::CRITICAL_FAIL)
        {
            screen.show_text("You find nothing out of the ordinary.");
        }

        break;
    }

    default:
        break;
    }
}

uint16_t CellarHall::save_state() const
{
    uint16_t bits = Room::save_state();
    if(_trap_sprung) bits |= 1 << 1;
    if(north_secret_door_found) bits |= 1 << 2;
    return bits;
}

void CellarHall::load_state(uint16_t bits)
{
    Room::load_state(bits);
    _trap_sprung = bits & (1 << 1);
    north_secret_door_found = bits & (1 << 2);
}

CellarHall& cellar_hall()
{
    return static_cast<CellarHall&>(room(RoomId::CellarHall));
}

}
