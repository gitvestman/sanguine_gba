#include "chapel.h"

#include "../ui/room_screen.h"
#include "../engine/inventory.h"
#include "../engine/game_state.h"
#include "hall_of_armors.h"

#include "bn_sprite_items_npc_ghostly_bride.h"

namespace sg::rooms
{

namespace
{
    constexpr bn::string_view long_description =
        "A pristine chapel, untouched by the castle's decay. Moonlight filters through a "
        "stained-glass knight.";
    constexpr bn::string_view short_description = "The chapel.";
    constexpr bn::string_view gate_hint = "\n\nBehind the altar, a locked iron gate bears a "
                                          "strange sliding-tile pattern.";
}

void Chapel::describe(bn::string<768>& out, bool force_long)
{
    out.append((! force_long && visited) ? short_description : long_description);
    visited = true;

    if(_gate_found && ! _gate_unlocked)
    {
        out.append(gate_hint);
    }
}

void Chapel::build_choices(bn::ivector<Choice>& out)
{
    out.push_back({ "Look", 0 });
    out.push_back({ "S", 1 });

    if(_gate_unlocked || _entered_from_ossuary)
    {
        out.push_back({ "Down", 2 });
    }

    if(! _searched)
    {
        out.push_back({ "Search", 3 });
    }

    if(_gate_found && ! _gate_unlocked)
    {
        out.push_back({ "Solve puzzle", 4 });
    }

    if(_gate_found && ! _bride.departed())
    {
        out.push_back({ "Focus on the uncanniness", 5 });
    }
}

void Chapel::on_choice(int action_id, RoomScreen& screen)
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
        static_cast<HallOfArmors&>(room(RoomId::HallOfArmors)).enter_from(1);
        screen.goto_room(RoomId::HallOfArmors);
        break;

    case 2:
        screen.goto_room(RoomId::Ossuary);
        break;

    case 3:
        _searched = true;
        _gate_found = true;
        screen.show_text("Old prayer books and cold candle stubs. Behind the altar, you "
                          "notice a locked iron gate.");
        break;

    case 4:
        if(_puzzle.play(screen))
        {
            _gate_unlocked = true;
            Inventory::instance().add(ItemId::RESTORATIVE_MIXTURE);
            screen.show_text("The tiles click into place and the gate swings open. In a "
                              "small alcove, you find a restorative mixture.");
        }
        else
        {
            screen.show_text("You step back from the puzzle, unsolved for now.");
        }

        break;

    case 5:
    {
        Result result = screen.roll(Attribute::INT_, 10, true, "Focus");

        if(result == Result::SUCCESS)
        {
            screen.show_text("Shadows part and whispers coalesce. A woman kneels before the "
                              "altar in prayer, translucent and unaware of you.");
            _bride.talk(screen, "She senses your presence and turns.", &bn::sprite_items::npc_ghostly_bride);
        }
        else if(result == Result::FAIL || result == Result::CRITICAL_FAIL)
        {
            GameState::instance().change_sanity(-1);
            screen.show_text("A ghostly shape resolves before the altar. Your mind reels. "
                              "You lose 1 Sanity.");
            _bride.talk(screen, "She senses your presence and turns.", &bn::sprite_items::npc_ghostly_bride);
        }

        break;
    }

    default:
        break;
    }
}

uint16_t Chapel::save_state() const
{
    uint16_t bits = Room::save_state();
    if(_searched) bits |= 1 << 1;
    if(_gate_found) bits |= 1 << 2;
    if(_gate_unlocked) bits |= 1 << 3;
    if(_bride.departed()) bits |= 1 << 4;
    if(_entered_from_ossuary) bits |= 1 << 5;
    return bits;
}

void Chapel::load_state(uint16_t bits)
{
    Room::load_state(bits);
    _searched = bits & (1 << 1);
    _gate_found = bits & (1 << 2);
    _gate_unlocked = bits & (1 << 3);
    _bride.set_departed(bits & (1 << 4));
    _entered_from_ossuary = bits & (1 << 5);
}

}
