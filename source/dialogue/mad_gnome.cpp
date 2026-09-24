#include "mad_gnome.h"

#include "../engine/combat.h"

namespace sg::dialogue
{

namespace
{
    enum ChoiceId
    {
        PARDON,
        GERMAN,
        WHO_ARE_YOU,
        MY_NAME,
        UNIMPORTANT,
        QUESTION,
        STEP_CLOSER,
        MOVE_CLOSER
    };
}

void MadGnome::initialize_choices()
{
    add_dialogue(PARDON, "\"I beg your pardon?\"",
                 "\"Rørt ved mine grøntsager?\" the voice snarls, still hidden in the bush.");
    add_dialogue(GERMAN, "Greet him in German",
                 "The voice pauses, confused, then snarls back in the same broken tongue.");

    add_dialogue(WHO_ARE_YOU, "\"Who are you?\"", "A wild-eyed gnome steps out, clutching a "
                                                   "rusty blade.", DialogueChoiceType::ONCE, true);
    add_dialogue(MY_NAME, "Give your name", "\"A fine name for a fine trespasser,\" he mutters.",
                 DialogueChoiceType::ONCE, true);
    add_dialogue(UNIMPORTANT, "\"That's unimportant.\"",
                 "\"Unimportant, is it? We'll see.\" His grip tightens on the blade.",
                 DialogueChoiceType::ONCE, true);
    add_dialogue(QUESTION, "Ask about the garden",
                 "\"Mine to guard, mine to keep. Questions won't save you now.\"",
                 DialogueChoiceType::ONCE, true);
    add_dialogue(STEP_CLOSER, "Step closer",
                 "\"Don't. Come. Closer.\" His blade trembles with barely-contained fury.",
                 DialogueChoiceType::ONCE, true);
    add_quit(MOVE_CLOSER, "Move closer anyway", "He lunges before you finish the step.", true);
}

void MadGnome::on_choice_selected(int id, RoomScreen& screen)
{
    switch(id)
    {

    case PARDON:
    case GERMAN:
        show_dialogue(WHO_ARE_YOU);
        break;

    case WHO_ARE_YOU:
        show_dialogue(MY_NAME);
        show_dialogue(UNIMPORTANT);
        break;

    case MY_NAME:
        hide_dialogue(UNIMPORTANT);
        show_dialogue(STEP_CLOSER);
        break;

    case UNIMPORTANT:
        hide_dialogue(MY_NAME);
        show_dialogue(QUESTION);
        show_dialogue(STEP_CLOSER);
        break;

    case QUESTION:
        show_dialogue(STEP_CLOSER);
        break;

    case STEP_CLOSER:
        show_dialogue(MOVE_CLOSER);
        break;

    case MOVE_CLOSER:
        Combat::fight(screen, EnemyId::MadGnome);
        break;

    default:
        break;
    }
}

}
