#ifndef SG_DIALOGUE_H
#define SG_DIALOGUE_H

#include "bn_vector.h"
#include "bn_string.h"
#include "bn_string_view.h"
#include "bn_sprite_item.h"
#include "bn_fixed.h"

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
    QUIT        // ends the conversation once its reply is shown
};

class Dialogue
{

public:
    virtual ~Dialogue() = default;

    // Shows the opening line, then loops choose()/show reply/
    // on_choice_selected() until a QUIT choice is picked or no choices
    // remain. `portrait`, when given, is overlaid on the room background
    // for the whole conversation (same slot Combat::fight uses for its
    // enemy sprite) — omit it for an NPC with no dedicated art (e.g.
    // Prisoner, which the source project never gave one either).
    // `portrait_scale` enlarges small source art (see MadGnome, whose
    // portrait would otherwise read as tiny next to every other NPC).
    // `show_leave_fallback` adds a no-consequence "Leave" choice whenever
    // the conversation's own defined choices run out — the source project's
    // dialogue.gd has no such automatic fallback at all, so a tree that
    // already defines its own way out (see Countess's "Take your leave" ->
    // dinner invitation) should pass false, or the auto-added "Leave" sits
    // right next to the real exit as a silent decoy that skips whatever
    // that real choice was about to trigger.
    void talk(RoomScreen& screen, const bn::string_view& opening_line,
              const bn::sprite_item* portrait = nullptr, bn::fixed portrait_scale = 1,
              bool show_leave_fallback = true);

protected:
    // GhostlyBride pre-registers every node of its branching tree up front
    // (as HIDDEN) so show_dialogue()/hide_dialogue() can reveal/hide them —
    // 10 distinct ids, so this needs headroom above Prisoner's simpler 5.
    static constexpr int max_choices = 12;

    // `hidden` registers the node without showing it yet — for a whole
    // branching tree pre-registered up front (see GhostlyBride), so
    // show_dialogue()/hide_dialogue() can reveal/hide it later without
    // changing what it actually does when picked (its `type` stays fixed;
    // only visibility toggles).
    void add_dialogue(int id, const bn::string_view& query, const bn::string_view& reply,
                       DialogueChoiceType type = DialogueChoiceType::ONCE, bool hidden = false);
    void add_quit(int id, const bn::string_view& query, const bn::string_view& reply,
                  bool hidden = false);
    void show_dialogue(int id);
    void hide_dialogue(int id);

    // Populates the starting set of choices — called once per talk().
    virtual void initialize_choices() = 0;

    // Called right after a choice's reply is shown, so subclasses can
    // reveal/hide further choices or apply game-state effects (mirrors the
    // inline lambdas the source project's add_dialogue() callers used).
    // `screen` lets a node trigger its own extra beat (a skill check, an
    // extra line) beyond the plain reply text — see GhostlyBride's
    // "try to understand her" INT check.
    virtual void on_choice_selected(int id, RoomScreen& screen)
    {
        (void)id;
        (void)screen;
    }

private:
    struct Entry
    {
        int id;
        bn::string<40> query;
        bn::string<200> reply;
        DialogueChoiceType type;
        bool hidden;
    };

    int _find(int id);

    bn::vector<Entry, max_choices> _choices;
};

}

#endif
