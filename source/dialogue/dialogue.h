#ifndef SG_DIALOGUE_H
#define SG_DIALOGUE_H

#include "bn_vector.h"
#include "bn_string.h"
#include "bn_string_view.h"

#include "../ui/room_screen.h"

// Ported from D:\Workspace\Godot\sanguine\dialogue\dialogue.gd — branching
// NPC conversation (query -> reply, some choices only appearing after an
// earlier one was picked). Every reply here is trimmed hard against the
// source project's much longer prose (see rooms/dungeon.cpp's Prisoner):
// only the beats that matter to THIS floor's story survive, one short line
// each, since GBA text-box space is the scarcest resource in the whole
// port — not a place to preserve the PC version's full monologues.
namespace sg
{

enum class DialogueChoiceType
{
    NORMAL,     // stays in the list after being picked
    ONCE,       // removed from the list after being picked
    HIDDEN,     // not shown until show_dialogue() reveals it
    QUIT        // ends the conversation once its reply is shown
};

class Dialogue
{

public:
    virtual ~Dialogue() = default;

    // Shows the opening line, then loops choose()/show reply/
    // on_choice_selected() until a QUIT choice is picked or nothing but
    // "Leave" remains.
    void talk(RoomScreen& screen, const bn::string_view& opening_line);

protected:
    static constexpr int max_choices = 8;

    void add_dialogue(int id, const bn::string_view& query, const bn::string_view& reply,
                       DialogueChoiceType type = DialogueChoiceType::ONCE);
    void add_quit(int id, const bn::string_view& query, const bn::string_view& reply);
    void show_dialogue(int id);
    void hide_dialogue(int id);

    // Populates the starting set of choices — called once per talk().
    virtual void initialize_choices() = 0;

    // Called right after a choice's reply is shown, so subclasses can
    // reveal/hide further choices or apply game-state effects (mirrors the
    // inline lambdas the source project's add_dialogue() callers used).
    virtual void on_choice_selected(int id)
    {
        (void)id;
    }

private:
    struct Entry
    {
        int id;
        bn::string<40> query;
        bn::string<200> reply;
        DialogueChoiceType type;
    };

    int _find(int id);

    bn::vector<Entry, max_choices> _choices;
};

}

#endif
