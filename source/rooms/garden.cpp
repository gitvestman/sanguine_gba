#include "garden.h"

#include "../ui/room_screen.h"
#include "../engine/combat.h"
#include "../engine/inventory.h"

#include "bn_sprite_items_enemy_mad_gnome.h"

namespace sg::rooms
{

namespace
{
    constexpr bn::string_view long_description =
        "Neat rows of vegetables stretch across the moonlit garden. A tattered scarecrow "
        "watches over them.";
    constexpr bn::string_view short_description = "The garden.";
    constexpr bn::string_view gnome_blurb = "\n\nA gruff, shrill voice threatens you from "
                                            "behind a gooseberry bush.";
}

void Garden::describe(bn::string<768>& out, bool force_long)
{
    out.append((! force_long && visited) ? short_description : long_description);
    visited = true;

    if(_searched && ! enemy(EnemyId::MadGnome).is_dead())
    {
        out.append(gnome_blurb);
    }
}

void Garden::build_choices(bn::ivector<Choice>& out)
{
    out.push_back({ "Look", 0 });
    out.push_back({ "E", 1 });
    out.push_back({ "N", 2 });

    if(! _searched)
    {
        out.push_back({ "Search", 3 });
    }
    else if(! enemy(EnemyId::MadGnome).is_dead())
    {
        out.push_back({ "Talk to the voice", 4 });
    }
}

void Garden::on_choice(int action_id, RoomScreen& screen)
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
        screen.goto_room(RoomId::GroundskeeperCottage);
        break;

    case 2:
        screen.goto_room(RoomId::Orangery);
        break;

    case 3:
        _searched = true;
        screen.show_text("Among the crops, something rustles and hisses a threat in a "
                          "foreign tongue.");
        break;

    case 4:
        _gnome.talk(screen, "You approach the bush.", &bn::sprite_items::enemy_mad_gnome, 2);

        if(enemy(EnemyId::MadGnome).is_dead())
        {
            Inventory::instance().add(ItemId::RESTORATIVE_MIXTURE);
            screen.show_text("The gnome collapses. Beside his body, you find a bottle of "
                              "restorative mixture.");
        }

        break;

    default:
        break;
    }
}

uint16_t Garden::save_state() const
{
    uint16_t bits = Room::save_state();
    if(_searched) bits |= 1 << 1;
    return bits;
}

void Garden::load_state(uint16_t bits)
{
    Room::load_state(bits);
    _searched = bits & (1 << 1);
}

}
