#include "kitchen.h"

#include "../ui/room_screen.h"
#include "../engine/game_state.h"
#include "../engine/rng.h"

namespace sg::rooms
{

namespace
{
    constexpr bn::string_view long_description =
        "A cold kitchen, pots hanging untouched for years. A haunch of meat still hangs "
        "within reach.";
    constexpr bn::string_view short_description = "The kitchen.";
}

void Kitchen::describe(bn::string<768>& out, bool force_long)
{
    out.append((! force_long && visited) ? short_description : long_description);
    visited = true;
}

void Kitchen::build_choices(bn::ivector<Choice>& out)
{
    out.push_back({ "Look", 0 });

    if(! _meat_taken)
    {
        out.push_back({ "Reach for meat", 1 });
    }

    out.push_back({ "N", 2 });
    out.push_back({ "W", 3 });
    out.push_back({ "E", 4 });

    if(! _south_wall_inspected)
    {
        out.push_back({ "Inspect south wall", 5 });
    }
    else
    {
        out.push_back({ "S", 6 });
    }
}

void Kitchen::on_choice(int action_id, RoomScreen& screen)
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
        _meat_taken = true;
        Result result = screen.roll(Attribute::DEX, 10, false, "Reach for meat");

        if(result == Result::SUCCESS)
        {
            GameState::instance().take_damage(-2);
            screen.show_text("You snag the meat without setting off the rusty hooks. You "
                              "recover 2 HP.");
        }
        else
        {
            int damage = rng().get_int(1, 7);
            GameState::instance().take_damage(damage);

            bn::string<160> text = "A hook swings loose and gashes your arm. You take ";
            text.append(bn::to_string<8>(damage));
            text.append(" damage.");
            screen.show_text(text);
        }

        break;
    }

    case 2:
        screen.goto_room(RoomId::Storage);
        break;

    case 3:
        screen.goto_room(RoomId::KitchenCellar);
        break;

    case 4:
        screen.goto_room(RoomId::DinnerHall);
        break;

    case 5:
        _south_wall_inspected = true;
        screen.show_text("Behind a stack of crates, a door leads back to the courtyard.");
        break;

    case 6:
        screen.goto_room(RoomId::Courtyard);
        break;

    default:
        break;
    }
}

uint8_t Kitchen::save_state() const
{
    uint8_t bits = Room::save_state();
    if(_meat_taken) bits |= 1 << 1;
    if(_south_wall_inspected) bits |= 1 << 2;
    return bits;
}

void Kitchen::load_state(uint8_t bits)
{
    Room::load_state(bits);
    _meat_taken = bits & (1 << 1);
    _south_wall_inspected = bits & (1 << 2);
}

}
