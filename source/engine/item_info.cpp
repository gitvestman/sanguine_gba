#include "item_info.h"

#include "game_state.h"
#include "../ui/room_screen.h"

#include "bn_sprite_items_item_iron_key.h"
#include "bn_sprite_items_item_skull_key.h"
#include "bn_sprite_items_item_lockpicks.h"
#include "bn_sprite_items_item_silver_dagger.h"
#include "bn_sprite_items_item_flintlock.h"
#include "bn_sprite_items_item_restorative_mixture.h"
#include "bn_sprite_items_item_healing_tincture.h"
#include "bn_sprite_items_item_homunculus.h"
#include "bn_sprite_items_item_cog.h"
#include "bn_sprite_items_item_journal.h"
#include "bn_sprite_items_item_strange_figurine.h"
#include "bn_sprite_items_item_doll.h"
#include "bn_sprite_items_item_bastet_figurine.h"
#include "bn_sprite_items_item_toy_soldier.h"

namespace sg
{

namespace
{
    constexpr ItemInfo infos[int(ItemId::COUNT)] = {
        { "Iron Key", "A heavy iron key. Opens iron-barred doors." },
        { "Skull Key", "An ornate key shaped like a skull. Opens any iron-barred door." },
        { "Lockpicks", "A set of thin lockpicks. Useful for prying open locks." },
        { "Silver Dagger", "A dagger of pure silver, cold to the touch." },
        { "Flintlock Pistol", "An old flintlock pistol, packed with powder and lead balls." },
        { "Restorative Mixture", "A bitter tonic that steadies the nerves. Restores 3 Sanity." },
        { "Healing Tincture", "A sharp-smelling tincture that closes wounds. Restores 4 HP." },
        { "Homunculus", "A small, unnervingly warm effigy. Strengthens your unarmed blows." },
        { "Brass Cog", "A small precision cog from some larger mechanism." },
        { "Journal", "A stranger's journal, its later pages lost to frantic scrawl." },
        { "Strange Figurine", "A carved figurine of an unfamiliar shape." },
        { "Porcelain Doll", "A porcelain doll with an eerily lifelike painted face." },
        { "Bastet Figurine", "A small bronze figurine of a cat-headed goddess. Steadies your hand." },
        { "Sheet Music", "A folded, hand-written sheet of music." },
        { "Toy Soldier", "A small clockwork toy soldier, still ticking faintly." },
    };
}

const ItemInfo& item_info(ItemId item)
{
    return infos[int(item)];
}

const bn::sprite_item& item_icon(ItemId item)
{
    switch(item)
    {

    case ItemId::IRON_KEY:
        return bn::sprite_items::item_iron_key;

    case ItemId::SKULL_KEY:
        return bn::sprite_items::item_skull_key;

    case ItemId::LOCKPICKS:
        return bn::sprite_items::item_lockpicks;

    case ItemId::SILVER_DAGGER:
        return bn::sprite_items::item_silver_dagger;

    case ItemId::FLINTLOCK:
        return bn::sprite_items::item_flintlock;

    case ItemId::RESTORATIVE_MIXTURE:
        return bn::sprite_items::item_restorative_mixture;

    case ItemId::HEALING_TINCTURE:
        return bn::sprite_items::item_healing_tincture;

    case ItemId::HOMUNCULUS:
        return bn::sprite_items::item_homunculus;

    case ItemId::COG:
        return bn::sprite_items::item_cog;

    case ItemId::JOURNAL:
        return bn::sprite_items::item_journal;

    case ItemId::STRANGE_FIGURINE:
        return bn::sprite_items::item_strange_figurine;

    case ItemId::DOLL:
        return bn::sprite_items::item_doll;

    case ItemId::BASTET_FIGURINE:
        return bn::sprite_items::item_bastet_figurine;

    case ItemId::TOY_SOLDIER:
        return bn::sprite_items::item_toy_soldier;

    // Sheet Music has no dedicated icon in the source project either;
    // falls back to the Cog's icon rather than adding a placeholder.
    case ItemId::SHEET_MUSIC:
    default:
        return bn::sprite_items::item_cog;
    }
}

bool item_is_usable(ItemId item)
{
    return item == ItemId::RESTORATIVE_MIXTURE || item == ItemId::HEALING_TINCTURE;
}

void use_item(ItemId item, RoomScreen& screen)
{
    switch(item)
    {

    case ItemId::RESTORATIVE_MIXTURE:
        GameState::instance().change_sanity(3);
        Inventory::instance().remove(item);
        screen.show_text("You use the restorative mixture.");
        break;

    case ItemId::HEALING_TINCTURE:
        GameState::instance().take_damage(-4);
        Inventory::instance().remove(item);
        screen.show_text("You use the healing tincture.");
        break;

    default:
        break;
    }
}

}
