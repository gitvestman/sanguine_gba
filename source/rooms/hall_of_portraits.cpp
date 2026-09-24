#include "hall_of_portraits.h"

#include "../ui/room_screen.h"
#include "../engine/game_state.h"
#include "../engine/rng.h"

namespace sg::rooms
{

namespace
{
    constexpr bn::string_view long_description =
        "Rows of painted eyes watch you from every wall. A faint whispering seems to come "
        "from the frames themselves.";
    constexpr bn::string_view short_description = "The hall of portraits.";
}

void HallOfPortraits::describe(bn::string<768>& out, bool force_long)
{
    out.append((! force_long && visited) ? short_description : long_description);
    visited = true;
}

void HallOfPortraits::build_choices(bn::ivector<Choice>& out)
{
    if(! _trap_sprung)
    {
        out.push_back({ "Endure the whispers", 1 });
        return;
    }

    out.push_back({ "Look", 0 });
    out.push_back({ "N", 2 });
    out.push_back({ "W", 3 });
}

void HallOfPortraits::on_choice(int action_id, RoomScreen& screen)
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
        Result result = screen.roll(Attribute::INT_, 10, false, "Endure the whispers");

        if(result == Result::SUCCESS)
        {
            screen.show_text("You steady your mind and shut out the whispering voices.");
        }
        else
        {
            int loss = rng().get_int(1, 4);
            GameState::instance().change_sanity(-loss);

            bn::string<160> text = "The whispers claw at your mind. You lose ";
            text.append(bn::to_string<8>(loss));
            text.append(" Sanity.");
            screen.show_text(text);
        }

        _trap_sprung = true;
        break;
    }

    case 2:
        screen.goto_room(RoomId::Library);
        break;

    case 3:
        screen.goto_room(RoomId::AudienceHall);
        break;

    default:
        break;
    }
}

uint16_t HallOfPortraits::save_state() const
{
    uint16_t bits = Room::save_state();
    if(_trap_sprung) bits |= 1 << 1;
    return bits;
}

void HallOfPortraits::load_state(uint16_t bits)
{
    Room::load_state(bits);
    _trap_sprung = bits & (1 << 1);
}

}
