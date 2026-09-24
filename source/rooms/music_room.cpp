#include "music_room.h"

#include "../ui/room_screen.h"
#include "../engine/combat.h"
#include "../engine/rng.h"

namespace sg::rooms
{

namespace
{
    constexpr bn::string_view long_description =
        "A grand organ dominates the room, flanked by angelic statues. The keys look "
        "recently played.";
    constexpr bn::string_view short_description = "The music room.";
}

void MusicRoom::describe(bn::string<768>& out, bool force_long)
{
    out.append((! force_long && visited) ? short_description : long_description);
    visited = true;
}

void MusicRoom::build_choices(bn::ivector<Choice>& out)
{
    out.push_back({ "Look", 0 });

    if(! _organ_played)
    {
        out.push_back({ "Play organ", 1 });
    }

    out.push_back({ "S", 2 });
}

void MusicRoom::on_choice(int action_id, RoomScreen& screen)
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
        _organ_played = true;

        if(rng().get_int(1, 3) == 1)
        {
            screen.show_text("A discordant chord echoes through the hall. Nothing happens.");
        }
        else
        {
            screen.show_text("A shrill note sends a cloud of bats pouring from the pipes!");
            Combat::fight(screen, EnemyId::HungryBats);
        }

        break;

    case 2:
        screen.goto_room(RoomId::DinnerHall);
        break;

    default:
        break;
    }
}

uint16_t MusicRoom::save_state() const
{
    uint16_t bits = Room::save_state();
    if(_organ_played) bits |= 1 << 1;
    return bits;
}

void MusicRoom::load_state(uint16_t bits)
{
    Room::load_state(bits);
    _organ_played = bits & (1 << 1);
}

}
