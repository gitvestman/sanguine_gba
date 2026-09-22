# Sanguine — GBA Port

A from-scratch Game Boy Advance port of **Sanguine** (`D:\Workspace\Godot\sanguine`, Godot 4.7).
See [IMPLEMENTATION_PLAN.md](IMPLEMENTATION_PLAN.md) for the full design/roadmap.

**Status:** Phase 0 (toolchain) and Phase 1 (vertical slice) are done. The whole Basement
floor (10 rooms) is playable: room-to-room navigation, paginated story text, a D-pad choice
menu, and real d20 skill checks against Sanguine's actual dice math. No combat, dialogue,
save/load, or inventory UI yet (later phases) — a couple of rooms that would normally start a
fight or a conversation show a short placeholder line instead and let you carry on.

## Prerequisites

- **devkitARM + Butano**, installed via devkitPro's pacman under **WSL2 Ubuntu** (not native
  Windows — see the *Toolchain* note below for why).
- **mGBA** (`mgba-qt`) for running the ROM. On Windows 11, WSLg forwards its window straight
  to your desktop, so it can also run from inside WSL.

If devkitARM/Butano/mgba-qt aren't installed yet, see the one-time setup commands recorded in
this project's memory (`project_toolchain_setup.md`) — in short:

```bash
sudo apt update
sudo apt install -y build-essential mgba-qt
sudo dkp-pacman -S gba-dev   # after installing devkitPro's pacman
```

## Building

From the project root, inside WSL (a Windows path like `D:\...` is `/mnt/d/...` in WSL):

```bash
make -j$(nproc)
```

This produces `sanguine_gba.gba` in the project root. `make clean` removes the `build/`
directory and the ROM.

## Running / testing

There's no automated test suite (nothing here is unit-testable in the traditional sense yet —
it's UI-and-content-driven). To check a change:

```bash
mgba-qt sanguine_gba.gba
```

- **D-pad up/down** — move the choice-menu cursor (`>` marks the selection)
- **A** — confirm a choice, advance a paginated text page, or roll the dice in a skill check
- Start in the **Cellar Hall**; every door, search, and trap on the Basement floor is wired up
  (see [IMPLEMENTATION_PLAN.md](IMPLEMENTATION_PLAN.md) for which rooms make up that floor)

If you change room text/logic, rebuild and relaunch `mgba-qt` — there's no hot reload.

To sanity-check a build without a display (e.g. after changing engine code rather than
content), it's enough that `make` succeeds and `sanguine_gba.gba` gets produced; booting it in
mGBA is the real check that nothing crashes or hangs.

## Project layout

```
source/
  main.cpp            Entry point
  engine/              Ported GameState/dice/inventory/constants (no Godot/UI dependency)
  rooms/                One .h/.cpp pair per Basement room, plus room_registry.cpp
  ui/                   Panel/TextBox/ChoiceList/RoomScreen — the GBA-side "game.gd"
assets/
  rooms/                Room background art, converted from the source project's PNGs
                         by tools/convert_room_bg.py (indexed BMP + JSON per Butano's
                         graphics pipeline)
  sprites/              Small sprite assets (currently just the text-box panel tile)
third_party/butano/      Butano engine (git submodule)
tools/
  convert_room_bg.py     PNG -> Butano-ready room background converter
```

## Toolchain note

devkitARM/Butano are set up under WSL2 Ubuntu rather than natively on Windows because that's
a much smoother path for this exact stack (apt/pacman packages, and WSLg forwards `mgba-qt`'s
window to the desktop with no extra setup) — see this project's saved memory
(`project_toolchain_setup.md`) for the reasoning, the exact install commands, and a couple of
gotchas hit along the way (notably: `apt.devkitpro.org` blocks `wget`/`curl` behind Cloudflare,
and installing devkitPro needs `sudo`, which has to be run interactively by you, not by an
agent).
