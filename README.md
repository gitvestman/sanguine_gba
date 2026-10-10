# Sanguine — GBA Port

A from-scratch Game Boy Advance port of **Sanguine** (`D:\Workspace\Godot\sanguine`, Godot 4.7).
See [IMPLEMENTATION_PLAN.md](IMPLEMENTATION_PLAN.md) for the full design/roadmap.

**Status:** Phases 0-4 are complete and Phase 5 (audio) is largely done. The whole manor is
playable from title screen to escape: 53 rooms across the Basement, First/Second/Third floors
and the grounds, turn-based d20 combat with animated enemy overlays, NPC dialogue (Countess,
Ghostly Bride, Golem, Mad Gnome, Prisoner), a usable inventory, the chapel sliding-tile puzzle,
the Countess's boss fight and the escape endgame. Also in: three SRAM save slots (autosaved on
every room change), a pause menu with persisted music/SFX volume, ambient torch/candle
animations, a title screen + coach-ride intro, title/in-game/boss music and dice/stat SFX.
Remaining (Phase 6): polish, difficulty pass, ROM-size close-out. A browser test build for
itch.io lives in `web/`.

## Prerequisites

- **devkitARM + Butano**, installed via devkitPro's pacman under **WSL2 Ubuntu** (see
  *Toolchain note* below for why not native Windows).
- **mGBA** (`mgba-qt`) for running the ROM. On Windows 11, WSLg shows its window on your
  desktop (it appears under an "Ubuntu" entry in the taskbar).

One-time setup, run yourself in a WSL terminal (it needs `sudo`):

```bash
sudo apt update
sudo apt install -y build-essential mgba-qt
```

Then install devkitPro's pacman. `apt.devkitpro.org` blocks `wget`/`curl` (Cloudflare), so open
`https://apt.devkitpro.org/install-devkitpro-pacman` in a browser, save its contents as
`install-devkitpro-pacman`, and run:

```bash
chmod +x install-devkitpro-pacman
sudo ./install-devkitpro-pacman
sudo dkp-pacman -S gba-dev
```

Open a new terminal afterwards so `DEVKITPRO`/`DEVKITARM` are set. Butano itself is a git
submodule — after cloning, run `git submodule update --init`.

## Building

From the project root, inside WSL (a Windows path like `D:\...` is `/mnt/d/...` in WSL):

```bash
make -j$(nproc)
```

This produces `sanguine_gba.gba` in the project root. `make clean` removes `build/` and the ROM.

## Running / testing

There's no automated test suite; the game is UI- and content-driven. To check a change:

```bash
mgba-qt -f sanguine_gba.gba
```

Fullscreen (`-f`) avoids window-sizing artifacts under WSLg. mGBA's default keys:

| GBA | Keyboard | Does |
|---|---|---|
| D-pad | Arrow keys | Move the `>` cursor in menus (up/down or left/right) |
| A | X | Confirm a choice, advance a text page, roll the dice |
| B | Z | Cancel/back (save-slot picker, dice roll prompts, settings, sliding puzzle) |
| Start | Enter | Pause menu: Resume / Settings (music + SFX volume) / Main Menu |

Every room's choice list ends with **Inv**, which opens the inventory (item descriptions, and
"Use" for usable items). Progress autosaves to one of 3 SRAM slots on every room change; the
title screen offers New Game / Continue.

A new game starts with the title screen and coach-ride intro, then drops you at the **Outside Gates**. The screen shows the room art, the story text / choice menu in the bottom panel, your stats (HP, Sanity, Luck) on the right, and the items you carry as icons on the left.

To see Butano's `BN_LOG(...)` debug output without the GUI, write mGBA's log to a file:

```bash
mgba-qt -l 255 -C logToFile=1 -C logFile=/tmp/mgba.log sanguine_gba.gba
```

## Adding art and audio

All converters are Python 3 + Pillow scripts in `tools/`; each documents its usage in its
docstring.

- **Room backgrounds:** `python3 tools/convert_room_bg.py <room.png> assets/rooms` pads the
  180x140 painting onto a 256x256 canvas, reduces it to 256 colors (`--bpp4` for 16), and writes
  a BMP + JSON for Butano. Rename the output to the room's name.
- **Item icons:** `python3 tools/convert_item_icon.py <icon.png> assets/sprites/item_<name>.bmp`
  turns a 28x28 icon into a 32x32 sprite. Map it in `source/engine/item_info.cpp`.
- **Enemies / portraits:** `convert_enemy_sprite.py`, `convert_portrait.py`.
- **Animated flames:** `convert_flame_frames.py` (torch/candle frame strips).
- **Intro, puzzle, dice:** `convert_intro_tree_layer.py`, `convert_puzzle_tiles.py`,
  `make_die_sprite.py`.
- **Audio:** drop `.wav`/`.s3m` files in `assets/audio` (Maxmod backend). `wav_to_s3m_loop.py`
  wraps a raw PCM clip as a looping `.s3m` for use as music; `make_stat_sfx.py` synthesizes the
  stat gain/loss tones.

New rooms are a `source/rooms/<name>.{h,cpp}` pair registered in
`source/rooms/room_registry.cpp`.

Two GBA quirks this code works around (worth knowing before touching graphics code):

- **Palette index 0 is always transparent.** The room converter puts the border color there,
  and `main.cpp` sets the backdrop to the same color, so no real art color is lost.
- **There's only one 256-color background palette, and Butano reuses an already-loaded one
  without checking its colors.** Always release the old background before creating a new one
  (see `RoomScreen::_enter_room`), or the new room is drawn with the old room's colors.

## Project layout

```
source/
  main.cpp              Entry point
  engine/               Ported game logic, no UI: GameState, dice, combat, enemies, escape
                        endgame, inventory/item info, save slots (SRAM), settings, music, RNG,
                        room base class
  rooms/                One .h/.cpp pair per room (53), plus room_registry.cpp
  dialogue/             NPC dialogue trees (Countess, Ghostly Bride, Golem, Mad Gnome, Prisoner)
  ui/                   Panel, TextBox, ChoiceList, SidePanel (stats), ItemBar (icons), DiceView,
                        SlidingPuzzle, RoomScreen — the GBA-side "game.gd"
assets/
  rooms/                Room backgrounds and title/menu art (BMP + JSON)
  sprites/              Panel tiles, item icons, enemies, portraits, die, torches/candles
  audio/                Music (.s3m) and SFX (.wav)
web/index.html          EmulatorJS wrapper for the itch.io browser test build
third_party/butano/     Butano engine (git submodule)
tools/                  Asset converters (see above)
IMPLEMENTATION_PLAN.md  Design, roadmap and per-phase notes
```

## Toolchain note

devkitARM/Butano run under WSL2 Ubuntu rather than natively on Windows because the apt/pacman
packages make setup straightforward, and WSLg shows `mgba-qt`'s window on the desktop with no
extra configuration.
