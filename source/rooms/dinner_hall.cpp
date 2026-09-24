#include "dinner_hall.h"

#include "../ui/room_screen.h"
#include "../engine/game_state.h"

namespace sg::rooms
{

namespace
{
    constexpr bn::string_view long_description =
        "The ruined dining hall reeks of old grease and violence. A crushing dread grips "
        "your soul.";
    constexpr bn::string_view short_description = "The dining hall.";
}

void DinnerHall::describe(bn::string<768>& out, bool force_long)
{
    out.append((! force_long && visited) ? short_description : long_description);
    visited = true;
}

void DinnerHall::build_choices(bn::ivector<Choice>& out)
{
    if(! _trap_sprung)
    {
        out.push_back({ "Resist the dread", 1 });
        return;
    }

    out.push_back({ "Look", 0 });
    out.push_back({ "N", 2 });
    out.push_back({ "W", 3 });
    out.push_back({ "S", 4 });

    if(! _searched)
    {
        out.push_back({ "Search", 5 });
    }
}

void DinnerHall::on_choice(int action_id, RoomScreen& screen)
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
        Result result = screen.roll(Attribute::INT_, 10, false, "Resist the dread");

        if(result == Result::SUCCESS)
        {
            screen.show_text("You steady your mind and shake off the worst of it.");
        }
        else
        {
            GameState::instance().change_sanity(-1);
            screen.show_text("The dread claws at your mind. You lose 1 Sanity.");
        }

        _trap_sprung = true;
        break;
    }

    case 2:
        screen.goto_room(RoomId::MusicRoom);
        break;

    case 3:
        screen.goto_room(RoomId::Kitchen);
        break;

    case 4:
        screen.goto_room(RoomId::GreatHall);
        break;

    case 5:
        _searched = true;
        screen.show_text("Broken chairs and mildewed linen. Nothing worth taking.");
        break;

    default:
        break;
    }
}

uint16_t DinnerHall::save_state() const
{
    uint16_t bits = Room::save_state();
    if(_trap_sprung) bits |= 1 << 1;
    if(_searched) bits |= 1 << 2;
    return bits;
}

void DinnerHall::load_state(uint16_t bits)
{
    Room::load_state(bits);
    _trap_sprung = bits & (1 << 1);
    _searched = bits & (1 << 2);
}

}
