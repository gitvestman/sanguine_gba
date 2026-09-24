#include "armory_room.h"

#include "../ui/room_screen.h"
#include "../engine/game_state.h"
#include "../engine/rng.h"

namespace sg::rooms
{

namespace
{
    constexpr bn::string_view long_description =
        "An old armory, its weapons and armor tarnished with age.";
    constexpr bn::string_view short_description = "The armory.";
    constexpr bn::string_view trap_description = "A click from the floor -- a suit of armor grabs you!";
}

void ArmoryRoom::describe(bn::string<768>& out, bool force_long)
{
    out.append((! force_long && visited) ? short_description : long_description);
    visited = true;

    if(! _trap_sprung)
    {
        out.append("\n");
        out.append(trap_description);
    }
}

void ArmoryRoom::build_choices(bn::ivector<Choice>& out)
{
    if(! _trap_sprung)
    {
        out.push_back({ "Fight armor", 1 });
        return;
    }

    out.push_back({ "Look", 0 });
    out.push_back({ "Search", 2 });
    out.push_back({ "SE", 3 });
}

void ArmoryRoom::on_choice(int action_id, RoomScreen& screen)
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
        Result result = screen.roll(Attribute::STR, 10, false, "Fight off armor");

        if(result == Result::SUCCESS)
        {
            screen.show_text("You manage to break free from the mechanical grip before it crushes you.");
        }
        else
        {
            int damage = rng().get_int(1, 7);
            GameState::instance().take_damage(damage);

            bn::string<160> text = "The armor mangles your body before you can finally break "
                                    "its grip.\n\nYou take ";
            text.append(bn::to_string<8>(damage));
            text.append(" points of damage.");
            screen.show_text(text);
        }

        _trap_sprung = true;
        break;
    }

    case 2:
        screen.show_text(
            "You are careful to keep your distance from the other armors and potential traps "
            "as you inspect the items in the armory more closely. As you initially thought, "
            "they are in poor condition. However, the older of them bears the same heraldry. "
            "It is a bull's head with a star above, supported by two princes. Above them is "
            "an eagle between a sun and a crescent, and at the base, two lions supporting an "
            "upright sword, standing on seven hills.\n\n"
            "You recognize the bull's head as the arms of Moldavia and the eagle as a "
            "Wallachian symbol, but you cannot place the rest.");
        break;

    case 3:
        screen.goto_room(RoomId::StoneCorridor);
        break;

    default:
        break;
    }
}

uint8_t ArmoryRoom::save_state() const
{
    uint8_t bits = Room::save_state();
    if(_trap_sprung) bits |= 1 << 1;
    return bits;
}

void ArmoryRoom::load_state(uint8_t bits)
{
    Room::load_state(bits);
    _trap_sprung = bits & (1 << 1);
}

}
