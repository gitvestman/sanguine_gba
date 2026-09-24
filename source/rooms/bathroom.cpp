#include "bathroom.h"

#include "../ui/room_screen.h"
#include "../engine/game_state.h"
#include "../engine/inventory.h"
#include "../engine/rng.h"

namespace sg::rooms
{

namespace
{
    constexpr bn::string_view long_description =
        "A cracked marble bathtub sits half-filled with black sludge. Something glints "
        "beneath the surface.";
    constexpr bn::string_view short_description = "The bathroom.";
}

void Bathroom::describe(bn::string<768>& out, bool force_long)
{
    out.append((! force_long && visited) ? short_description : long_description);
    visited = true;

    if(! _sludge_searched)
    {
        out.append(" Whatever it is, it's submerged deep in the muck.");
    }
}

void Bathroom::build_choices(bn::ivector<Choice>& out)
{
    out.push_back({ "Look", 0 });
    out.push_back({ "E", 1 });
    out.push_back({ "Look outside", 2 });

    if(! _sludge_searched)
    {
        out.push_back({ "Search sludge", 3 });
    }
}

void Bathroom::on_choice(int action_id, RoomScreen& screen)
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
        screen.show_text("Through the cracked window, moonlit cliffs drop away into the "
                          "dark below.");
        break;

    case 3:
    {
        _sludge_searched = true;
        Result result = screen.roll(Attribute::INT_, 10, true, "Search sludge");

        if(result == Result::SUCCESS)
        {
            ItemId reward = rng().get_int(0, 2) == 0 ? ItemId::RESTORATIVE_MIXTURE
                                                      : ItemId::HEALING_TINCTURE;
            Inventory::instance().add(reward);
            screen.show_text("Gagging, you fish out a small, unbroken bottle.");
        }
        else if(result == Result::FAIL || result == Result::CRITICAL_FAIL)
        {
            int loss = rng().get_int(1, 4);
            GameState::instance().change_sanity(-loss);

            bn::string<160> text = "Your hand closes on something soft and yielding. You "
                                    "lose ";
            text.append(bn::to_string<8>(loss));
            text.append(" Sanity.");
            screen.show_text(text);
        }

        break;
    }

    default:
        break;
    }
}

uint16_t Bathroom::save_state() const
{
    uint16_t bits = Room::save_state();
    if(_sludge_searched) bits |= 1 << 1;
    return bits;
}

void Bathroom::load_state(uint16_t bits)
{
    Room::load_state(bits);
    _sludge_searched = bits & (1 << 1);
}

}
