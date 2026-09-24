#ifndef SG_DIALOGUE_GHOSTLY_BRIDE_H
#define SG_DIALOGUE_GHOSTLY_BRIDE_H

#include "dialogue.h"

// Ported from D:\Workspace\Godot\sanguine\dialogue\ghostlybride.gd —
// trimmed to one short line per node, same as Prisoner, but keeping the
// branch shape: Address/Approach/Wait, then a deeper set of questions that
// converge on either asking what binds her here or trying (INT check) to
// understand her, both of which end the conversation for good.
namespace sg::dialogue
{

class GhostlyBride : public Dialogue
{

public:
    // True once the ghost has departed (any of the three ending branches) —
    // Chapel uses this to stop offering "Focus on the uncanniness" again.
    bool departed() const
    {
        return _departed;
    }

    void set_departed(bool departed)
    {
        _departed = departed;
    }

protected:
    void initialize_choices() override;
    void on_choice_selected(int id, RoomScreen& screen) override;

private:
    bool _departed = false;
};

}

#endif
