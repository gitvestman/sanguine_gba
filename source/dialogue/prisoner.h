#ifndef SG_DIALOGUE_PRISONER_H
#define SG_DIALOGUE_PRISONER_H

#include "dialogue.h"

// Ported from D:\Workspace\Godot\sanguine\dialogue\prisoner.gd — drastically
// cut down. The source version has a whole second branch of dialogue that
// only appears for the Milly character and a page of prose per reply; this
// port has one player character (Dario) and a hard per-string budget, so
// only the beats that carry the floor's plot (why he's here, who "she" is,
// the choice to help him) survive.
namespace sg::dialogue
{

class Prisoner : public Dialogue
{

public:
    // True once the player has dressed his wounds or given up an item to
    // help him — Dungeon uses this to show a short revisit line instead of
    // reopening the full conversation.
    bool resolved() const
    {
        return _resolved;
    }

    // For restoring from a save — see Dungeon::load_state.
    void set_resolved(bool resolved)
    {
        _resolved = resolved;
    }

protected:
    void initialize_choices() override;
    void on_choice_selected(int id, RoomScreen& screen) override;

private:
    bool _resolved = false;
};

}

#endif
