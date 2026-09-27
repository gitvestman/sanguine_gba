#include "dinner_hall.h"

#include "../ui/room_screen.h"
#include "../engine/game_state.h"
#include "../engine/combat.h"
#include "../engine/settings.h"
#include "../engine/music.h"
#include "fancy_corridor_01.h"

#include "bn_sprite_items_enemy_countess.h"
#include "bn_music_items.h"
#include "bn_music.h"

namespace sg::rooms
{

namespace
{
    constexpr bn::string_view long_description =
        "The ruined dining hall reeks of old grease and violence. A crushing dread grips "
        "your soul.";
    constexpr bn::string_view short_description = "The dining hall.";

    dialogue::Countess& countess()
    {
        return static_cast<FancyCorridor01&>(room(RoomId::FancyCorridor01)).countess();
    }

    bool dinner_active()
    {
        return countess().dinner_pending() && ! enemy(EnemyId::Countess).is_dead();
    }
}

void DinnerHall::describe(bn::string<768>& out, bool force_long)
{
    if(dinner_active())
    {
        out.append("The Countess waits at the head of a lavishly set table, a single "
                    "place laid across from her.");
        visited = true;
        return;
    }

    out.append((! force_long && visited) ? short_description : long_description);
    visited = true;
}

void DinnerHall::on_enter(RoomScreen& screen)
{
    if(dinner_active())
    {
        screen.show_overlay_sprite(bn::sprite_items::enemy_countess);
    }
}

void DinnerHall::build_choices(bn::ivector<Choice>& out)
{
    if(dinner_active())
    {
        out.push_back({ "Attack her", 6 });
        out.push_back({ "Dine with her", 7 });
        return;
    }

    if(! _trap_sprung)
    {
        out.push_back({ "Resist the dread", 1 });
        return;
    }

    out.push_back({ "Look", 0 });
    out.push_back({ "N", 2 });
    out.push_back({ "W", 3 });
    out.push_back({ "S", 4 });

    if(! _searched)
    {
        out.push_back({ "Search", 5 });
    }
}

void DinnerHall::on_choice(int action_id, RoomScreen& screen)
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
        Result result = screen.roll(Attribute::INT_, 10, false, "Resist the dread");

        if(result == Result::SUCCESS)
        {
            screen.show_text("You steady your mind and shake off the worst of it.");
        }
        else
        {
            GameState::instance().change_sanity(-1);
            screen.show_text("The dread claws at your mind. You lose 1 Sanity.");
        }

        _trap_sprung = true;
        break;
    }

    case 2:
        screen.goto_room(RoomId::MusicRoom);
        break;

    case 3:
        screen.goto_room(RoomId::Kitchen);
        break;

    case 4:
        screen.goto_room(RoomId::GreatHall);
        break;

    case 5:
        _searched = true;
        screen.show_text("Broken chairs and mildewed linen. Nothing worth taking.");
        break;

    case 6:
        screen.hide_overlay_sprite();
        screen.show_text("You overturn the table and lunge at her.");
        Combat::fight(screen, EnemyId::Countess);
        countess().set_dinner_pending(false);

        if(enemy(EnemyId::Countess).is_dead())
        {
            countess().set_resolved(true);
        }

        break;

    case 7:
        screen.hide_overlay_sprite();
        countess().set_dinner_pending(false);
        countess().set_resolved(true);
        GameState::instance().change_sanity(1);
        // Her theme plays for this scene same as it does for any of her
        // other encounters (Combat::fight handles this itself for "Attack
        // her" above; this peaceful path has no fight to piggyback on).
        bn::music::play(bn::music_items::countess_theme, Settings::instance().music_volume());
        screen.show_text("You dine with her, trading pleasantries you don't mean over a "
                          "meal you don't taste. When it's done, she retires without a "
                          "word, seemingly satisfied. You feel steadier for having survived "
                          "it.");
        play_default_theme();
        break;

    default:
        break;
    }
}

void DinnerHall::build_ambient(bn::ivector<AmbientDecor>& out) const
{
    out.push_back({ AmbientKind::CANDLE, -10, -49 });
    out.push_back({ AmbientKind::CANDLE, 0, -49 });
    out.push_back({ AmbientKind::CANDLE, 10, -49 });
}

uint16_t DinnerHall::save_state() const
{
    uint16_t bits = Room::save_state();
    if(_trap_sprung) bits |= 1 << 1;
    if(_searched) bits |= 1 << 2;
    return bits;
}

void DinnerHall::load_state(uint16_t bits)
{
    Room::load_state(bits);
    _trap_sprung = bits & (1 << 1);
    _searched = bits & (1 << 2);
}

}
