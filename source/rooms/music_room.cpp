#include "music_room.h"

#include "../ui/room_screen.h"
#include "../engine/combat.h"
#include "../engine/game_state.h"
#include "../engine/inventory.h"
#include "../engine/item_info.h"
#include "../engine/rng.h"

namespace sg::rooms
{

namespace
{
    constexpr bn::string_view long_description =
        "A grand organ dominates the room, flanked by angelic statues. The keys look "
        "recently played.";
    constexpr bn::string_view short_description = "The music room.";

    // Mirrors the source's RANDOM_ITEM_POOL, trimmed to this build's own item
    // roster (no Rosary/Blunt Dagger/Medkit/Faerie Cocktail here) — prefers
    // an item the player doesn't have yet, falling back to one of the two
    // potions (matching the source's RANDOM_CONSUMABLE_POOL fallback) once
    // every pool item is already owned.
    constexpr ItemId random_item_pool[] = {
        ItemId::FLINTLOCK, ItemId::SILVER_DAGGER, ItemId::RESTORATIVE_MIXTURE,
        ItemId::HEALING_TINCTURE, ItemId::MEZUZAH_AMULET
    };

    void grant_random_item(RoomScreen& screen)
    {
        Inventory& inventory = Inventory::instance();

        ItemId unowned[sizeof(random_item_pool) / sizeof(random_item_pool[0])];
        int unowned_count = 0;

        for(ItemId item : random_item_pool)
        {
            if(! inventory.has(item))
            {
                unowned[unowned_count] = item;
                ++unowned_count;
            }
        }

        ItemId reward = unowned_count > 0
                            ? unowned[rng().get_int(0, unowned_count - 1)]
                            : (rng().get_int(0, 1) == 0 ? ItemId::RESTORATIVE_MIXTURE
                                                         : ItemId::HEALING_TINCTURE);

        inventory.add(reward);

        // Mirrors GameState.add_item's own MEZUZAH_AMULET special case
        // (gamestate_singleton.gd) — the amulet's +3 INT is a permanent
        // pickup effect, same shape as Stairway2ndFloor's Bastet Figurine.
        if(reward == ItemId::MEZUZAH_AMULET)
        {
            GameState::instance().change_int(3);
        }

        bn::string<128> text;
        text.append("You find a ");
        text.append(item_info(reward).name);
        text.append("!");
        screen.show_text(text);
    }
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

    if(Inventory::instance().has(ItemId::SHEET_MUSIC) && ! _sheet_music_played)
    {
        out.push_back({ "Play sheet music", 3 });
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

    case 3:
        _sheet_music_played = true;
        screen.show_text("You unfold the sheet music and play it on the organ. The melody "
                          "lingers, and something in the room seems to shift.");
        grant_random_item(screen);
        break;

    default:
        break;
    }
}

void MusicRoom::build_ambient(bn::ivector<AmbientDecor>& out) const
{
    out.push_back({ AmbientKind::CANDLE, -35, -22 });
    out.push_back({ AmbientKind::CANDLE, 35, -22 });
}

uint16_t MusicRoom::save_state() const
{
    uint16_t bits = Room::save_state();
    if(_organ_played) bits |= 1 << 1;
    if(_sheet_music_played) bits |= 1 << 2;
    return bits;
}

void MusicRoom::load_state(uint16_t bits)
{
    Room::load_state(bits);
    _organ_played = bits & (1 << 1);
    _sheet_music_played = bits & (1 << 2);
}

}
