#include "wine_cellar.h"

#include "../ui/room_screen.h"
#include "../engine/game_state.h"
#include "../engine/rng.h"
#include "kitchen_cellar.h"

namespace sg::rooms
{

namespace
{
    constexpr bn::string_view long_description =
        "Rows of dusty wine bottles line the cold cellar shelves, undisturbed for decades.";
    constexpr bn::string_view short_description = "The wine cellar.";
}

void WineCellar::describe(bn::string<768>& out, bool force_long)
{
    out.append((! force_long && visited) ? short_description : long_description);
    visited = true;
}

void WineCellar::build_choices(bn::ivector<Choice>& out)
{
    out.push_back({ "Look", 0 });

    if(! _wine_tasted)
    {
        out.push_back({ "Taste wine", 1 });
    }

    out.push_back({ "N", 2 });
}

void WineCellar::on_choice(int action_id, RoomScreen& screen)
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
        _wine_tasted = true;
        Result result = screen.roll(Attribute::STR, 10, false, "Taste wine");

        if(result == Result::SUCCESS)
        {
            GameState::instance().take_damage(-1);
            screen.show_text("A fine vintage warms your chest. You recover 1 HP.");
        }
        else
        {
            int loss = rng().get_int(1, 4);
            GameState::instance().change_sanity(-loss);

            bn::string<160> text = "The wine tastes of copper and rot. You lose ";
            text.append(bn::to_string<8>(loss));
            text.append(" Sanity.");
            screen.show_text(text);
        }

        break;
    }

    case 2:
        static_cast<KitchenCellar&>(room(RoomId::KitchenCellar)).announce_rat_if_visited();
        screen.goto_room(RoomId::KitchenCellar);
        break;

    default:
        break;
    }
}

uint8_t WineCellar::save_state() const
{
    uint8_t bits = Room::save_state();
    if(_wine_tasted) bits |= 1 << 1;
    return bits;
}

void WineCellar::load_state(uint8_t bits)
{
    Room::load_state(bits);
    _wine_tasted = bits & (1 << 1);
}

}
