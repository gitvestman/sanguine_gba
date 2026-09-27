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

        // The flintlock's remaining shots (see Inventory::flintlock_ammo) —
        // the one piece of inventory state that isn't just an owned/not-owned
        // bit, so it needs its own field alongside inventory_mask.
        uint8_t flintlock_ammo;

        // Widened from uint8_t once Cemetery's Myling encounter (Phase 4c)
        // needed more than 8 bits of state for one room — a GBA SRAM chip
        // has many KB to spare, so this is just a self-imposed sanity
        // bound below, not a hardware limit.
        uint16_t room_state[int(RoomId::COUNT)];
    };

    static_assert(sizeof(SaveData) <= SaveGame::slot_size,
                  "Save data has grown past its reserved SRAM slot size");

    // The slot save()/load_game()/start_new_game() operate on — set by
    // whichever of the latter two ran most recently, so every autosave
    // during play lands in the slot the player actually picked at the title
    // screen without threading a slot parameter through every save() call
    // site (there's only ever one active game per session anyway).
    int active_slot = 0;

    int slot_offset(int slot)
    {
        return slot * SaveGame::slot_size;
    }
}

bool SaveGame::save_exists(int slot)
{
    SaveData data{};
    bn::sram::read_offset(data, slot_offset(slot));
    return data.magic == magic;
}

bool SaveGame::any_save_exists()
{
    for(int i = 0; i < slot_count; ++i)
    {
        if(save_exists(i))
        {
            return true;
        }
    }

    return false;
}

RoomId SaveGame::peek_room(int slot)
{
    SaveData data{};
    bn::sram::read_offset(data, slot_offset(slot));

    if(data.magic != magic)
    {
        return RoomId::OutsideGates;
    }

    return RoomId(data.current_room);
}

RoomId SaveGame::load_game(int slot)
{
    active_slot = slot;

    SaveData data{};
    bn::sram::read_offset(data, slot_offset(slot));

    if(data.magic != magic)
    {
        return RoomId::OutsideGates;
    }

    GameState& game_state = GameState::instance();
    game_state.load_stat(Attribute::HP, data.hp);
    game_state.load_stat(Attribute::SANITY, data.sanity);
    game_state.load_stat(Attribute::LUCK, data.luck);

    Inventory& inventory = Inventory::instance();

    // inventory_mask has no concept of pickup order, so a loaded save's
    // discovery_order() just ends up in plain ItemId order — a cosmetic
    // difference from a save actually played through, not a correctness
    // issue (nothing depends on it beyond the sidebar/browser's own
    // ordering).
    for(int i = 0; i < int(ItemId::COUNT); ++i)
    {
        if(data.inventory_mask & (1 << i))
        {
            inventory.add(ItemId(i));
        }
    }

    inventory.set_flintlock_ammo(data.flintlock_ammo);

    for(int i = 0; i < int(RoomId::COUNT); ++i)
    {
        room(RoomId(i)).load_state(data.room_state[i]);
    }

    return RoomId(data.current_room);
}

void SaveGame::start_new_game(int slot)
{
    active_slot = slot;
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

    data.flintlock_ammo = uint8_t(inventory.flintlock_ammo());

    for(int i = 0; i < int(RoomId::COUNT); ++i)
    {
        data.room_state[i] = room(RoomId(i)).save_state();
    }

    bn::sram::write_offset(data, slot_offset(active_slot));
}

}
