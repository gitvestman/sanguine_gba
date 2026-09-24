#include "treasury.h"

#include "../ui/room_screen.h"
#include "../engine/inventory.h"
#include "../engine/rng.h"

namespace sg::rooms
{

namespace
{
    constexpr bn::string_view long_description =
        "A cramped strongroom, a heavy iron-bound chest sitting untouched in the corner.";
    constexpr bn::string_view short_description = "The treasury.";

    void grant_random_reward(RoomScreen& screen)
    {
        ItemId reward = rng().get_int(0, 2) == 0 ? ItemId::RESTORATIVE_MIXTURE
                                                  : ItemId::HEALING_TINCTURE;
        Inventory::instance().add(reward);
        screen.show_text("The chest creaks open. Inside, wrapped in oilcloth, is a small "
                          "bottle.");
    }
}

void Treasury::describe(bn::string<768>& out, bool force_long)
{
    out.append((! force_long && visited) ? short_description : long_description);
    visited = true;
}

void Treasury::build_choices(bn::ivector<Choice>& out)
{
    out.push_back({ "Look", 0 });
    out.push_back({ "S", 1 });

    if(! _chest_open)
    {
        if(! _searched)
        {
            out.push_back({ "Search", 2 });
        }

        if(Inventory::instance().has(ItemId::IRON_KEY) || Inventory::instance().has(ItemId::SKULL_KEY))
        {
            out.push_back({ "Use key", 3 });
        }
        else if(Inventory::instance().has(ItemId::LOCKPICKS) ||
                Inventory::instance().has(ItemId::SILVER_DAGGER))
        {
            out.push_back({ "Pick lock", 4 });
        }
    }
}

void Treasury::on_choice(int action_id, RoomScreen& screen)
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
        screen.goto_room(RoomId::TrophyRoom);
        break;

    case 2:
        _searched = true;
        screen.show_text("A heavy chest, its lock rusted shut.");
        break;

    case 3:
        _chest_open = true;
        grant_random_reward(screen);
        break;

    case 4:
    {
        Result result = screen.roll(Attribute::DEX, 12, true, "Pick lock");
        _chest_open = true;

        if(result == Result::SUCCESS)
        {
            grant_random_reward(screen);
        }
        else if(result == Result::FAIL || result == Result::CRITICAL_FAIL)
        {
            screen.show_text("The lockpick snaps off in the mechanism. The chest stays "
                              "shut.");
        }
        else
        {
            _chest_open = false;
        }

        break;
    }

    default:
        break;
    }
}

uint16_t Treasury::save_state() const
{
    uint16_t bits = Room::save_state();
    if(_searched) bits |= 1 << 1;
    if(_chest_open) bits |= 1 << 2;
    return bits;
}

void Treasury::load_state(uint16_t bits)
{
    Room::load_state(bits);
    _searched = bits & (1 << 1);
    _chest_open = bits & (1 << 2);
}

}
