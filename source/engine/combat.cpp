#include "combat.h"

#include "bn_string.h"
#include "bn_vector.h"

#include "game_state.h"
#include "inventory.h"
#include "rng.h"
#include "../ui/room_screen.h"

namespace sg
{

namespace
{
    // Fixed option ids so a picked label's meaning doesn't depend on which
    // optional options happened to be shown before it (see
    // get_player_fight_choices()' variable-length choice list).
    constexpr int hand_to_hand_id = 0;
    constexpr int use_restorative_id = 1;
    constexpr int use_healing_id = 2;
    constexpr int flee_id = 3;
}

CombatResult Combat::fight(RoomScreen& screen, EnemyId enemy_id)
{
    Enemy& target = enemy(enemy_id);
    target.reset_hp();
    screen.show_enemy(enemy_id);

    bn::string<256> intro;
    intro.append("You are attacked by ");
    intro.append(target.name);
    intro.append(".\n\n");
    intro.append(target.description);
    screen.show_text(intro);

    while(true)
    {
        bn::vector<bn::string<40>, 4> labels;
        bn::vector<int, 4> ids;

        labels.push_back("Hand to Hand");
        ids.push_back(hand_to_hand_id);

        if(Inventory::instance().has(ItemId::RESTORATIVE_MIXTURE))
        {
            labels.push_back("Use restorative mixture");
            ids.push_back(use_restorative_id);
        }

        if(Inventory::instance().has(ItemId::HEALING_TINCTURE))
        {
            labels.push_back("Use healing tincture");
            ids.push_back(use_healing_id);
        }

        labels.push_back("Flee");
        ids.push_back(flee_id);

        int picked = ids[screen.choose(labels)];

        if(picked == use_restorative_id)
        {
            GameState::instance().change_sanity(3);
            Inventory::instance().remove(ItemId::RESTORATIVE_MIXTURE);
            screen.show_text("You use the restorative mixture.");
            continue;
        }

        if(picked == use_healing_id)
        {
            GameState::instance().take_damage(-4);
            Inventory::instance().remove(ItemId::HEALING_TINCTURE);
            screen.show_text("You use the healing tincture.");
            continue;
        }

        if(picked == flee_id)
        {
            bn::string<64> text = "You flee from ";
            text.append(target.name);
            text.append("!");
            screen.show_text(text);
            target.do_action(screen);
            screen.hide_enemy();
            return CombatResult::FLED;
        }

        // Hand to Hand: an unarmed STR check vs the enemy's dc, same dice
        // flow as any skill check (hand_to_hand_result in
        // gamestate_singleton.gd).
        Result result = screen.roll(Attribute::STR, target.dc, false, "Hand to Hand");

        if(result == Result::SUCCESS)
        {
            // Dario's unarmed damage die (randi_range(1,2) in
            // hand_to_hand_result — other characters aren't part of this
            // slice yet), STR bonus, minimum 2, +3 while carrying a
            // Homunculus.
            int hit_damage = rng().get_int(1, 3) + convert_stat_to_bonus(GameState::instance().stat(Attribute::STR));

            if(hit_damage < 2)
            {
                hit_damage = 2;
            }

            if(Inventory::instance().has(ItemId::HOMUNCULUS))
            {
                hit_damage += 3;
            }

            target.take_player_damage(hit_damage);

            bn::string<64> text = "And hits for ";
            text.append(bn::to_string<8>(hit_damage));
            text.append(" damage.");
            screen.show_text(text);
        }
        else
        {
            screen.show_text("And misses.");
        }

        if(target.is_dead())
        {
            bn::string<64> text = "You defeat ";
            text.append(target.name);
            text.append(".");
            screen.show_text(text);
            screen.hide_enemy();
            return CombatResult::WON;
        }

        target.do_action(screen);
    }
}

}
