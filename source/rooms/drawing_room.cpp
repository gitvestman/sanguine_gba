#include "drawing_room.h"

#include "../ui/room_screen.h"
#include "../engine/combat.h"

#include "bn_sprite_items_enemy_night_raven.h"

namespace sg::rooms
{

namespace
{
    constexpr bn::string_view long_description =
        "An elegant drawing room, its paintings covered by dust sheets. Something perched in "
        "the shadows watches you with unnatural eyes.";
    constexpr bn::string_view short_description = "The drawing room.";
    constexpr bn::string_view resolved_description = "The drawing room, quiet now.";

    bool raven_resolved(bool raven_gone)
    {
        return raven_gone || enemy(EnemyId::NightRaven).is_dead();
    }
}

void DrawingRoom::describe(bn::string<768>& out, bool force_long)
{
    if(raven_resolved(_raven_gone))
    {
        out.append((! force_long && visited) ? short_description : resolved_description);
        visited = true;
        return;
    }

    if(_scare_failed)
    {
        out.append("The raven shrieks and dives at your face, enraged.");
    }
    else if(_greeted)
    {
        out.append("The raven cocks its head, studying you with an eerie intelligence.");
    }
    else
    {
        out.append(long_description);
    }

    visited = true;
}

void DrawingRoom::on_enter(RoomScreen& screen)
{
    if(! raven_resolved(_raven_gone))
    {
        screen.show_overlay_sprite(bn::sprite_items::enemy_night_raven);
    }
}

void DrawingRoom::build_choices(bn::ivector<Choice>& out)
{
    if(raven_resolved(_raven_gone))
    {
        out.push_back({ "Look", 0 });
        out.push_back({ "W", 1 });

        if(! _searched)
        {
            out.push_back({ "Search", 2 });
        }

        return;
    }

    if(_scare_failed)
    {
        out.push_back({ "Defend yourself", 3 });
        return;
    }

    out.push_back({ "Attack", 3 });

    if(! _greeted)
    {
        out.push_back({ "Greet it", 4 });
        out.push_back({ "W", 1 });

        if(! _identify_attempted)
        {
            out.push_back({ "Identify the creature", 5 });
        }
    }
    else
    {
        out.push_back({ "W", 1 });

        if(_identify_success)
        {
            out.push_back({ "Give it a shiny bauble", 6 });
        }
    }

    if(! _scare_attempted)
    {
        out.push_back({ "Scare it away", 7 });
    }
}

void DrawingRoom::on_choice(int action_id, RoomScreen& screen)
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
        screen.goto_room(RoomId::FancyCorridor02);
        break;

    case 2:
        _searched = true;
        screen.show_text("Dust sheets and forgotten furniture. Nothing worth taking.");
        break;

    case 3:
        Combat::fight(screen, EnemyId::NightRaven);

        if(! enemy(EnemyId::NightRaven).is_dead())
        {
            screen.show_overlay_sprite(bn::sprite_items::enemy_night_raven);
        }

        break;

    case 4:
        _greeted = true;
        screen.show_text("You speak softly to it. It doesn't flee - if anything, it seems "
                          "to be listening.");
        break;

    case 5:
    {
        _identify_attempted = true;
        Result result = screen.roll(Attribute::INT_, 10, false, "Identify the creature");

        if(result == Result::SUCCESS)
        {
            _identify_success = true;
            screen.show_text("It isn't quite a raven at all - something stranger, drawn to "
                              "bright, shiny things.");
        }
        else
        {
            screen.show_text("You can't place what it is, only that its eyes are wrong.");
        }

        break;
    }

    case 6:
        _raven_gone = true;
        screen.hide_overlay_sprite();
        screen.show_text("You toss down a bright bauble. The creature snatches it up and "
                          "flees through a gap in the ceiling.");
        break;

    case 7:
    {
        _scare_attempted = true;
        Result result = screen.roll(Attribute::STR, 10, false, "Scare it away");

        if(result == Result::SUCCESS)
        {
            _raven_gone = true;
            screen.hide_overlay_sprite();
            screen.show_text("You lunge and shout. Startled, it bolts through a gap in the "
                              "ceiling.");
        }
        else
        {
            _scare_failed = true;
            screen.show_text("It doesn't scare. If anything, you've enraged it.");
        }

        break;
    }

    default:
        break;
    }
}

void DrawingRoom::build_ambient(bn::ivector<AmbientDecor>& out) const
{
    // The room's blinking stars/eyes overlays (blinking_stars/blinking_eyes
    // in the source) are a separate, state-dependent ambient effect, not
    // ported here — this is only the three unconditional candles.
    out.push_back({ AmbientKind::CANDLE, -52, -47 });
    out.push_back({ AmbientKind::CANDLE, -42, -47 });
    out.push_back({ AmbientKind::CANDLE, -32, -47 });
}

uint16_t DrawingRoom::save_state() const
{
    uint16_t bits = Room::save_state();
    if(_raven_gone) bits |= 1 << 1;
    if(_searched) bits |= 1 << 2;
    if(_greeted) bits |= 1 << 3;
    if(_scare_failed) bits |= 1 << 4;
    if(_identify_attempted) bits |= 1 << 5;
    if(_identify_success) bits |= 1 << 6;
    if(_scare_attempted) bits |= 1 << 7;
    return bits;
}

void DrawingRoom::load_state(uint16_t bits)
{
    Room::load_state(bits);
    _raven_gone = bits & (1 << 1);
    _searched = bits & (1 << 2);
    _greeted = bits & (1 << 3);
    _scare_failed = bits & (1 << 4);
    _identify_attempted = bits & (1 << 5);
    _identify_success = bits & (1 << 6);
    _scare_attempted = bits & (1 << 7);
}

}
