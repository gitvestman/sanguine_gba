#ifndef SG_DIALOGUE_GOLEM_H
#define SG_DIALOGUE_GOLEM_H

#include "dialogue.h"

// Ported from D:\Workspace\Godot\sanguine\dialogue\golem.gd — trimmed
// hard. The source has two full initializer paths (initialize_intro vs.
// initialize_threatened) depending on whether the player addressed or
// threatened the golem first, each with its own multi-node negotiation;
// here that first choice is just flavor text ScientificLaboratory shows
// before talk() starts (see its Address/Threaten choices), and this
// dialogue only covers what's common to both outcomes: back off, or
// don't.
namespace sg::dialogue
{

class Golem : public Dialogue
{

public:
    bool resolved() const
    {
        return _resolved;
    }

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
