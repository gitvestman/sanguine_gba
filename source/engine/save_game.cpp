#include "save_game.h"

#include <cstdint>

#include "bn_sram.h"

#include "game_state.h"
#include "inventory.h"

namespace sg
{

namespace
{
    // Bumping this invalidates any existing save (rather than misreading it
    // as this version's layout) — mirrors SAVE_VERSION in
    // gamestate_singleton.gd.
    constexpr uint32_t magic = 0x53'41'56'01; // "SAV" + version 1

    struct SaveData
    {
        uint32_t magic;
        uint8_t current_room;
        uint8_t hp;
        uint8_t sanity;
        uint8_t luck;
        // Widened from uint8_t alongside room_state below, once Phase 4d's
        // new items (Journal, Strange Figurine, Doll, Bastet Figurine)
        // pushed ItemId::COUNT past 8.
        uint16_t inventory_mask;

        // Widened from uint8_t once Cemetery's Myling encounter (Phase 4c)
        // needed more than 8 bits of state for one room — a GBA SRAM chip
        // has many KB to spare, so this is just a self-imposed sanity
        // bound below, not a hardware limit.
        uint16_t room_state[int(RoomId::COUNT)];
    };

    static_assert(sizeof(SaveData) <= 256, "Save data has grown past a sane SRAM footprint");
}

bool SaveGame::save_exists()
{
    SaveData data{};
    bn::sram::read(data);
    return data.magic == magic;
}

RoomId SaveGame::load_or_new_game()
{
    SaveData data{};
    bn::sram::read(data);

    if(data.magic != magic)
    {
        return RoomId::ForestCrossing;
    }

    GameState& game_state = GameState::instance();
    game_state.load_stat(Attribute::HP, data.hp);
    game_state.load_stat(Attribute::SANITY, data.sanity);
    game_state.load_stat(Attribute::LUCK, data.luck);

    Inventory& inventory = Inventory::instance();

    for(int i = 0; i < int(ItemId::COUNT); ++i)
    {
        if(data.inventory_mask & (1 << i))
        {
            inventory.add(ItemId(i));
        }
    }

    for(int i = 0; i < int(RoomId::COUNT); ++i)
    {
        room(RoomId(i)).load_state(data.room_state[i]);
    }

    return RoomId(data.current_room);
}

void SaveGame::save(RoomId current_room)
{
    SaveData data{};
    data.magic = magic;
    data.current_room = uint8_t(current_room);

    GameState& game_state = GameState::instance();
    data.hp = uint8_t(game_state.stat(Attribute::HP));
    data.sanity = uint8_t(game_state.stat(Attribute::SANITY));
    data.luck = uint8_t(game_state.stat(Attribute::LUCK));

    Inventory& inventory = Inventory::instance();
    data.inventory_mask = 0;

    for(int i = 0; i < int(ItemId::COUNT); ++i)
    {
        if(inventory.has(ItemId(i)))
        {
            data.inventory_mask |= uint16_t(1 << i);
        }
    }

    for(int i = 0; i < int(RoomId::COUNT); ++i)
    {
        data.room_state[i] = room(RoomId(i)).save_state();
    }

    bn::sram::write(data);
}

}
