#include "dice.h"

#include "bn_core.h"
#include "bn_string.h"

#include "game_state.h"
#include "rng.h"

namespace sg
{

namespace
{
    bn::string_view attribute_name(Attribute attribute)
    {
        switch(attribute)
        {

        case Attribute::INT_:
            return "INT";

        case Attribute::STR:
            return "STR";

        case Attribute::DEX:
            return "DEX";

        case Attribute::SANITY:
            return "SANITY";

        case Attribute::LUCK:
            return "LUCK";

        case Attribute::HP:
        case Attribute::HEALTH:
            return "HP";

        default:
            return "?";
        }
    }

    void wait_for_text(ui::TextBox& text_box)
    {
        while(text_box.active())
        {
            text_box.update();
            bn::core::update();
        }
    }

    int wait_for_choice(ui::ChoiceList& choices, const bn::ivector<bn::string<40>>& options)
    {
        choices.set_options(options);

        while(true)
        {
            bn::optional<int> picked = choices.update();

            if(picked.has_value())
            {
                return picked.value();
            }

            bn::core::update();
        }
    }
}

Result Dice::roll_check(ui::TextBox& text_box, ui::ChoiceList& choices, Attribute attribute, int dc,
                          bool cancelable, const bn::string_view& title)
{
    GameState& game_state = GameState::instance();
    int target = game_state.roll_needed(dc);

    bn::string<64> intro;
    intro.append(title);
    intro.append("\n\nRoll ");
    intro.append(attribute_name(attribute));
    intro.append(" vs ");
    intro.append(bn::to_string<8>(target));
    text_box.show(intro);
    wait_for_text(text_box);

    bn::vector<bn::string<40>, 2> roll_options;
    roll_options.push_back("Roll dice");
    if(cancelable)
    {
        roll_options.push_back("Cancel");
    }

    int picked = wait_for_choice(choices, roll_options);

    if(cancelable && picked == 1)
    {
        return Result::CANCEL;
    }

    Result result;

    while(true)
    {
        int roll = rng().get_int(1, 21);
        int bonus = convert_stat_to_bonus(game_state.stat(attribute));
        int total = roll + bonus;

        if(roll == 1)
        {
            result = Result::CRITICAL_FAIL;
        }
        else if(total >= target)
        {
            result = Result::SUCCESS;

            if(roll == 20)
            {
                game_state.change_luck(1);
            }
        }
        else
        {
            result = Result::FAIL;
        }

        bn::string<64> result_text;
        result_text.append("Rolled ");
        result_text.append(bn::to_string<8>(roll));
        result_text.append(" + (");
        result_text.append(bn::to_string<8>(bonus));
        result_text.append(") = ");
        result_text.append(bn::to_string<8>(total));
        result_text.append(" vs ");
        result_text.append(bn::to_string<8>(target));
        result_text.append("\n\n");

        switch(result)
        {

        case Result::SUCCESS:
            result_text.append(roll == 20 ? "Critical success!" : "Success!");
            break;

        case Result::CRITICAL_FAIL:
            result_text.append("Critical failure!");
            break;

        default:
            result_text.append("Failure.");
            break;
        }

        text_box.show(result_text);
        wait_for_text(text_box);

        if(result != Result::FAIL || game_state.stat(Attribute::LUCK) <= 0)
        {
            break;
        }

        bn::vector<bn::string<40>, 2> reroll_options;
        reroll_options.push_back("Reroll (-1 Luck)");
        reroll_options.push_back("Accept");

        if(wait_for_choice(choices, reroll_options) == 1)
        {
            break;
        }

        game_state.change_luck(-1);
    }

    bn::vector<bn::string<40>, 1> continue_options;
    continue_options.push_back("Continue");
    wait_for_choice(choices, continue_options);

    return result;
}

}
