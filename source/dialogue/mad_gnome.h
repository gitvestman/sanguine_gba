#ifndef SG_DIALOGUE_MAD_GNOME_H
#define SG_DIALOGUE_MAD_GNOME_H

#include "dialogue.h"

// Ported from D:\Workspace\Godot\sanguine\dialogue\mad_gnome.gd — trimmed
// hard, same as Prisoner/GhostlyBride. The source tree branches on the
// player character (Milly gets a Danish-language sub-branch) and on
// carrying a Flintlock/Silver Dagger (an alternate "I'm armed" bluff path)
// — neither exists in this build (no character select, and while Silver
// Dagger exists as an item, the branch it unlocks is flavor-only, not
// worth the extra nodes), so only the one path reachable by a Dario
// player with no weapon survives: greet him, give your name or not, and
// he eventually attacks regardless — talking never avoids the fight, only
// delays it, matching the source (`show_unfriendly_final` always fires).
namespace sg::dialogue
{

class MadGnome : public Dialogue
{

protected:
    void initialize_choices() override;
    void on_choice_selected(int id, RoomScreen& screen) override;
};

}

#endif
