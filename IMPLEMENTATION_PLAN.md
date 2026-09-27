# Sanguine — Game Boy Advance Port: Implementation Plan

## What this is

A from-scratch GBA implementation of **Sanguine** (`D:\Workspace\Godot\sanguine`, Godot 4.7,
gothic-horror room-by-room adventure with d20 skill checks) — not a recompile, and not a
continuation of **Castle Vojvod** (`D:\Workspace\Godot\sanguine_gb`, the GBC action-platformer
spinoff sharing this world's cast and opening beat). This plan keeps Sanguine's actual design
(room graph, text choices, dice-vs-attribute checks, sanity, inventory) essentially intact —
that structure already fits a handheld's button/menu interface well — and treats the port as
primarily an **asset-pipeline and content-compression problem**, not a redesign.

**Status (2026-09-26):** Phases 0–4 are complete and committed — all of Phase 4's sub-phases
(4a dialogue engine, 4b First Floor, 4c Outside, 4d Second Floor, 4e Third Floor + Countess,
4f escape endgame, 4g title screen) are implemented, boot-tested, and playtested (several
rounds of bug fixes/polish already folded back in). 4h (usable inventory), 4i (enemy overlay
scale/movement/ambient appearance), and 4j (ambient torch/candle flicker, added after 4h/4i on
the user's own follow-up request) are also now implemented and boot-tested, pending playtest.
Phase 5 (audio) is underway — 5a (settings/volume + pause menu) is done; 5b/5c (the actual
music/SFX content) haven't started. Phase 6 (polish + itch.io web build) is not started.

Grounded in a real look at the source project, not assumptions:

| Fact | Source | Why it matters here |
|---|---|---|
| Room art is 180×140 RGBA, 79 PNGs, 1.7MB total | `graphics/rooms/*.png` | Smaller than GBA's own 240×160 screen — reuse near-verbatim, no redraw |
| 59 rooms, 4191 lines of room logic | `rooms/*.gd` | One room ≈ 71 lines average; state-gated choices (`larder.gd`) port ~1:1 |
| 1460 text strings, ~36,000 words total | `localization/sanguine_en.po` | ≈24.7 words/string average — the problem is the long tail, not the count |
| Dice roll is 3D physics (`RigidBody3D` + glb model) | `dice/` | No GBA equivalent hardware — needs a 2D sprite-animation substitute |
| Room atmosphere uses layered animated overlays (torches, bats, candle flicker, pixel blink) | `graphics/rooms/*.tres`, `Torches_*.png`, `bat_flight_16x10px.png` | Maps directly onto GBA hardware sprites over a static BG — better suited to GBA than to redrawing as one flat image |
| Audio is 71MB of full-quality music/loops | `Sounds/`, `Sounds/Loops/` | Needs conversion to a compressed GBA-native format; cannot ship as-is |
| Attributes: INT/STR/DEX/HEALTH/SANITY/LUCK/HP/DC; d20-vs-DC checks; difficulty scales both player target and enemy damage | `singletons/constants_singleton.gd`, `gamestate_singleton.gd` | Pure data/logic — ports to C essentially unchanged |
| Enemy = `toHit`, `damage`, `dc`, `hp`, `is_dead()` | `enemies/enemy.gd` | Trivial C struct + a handful of functions |
| Event bus (`GlobalMessenger`) decouples rooms/enemies/dialogue from the UI via ~25 message types | `singletons/constants_singleton.gd` (`enum Message`) | The dispatch *pattern* is worth keeping (rooms don't know about rendering); the mechanism becomes a plain C function-pointer/switch dispatch, not a signal bus |
| `INTRO_START_COACH` / `INTRO_STOP_COACH` | `main.gd`, `scenes/intro.gd` | Confirms the coach-to-the-manor opening is shared canon with `sanguine_gb`'s own intro crawl — reuse the beat, not the text |

## Non-goals

- Not a continuation of `sanguine_gb`'s real-time action-platformer combat — the Countess,
  manor, and monsters are shared canon, but this project's combat stays the original's
  turn-based d20 check against a DC, not sprite-based melee/dodge.
- Not a 1:1 content port on day one. 59 rooms and 36K words is a full campaign; the roadmap
  below builds a complete, shippable *vertical slice* first (a handful of rooms, one full
  floor's loop) and treats "port every remaining room" as an explicit, separately-scoped
  later phase — see [Open decisions](#open-decisions--risks).
- Not reusing GBDK-2020 or anything from `sanguine_gb`'s toolchain — GBDK targets GB/GBC
  only. See [Toolchain](#toolchain) below.

## Toolchain

**devkitARM + libgba**, with **Butano** (a modern C++17 engine layer over libgba/libtonc)
as the recommended default rather than bare-metal libgba calls throughout.

Why Butano specifically, given this project's shape:
- It has first-class support for exactly the two things this port leans on hardest: regular
  (non-affine) tiled backgrounds with per-tile 8bpp/4bpp color, and sprite-based animated
  overlays composited over a background — i.e. "one big background image plus a handful of
  animated sprites on top" (torches, bats, candle flicker), which is precisely the existing
  room-art structure.
- Built-in save-file (SRAM) helpers, a sprite/tile/palette build step driven by ordinary PNGs
  (feeds the asset pipeline in [Assets](#asset-pipeline) directly, no hand-authored tile
  grids the way `sanguine_gb`'s ASCII-pixel-art pipeline needed), and audio via a tracker
  format (see [Audio](#audio)).
- C++ (not C) — a genuine win here specifically: Sanguine's own source is organized as
  small per-room/per-enemy/per-dialogue classes (`extends Room`, `extends Enemy`,
  `extends Dialogue`) with shared base-class behavior. That maps far more directly onto
  C++ classes/virtual dispatch than it would onto GBDK's C-only, manually-banked-per-file
  style — porting room *logic* (not art) is much closer to a mechanical translation than a
  redesign if the target language already has classes.
- Fallback: if Butano's binary-size/feature overhead ever becomes a real constraint, drop to
  raw libgba/libtonc for the specific subsystem in question — it's a strict subset, not a
  rewrite-everything decision.

## Asset pipeline

### Backgrounds (room art)
1. Batch-convert `graphics/rooms/*.png` from RGBA truecolor to indexed color via a Python
   preprocessing script (Pillow), one palette per room (or a shared palette family across
   rooms that already read as tonally similar — check before assuming one-palette-per-room).
   GBA background modes give up to 256 colors per screen block in 8bpp tiled mode; most of
   these paintings will compress well below that after quantization, since dice.pixel-art
   research the original PC project isn't full photographic gradient work.
2. Since 180×140 is already smaller than GBA's 240×160, decide once (not per-room, to avoid
   inconsistent framing) whether to center-pad with a themed border/vignette or scale up —
   recommend padding: it preserves the original composition exactly rather than reintroducing
   the kind of resize/crop tradeoffs `sanguine_gb`'s title screen needed to fight with.
3. Butano's graphics build step converts the resulting PNGs into tile/map/palette data at
   build time — no hand-written tile-array generator script needed here, unlike
   `sanguine_gb`'s `tools/*_assets.py`.
4. Budget check: 79 room-related PNGs at ~1.7MB raw RGBA on disk will compress to
   *well* under 1MB as GBA tile data after palette reduction and tile deduplication (flat
   sky/wall/floor regions dedupe the same way `sanguine_gb`'s backgrounds did) — comfortable
   within even a 4MB cart target with headroom for everything else.

### Animated overlays (torches, bats, candle flicker, pixel blink)
- These already exist as separate small assets (`bat_flight_16x10px.png`,
  `candle_flame_6x9px.png`, `Torches_*.png`, `.tres` `SpriteFrames`-style resources) —
  meaning the original game *already* treats them as sprite-like overlays, not baked into
  the background painting. Port them as literal GBA sprites (OBJs) positioned per-room, using
  Butano's sprite animation support directly. This is less work than it sounds: the
  separation already exists in the source project.

### Dice roll
- Replace the 3D `RigidBody3D` d20 with a short (8-16 frame) 2D sprite animation: a tumbling
  die that settles on a face, then a brief hold on the result before dispatching
  `Result.SUCCESS/FAIL/CANCEL/CRITICAL_FAIL` exactly as `GlobalMessenger.show_dice(...)`
  does today. The *logic* (`RollNeeded()`, attribute bonus, difficulty modifier) ports
  unchanged from `gamestate_singleton.gd`; only the presentation layer changes.

### Audio
- 71MB of source audio cannot ship on a GBA cart. Two realistic paths, to be decided once a
  cart-size budget is set (see [Open decisions](#open-decisions--risks)):
  1. **Tracker music** (MOD/S3M via Butano's audio backend) for ambient loops — recompose or
     re-arrange the existing `Sounds/Loops/` cues rather than converting them, since
     lossy-sampled full mixes rarely survive GBA's audio hardware well.
  2. **Short compressed one-shot SFX** (hit, roll, pickup, door) via DMA sample playback for
     the handful of sounds where a real recording matters more than a chiptune substitute.
- This is very likely the single largest content-creation cost in the whole port and should
  be scoped and estimated on its own, not folded silently into "polish."

## Engine architecture

Directory layout (mirrors Sanguine's own `rooms/` / `enemies/` / `dialogue/` / `singletons/`
split, since that separation already works and translating it 1:1 keeps the port's structure
recognizable against the source project):

```
source/
  main.cpp                 Entry point, boot sequence, top-level game-state switch
  engine/
    game_state.{h,cpp}      Ported GameState singleton: stats, inventory, save/load, difficulty
    constants.h              Ported enums: Attribute, Result, Message, Level, Difficulty
    event_bus.{h,cpp}        Ported GlobalMessenger pattern as a typed dispatch table
    dice.{h,cpp}             2D dice-roll presentation + RollNeeded()/resolve logic
    text_box.{h,cpp}         Paginated text renderer (see Text rendering below)
    save.{h,cpp}             SRAM save/load, 3 slots (matches MAX_SAVE_SLOTS)
  rooms/
    room.{h,cpp}             Base class: get_description(), get_choices(), get_state()/set_state()
    <room_name>.{h,cpp}       One pair per room, same granularity as the .gd originals
  enemies/
    enemy.{h,cpp}             Base class: toHit, damage, dc, hp, is_dead(), take_player_damage()
    <enemy_name>.{h,cpp}
  dialogue/
    dialogue.{h,cpp}          Base class: add_dialogue()/add_dialogue_first()/add_quit(), Type enum
    <npc_name>.{h,cpp}
  ui/
    room_screen.{h,cpp}       Background + choice-list + text-box compositor (the "game.gd" equivalent)
    map_screen.{h,cpp}        Floor map / navigation overview
    intro_screen.{h,cpp}      Coach-ride opening (shared beat with sanguine_gb's intro.c)
assets/
  rooms/                     Source PNGs staged for the Butano graphics build (see pipeline above)
  sprites/                   Torch/bat/candle/dice/portrait sprite sheets
  audio/                     Tracker modules + compressed SFX
  text/                      Curated, length-budgeted string tables (see Content triage)
```

### Text rendering
GBA has no console/font system to lean on the way GBDK does — this needs a real (if small)
bitmap-font text renderer from the start: word-wrap to a fixed box width, pagination on
overflow (A to advance, matching the "press a button to continue" idiom this genre already
uses), and a simple choice-list cursor (D-pad up/down, A to confirm, B to back where a
`get_choices()` menu has a "back"/cancel option). This is new code, not a port of anything —
Sanguine's PC version relies on Godot's `RichTextLabel` and mouse/scroll, which has no GBA
analog.

### Event bus
`GlobalMessenger` in the original is a Godot `Node` singleton using signals. On GBA this
becomes a plain typed dispatch (a `switch` over the same `Message` enum, or a small array of
function pointers keyed by message type) — same decoupling *intent* (rooms/enemies/dialogue
never reach into the UI directly), deliberately not the same *mechanism* (no signal/Callable
allocation overhead on a device this constrained).

## Content triage

36,000 words across 1460 strings, average 24.7 words/string, is not itself an unreasonable
number of *beats* for an RPG-scale GBA cart (compare Fire Emblem/Golden Sun's own dialogue
volume) — the problem is specifically the long tail: the "long form on first visit" room
descriptions and the deeper dialogue trees (`dialogue/mad_gnome.gd` alone is 196 lines).

Process:
1. **Extract every string with its current word count** (a one-off Python script over
   `localization/sanguine_en.po`, mirroring the bulk-editing scripts already in Sanguine's
   own `tools/`) and sort descending.
2. **Set a hard per-string cap** — recommend 40 words (≈2 GBA text-box pages at a
   comfortable per-page line count) for anything shown by default, with a small, explicitly
   tagged allowlist for the handful of true set-piece story beats (arrival, the Countess's
   reveal, the ending) allowed to run longer across multiple advance-presses.
3. **Rewrite over the cap, don't auto-truncate** — this is an editorial pass, done by
   whoever owns Sanguine's prose, using the *short*-form revisit text already written for
   many rooms as the template for voice/length even on first visit.
4. **Prune dialogue trees to load-bearing branches**, moving flavor-only lines to the
   short-bark convention `sanguine_gb`'s `textbox.c` already validated for this exact
   problem (one punchy line, no reply options, auto-advance) rather than full back-and-forth.
5. Re-run the word-count script after each pass and track total-words-remaining as a single
   number over time — gives a concrete, visible target instead of "trim it down" as an
   open-ended ask.

## Roadmap

**Phase 0 — Toolchain bring-up.** devkitARM + Butano installed and building; a blank ROM
boots in mGBA/an emulator and in the Browser-pane preview if feasible; a single test room's
art round-trips through the Butano graphics pipeline end to end.

**Phase 1 — Vertical slice.** One full floor's worth of rooms (recommend Basement or First
Floor — smallest `Level` per `constants_singleton.gd`'s enum), the room-graph engine,
text-box renderer with pagination, choice-list navigation, and the 2D dice roll with one
working skill check. No combat yet, no save/load yet — proves the core loop end to end.

**Phase 2 — Combat.** Enemy base class, `attack_player()` equivalent, HP/death, at least one
real fight ported (recommend one of the simpler early enemies, e.g. `zombie` given its short
dialogue file, as the first combat vertical slice).

**Phase 3 — Save/load + inventory.** SRAM save (3 slots, matching `MAX_SAVE_SLOTS`),
inventory add/remove/gate-check (`add_locked_path` equivalent), achievements deferred unless
trivial to include here.

**Phase 4 — Content port + trim pass.** Remaining rooms/dialogue/enemies, run through the
content-triage process above as each batch is ported (not as a separate pass afterward —
catches over-length text while the room is already open in an editor). Too large for one
sitting (49 more rooms across 4 areas, 9 dialogue trees, 12 more enemies), so split by
area — each sub-phase is a self-contained batch of the same size as the Basement floor
already shipped in Phase 1, portable/testable/committable on its own:

- **4a — Dialogue engine + Prisoner.** Before touching more rooms: the dialogue system itself
  (branching NPC conversation, `add_dialogue`/`add_dialogue_first`/`add_quit` equivalents)
  doesn't exist yet, and every later sub-phase needs it. Wiring up Prisoner (already stubbed
  in Dungeon) proves it end to end without also needing a new floor.
- **4b — First Floor** (13 rooms: GreatHall, Courtyard, Kitchen, DinnerHall, HallOfArmors,
  MusicRoom, Chapel, Storage, KitchenCellar, WineCellar, Orangery, Larder,
  Stairway1stFloor). Connects to the Basement via the Stairway1stFloor/StairwayBasement
  link both floors already stub out.
- **4c — Outside** (10 rooms: ForestCrossing, GroundskeeperCottage, Garden, OvergrownPond,
  MainGate, KitchenEntrance, Cemetery, Mausoleum, Tombstones, CliffTombs). The game's actual
  chronological start (coach-ride intro leads here); connects to First Floor via MainGate.
- **4d — Second Floor** (12 rooms: FancyCorridor01, BedChamber01, GreatChamber, Library,
  HallOfPortraits, AudienceHall, Treasury, Bathroom, ChildrensRoom, Stairway2ndFloor,
  TrophyRoom, GuestBedroom).
- **4e — Third Floor** (9 rooms: FancyCorridor02, Reliquary, Observatory, Skybridge,
  ToymakersWorkshop, DrawingRoom, ScientificLaboratory, plus whichever of the ending/
  documents rooms belong here) and the remaining enemies/dialogue tied to it (the Countess).
- **4f — Escape endgame.** `is_escaping()` and the living-pursuers chase — its own system,
  and only meaningful once the full map from 4a–4e exists to chase the player across.
- **4g — Title screen + coach-ride intro.** Phase 3 deliberately shipped with no title/slot
  screen (single-slot autosave straight into the last room, or a fresh game at Cellar Hall —
  see that phase's design-decision note); a real main menu (New Game / Continue, and
  eventually multi-slot once there's enough game to warrant picking a slot) belongs once
  there's a proper first room to send a "New Game" into. That's the coach-ride opening
  (`scenes/intro.gd`'s `INTRO_START_COACH`/`INTRO_STOP_COACH`, already flagged as shared canon
  with `sanguine_gb`'s own intro in the facts table above) landing in ForestCrossing — so this
  sub-phase depends on 4c's Outside rooms existing, and should follow it rather than come
  first.
- **4h — Usable inventory. DONE.** Implemented as `RoomScreen::_open_inventory()`, reached via
  a global "Inv" entry `run()` appends to every room's choice list. Item name/description
  table lives in the new `engine/item_info.h/.cpp` (`item_info`, `item_is_usable`, `use_item`,
  and the shared `item_icon()` lookup — `ItemBar` and the browser both call it now instead of
  each keeping their own copy). Browsing pages through owned items
  `ui::ChoiceList::max_options - 2` at a time (a "More" entry appears when there's another
  page), shows each one's description via the normal `show_text`, and offers "Use" when
  `item_is_usable()` allows it — `Combat::fight`'s own mid-fight item use now calls the same
  shared `use_item()` instead of duplicating the potion effects inline. Also bumped
  `ui::ChoiceList::max_options` from 8 to 10 while touching this: `FancyCorridor01` can already
  emit 9 choices (Look + 7 directions + "Talk to the Countess") before the new global "Inv"
  entry pushed the worst case to 10 — a real latent overflow bug, fixed as part of this same
  change rather than left for the next room that happens to trip it. The item sidebar's
  own 4-icon limit (flagged back in Phase 1) is untouched — no floor has exceeded it yet.
- **4i — Enemy overlay scale + movement. DONE** (with one deliberate cut — see below).
  Flagged by the user as future polish once real
  combat encounters (4b's rat/bats) surfaced the gap: `utils/sprite_animations.gd` gives each
  small critter both a per-instance scale (the rat renders at `Vector2(2, 2)`, i.e. 2x, since
  its source art is tiny — `enemy_sprite_item()`/`RoomScreen::show_enemy` currently always
  draws at native size) and a movement pattern — the rat dashes between floor waypoints
  (`rat_floor`), while the raven/hungry-bats loop along a figure-eight (Lissajous) path
  (`mosquito`/`raven_anim`), the raven also cycling a 4-frame flight animation from a 41x41
  sprite sheet. Porting the scale needs Butano affine sprites (regular sprites can't scale);
  porting the movement needs a per-frame position update hook during `Combat::fight`'s
  blocking loop (currently static — `show_enemy` positions once and never moves it), most
  naturally as a small per-enemy "animator" driven each `bn::core::update()` tick rather than
  a one-shot tween, since Butano has no tween system of its own. Also flagged by the user: in
  the source project, some of these enemies are visible as *ambient room content*
  (`Room.get_extra_content()`) before the fight even starts — e.g. the raven already
  circling in ForestCrossing, or the rat already scurrying in KitchenCellar once
  `_rat_active()` is true — not summoned only at the moment `Combat::fight` begins the way
  every enemy overlay currently works (`RoomScreen::show_enemy` is only ever called from
  inside `Combat::fight`). Those rooms already mention the creature in their description
  text, but don't show its sprite until combat starts; porting this means also showing (a
  non-blocking version of) the enemy overlay from within the *room's own* `describe()`/enter
  flow, before any fight choice is picked, wherever the source room's `get_extra_content()`
  did the same.

  **Implemented as:** `EnemyId`/`EnemyAnim` gained `enemy_anim_kind()` (`engine/enemy.h/.cpp`)
  — `RAT_FLOOR` for the Rat, `FIGURE_EIGHT` for the (forest) Raven and Hungry Bats, `NONE` for
  everything else (Zombie/Golem/Countess/MadGnome/NightRaven/GhostlyBride's portrait all stay
  static in the source too). `RoomScreen::show_enemy(EnemyId)` now sets this up and
  `RoomScreen::_update_enemy_animation()` advances it one tick every time `show_text`/`choose`
  already call `bn::core::update()` — no separate tween system needed, and it runs during
  ambient display exactly the same as during `Combat::fight` since both now go through the one
  `show_enemy()` entry point (ForestCrossing/KitchenCellar/OutsideGates' ambient overlays were
  switched from calling `show_overlay_sprite()` directly to `show_enemy()`, so ambient content
  animates too, e.g. the escape guard if it happens to roll Rat/Raven/HungryBats). The rat gets
  `set_scale(2)` (Butano auto-attaches the affine mat this needs) and dashes between 6 waypoints
  with a pause and a horizontal flip at each turn, rescaled from the source's 180px-wide canvas
  coordinates onto this build's narrower safe travel range; the raven/bats loop a figure-eight
  via `bn::degrees_sin` at two different phases (`sin(θ)`/`sin(2θ)`), matching the source's
  Lissajous path shape. **Deliberately cut:** the raven's 4-frame flight-animation cycling —
  the source pulls its frames from a 41x41 sprite sheet, but this port's `enemy_raven` asset was
  already converted as a single static 64x64 image with no sheet to slice; adding one needs new
  source art conversion work, not just wiring, so it's left as a static sprite that only moves,
  not flaps. Revisit if/when raven art gets a real multi-frame sheet.

Enemies and dialogue for a given area are ported alongside that area's own sub-phase (as
DungeonCreatures/Homunculus were with the Basement in Phases 1–2), not as a separate pass.

- **4j — Ambient torch/candle flicker. DONE.** User asked, after seeing 4i's animated enemy
  overlays, whether the source project's animated torch/candle overlays (`Room.
  corridor_torches()`/`doorway_torches()`/`candles()`, `graphics/rooms/Torches_*.tres` +
  `Candle_Frames.tres`) could be reused too. They could, almost directly: both source sheets
  are already frame-strips (`torch_front_10x38px.png` 8 frames of 10x38, `candle_flame_6x9px.png`
  6 frames of 6x9, both looping at the SpriteFrames "speed 5.0" — 5 frames/second), just laid
  out horizontally where this port's multi-frame sprite convention (see `assets/sprites/die.bmp`
  + its `"height"` JSON key, first used for the dice) needs them vertical. New
  `tools/convert_flame_frames.py` slices a horizontal N-frame sheet, pads each frame into the
  smallest valid GBA sprite shape that contains it (GBA sprites only support a fixed list of
  (width, height) pairs — 10x38 isn't one, so the torch frame is padded into 32x64; 6x9 fits
  into 8x16 for the candle), and quantizes all frames together into one shared palette, output
  as `assets/sprites/ambient_torch.bmp`/`ambient_candle.bmp`. `Room` gained a new
  `AmbientDecor`/`AmbientKind` (`engine/room.h`) and a `build_ambient()` virtual (default
  no-op, like `on_enter`) — a fixed list of torch/candle positions built once per room entry
  (`RoomScreen::_refresh_ambient`), since unlike 4i's enemies these never move or change with
  room state. `RoomScreen::_update_ambient()` advances each one's frame every 12 ticks (60 FPS
  / 5 FPS) from the same `show_text`/`choose` update hooks 4i already added. Positions were
  read directly out of each source room's `.gd` (`get_extra_content()`) and converted from the
  source's 180x140 room-canvas coordinates into this engine's centered sprite coordinates via
  `(source_x - 90, source_y - 80)` — derived from `convert_room_bg.py`'s centering math
  (180x140 art padded onto a 256x256 canvas at offset (38,58), displayed via
  `create_bg(0, -10)`). Wired into 20 rooms this way: `corridor_torches()` (CellarHall,
  KitchenCellar, StoneCorridor), `doorway_torches()` (HallOfArmors, Stairway2ndFloor,
  StairwayBasement), a single `doorway_torch()` (Dungeon), and `candles()` (AlchemicalLaboratory,
  AudienceHall, Chapel, DinnerHall, DrawingRoom, GreatChamber, GreatHall, GuestBedroom, Library,
  MusicRoom, Reliquary, Stairway1stFloor, HomunculusWorkshop), plus OutsideGates (see below).
  **Deliberately skipped**: ToymakersWorkshop (its candle is baked directly into the static room
  art, no separate overlay in the source either). DrawingRoom's other two ambient effects there
  (`blinking_stars`/`blinking_eyes`, state-dependent) are a different, not-yet-ported feature —
  only its three unconditional candles were added.

  **OutsideGates follow-up** (user noticed the torches were missing there, asked to work out
  the placement): initially skipped because the source's `outside_gates.gd` positions its two
  torches at Vector2(50,219)/Vector2(130,219) in a 180x294 "tall" canvas (`outside_gates_tall_
  torches.png`, used for the source's scrolling look-at-the-castle shot) — outside the normal
  180x140 room-art coordinate space every other room's ambient positions are given in.
  Resolved by diffing this port's converted `assets/rooms/outside_gates.bmp` against the
  source PNGs (`Image.crop` + a mean absolute pixel-difference check): it's a near-exact match
  (~1.1 average channel difference, vs. ~19-34 for the other candidates) for the tall image's
  *bottom* 140 rows, i.e. rows [154, 294) — confirming the scroll effect itself isn't ported,
  just its final resting frame. That fixes the crop offset: source row 219 -> 219-154=65 in
  this room's own art coordinates, then the usual `(x-90, y-80)` conversion applies from
  there — `OutsideGates::build_ambient()` places both torches at screen (-40,-15)/(40,-15).

**Phase 5 — Audio.** Tracker music + compressed SFX, scoped and estimated as its own
milestone per the [Audio](#audio) note above.

- **5a — Settings (volume/mute) + pause menu. DONE**, started ahead of the music/SFX content
  itself so the infrastructure exists before there's anything to turn down. Analysis of the
  source project's actual audio usage (not just its `Sounds/` folder, which is 71MB/49 files
  but mostly either orphaned `Loops/*.mp3` cues never wired to any `get_music()` override, or
  a handful of short SFX) found only ~8 real music tracks in play: the title/intro theme
  (reused for DinnerHall), one generic default loop covering the large majority of unstyled
  rooms, 5 room-specific tracks (GreatHall/AudienceHall/StoneCorridor/MusicRoom's organ/
  ChildrensRoom's story-beat swap), and the Countess's boss theme (reused across her dinner
  scenes too) — recommended as the actual scope for 5b/5c rather than the full 71MB, in that
  priority order (title+default loop first, Countess second, GreatHall third, the rest as time
  allows), once composed as short loopable tracker modules (Butano's `bn::music` plays `.mod`/
  `.s3m`/`.it`/`.xm` via Maxmod — `mmutil` is already in the toolchain — not the source's MP3/
  OGG streams directly, which have no GBA-side decoder).
  New `engine/settings.{h,cpp}`: `Settings` persists music/SFX volume (0..4 steps, 25% each) to
  its own fixed SRAM block, positioned right after the 3 save slots' reserved span
  (`SaveGame::slot_count * SaveGame::slot_size` — `SaveGame` gained an explicit `slot_size`
  constant so this offset never depends on `SaveData`'s actual internal layout). `apply()`
  pushes both to `bn::music::set_volume()`/`bn::sound::set_master_volume()` — the SFX one is a
  persistent global multiplier, so it already covers the existing `stat_gain`/`stat_loss` cues
  and any future ones with no per-call-site changes; the music one only affects whatever's
  *currently* playing, so once Phase 5b's tracks exist, every `bn::music::play()` call site
  will need to pass the current volume itself too, not rely on this alone.
  `RoomScreen::show_settings_menu()` is a new Music Volume/SFX Volume/Back menu (D-pad
  left/right adjusts the selected row, applying live), reachable from a third "Settings" row
  added to the title screen's New Game/Continue menu (`choose_new_or_continue()` now returns a
  stable id — 0/1/2 — regardless of whether "Continue" happens to be present, so main.cpp can
  always recognize "Settings" by id) and from a new in-game pause menu.
  The pause menu (`RoomScreen::_show_pause_menu()`: Resume/Settings/Main Menu) is reachable via
  Start from inside `RoomScreen::choose()` — the one blocking choice loop shared by room
  exploration, `Combat::fight`, and `Dialogue::talk`, so pausing works uniformly everywhere
  without touching those call sites individually. "Main Menu" calls `bn::core::reset()`
  ([[noreturn]]) rather than trying to unwind back out of however deep the call stack
  currently is — SRAM (save slots, settings) already reflects the last explicit save, so a
  soft reset back to the title screen loses only the in-progress moment, exactly matching what
  "return to main menu" should do.
- **5b (partial) — Title theme. DONE**, first of the priority-ordered tracks above. The source's
  `Respect for bones 3.ogg` is a streamed MP3 the GBA can't decode, so it's re-purposed rather
  than converted: trimmed to its first 40 seconds and downsampled to mono 8-bit unsigned PCM
  (10512Hz — ffmpeg: `-t 40 -ac 1 -ar 10512 -f u8`), then wrapped as a minimal Scream Tracker 3
  module (`tools/wav_to_s3m_loop.py`) that triggers the clip once as a single sample (with its
  own loop points covering nearly all of it) and never retriggers it again for ~15 minutes of
  silent filler patterns — long enough that a real play session won't hit the retrigger seam.
  This is the standard GBA-homebrew trick for reusing pre-recorded audio as `bn::music` without
  actual composition; the exact S3M byte layout was derived by parsing (not guessing) the
  already-vendored `third_party/butano/examples/core/audio/soda7_xcopy_ohc.s3m`, and validated
  by running `mmutil` on the output directly before trusting it in a real build. Wired into
  `RoomScreen::show_title_card()` (`bn::music::play(bn::music_items::title_theme, Settings::
  instance().music_volume())`, loops by default) and stopped in `main.cpp` right before
  `screen.run()` begins real gameplay, matching the source's own intro_music lifetime (title
  screen through the coach-ride intro, no in-game successor yet). Adds ~430KB to the ROM
  (2.22MB total) — trimming duration and/or sample rate further directly and linearly reduces
  this if cart budget gets tight; 40s/10512Hz was a first attempt, not a settled number.
  **Not verified by ear** — only that `mmutil` parses it and mgba boots without asserting;
  actual pitch/loop-seam quality (the 40s cut point isn't aligned to any musical
  beat/zero-crossing) needs a real playtest.

**Phase 6 — Polish and cart budget close-out.** Final ROM size check against the chosen
target (see below), difficulty-curve pass, credits/ending states (`GAME_WON`/`GAME_OVER`
equivalents), and an **itch.io web build**: devkitARM/Butano only ever produce a real `.gba`
ROM (there's no wasm/web export in this toolchain), so "playable on itch" means wrapping that
ROM with a browser-based GBA emulator — [EmulatorJS](https://emulatorjs.org/) is the standard
choice (it embeds via a small HTML page + its own JS/wasm core, no server needed) — and
zipping the page + ROM as an itch HTML5 upload. This is packaging, not a build-system change,
so it can be done once near the end rather than kept in sync with every earlier phase.

## Open decisions / risks

- **Cart size target** — a real number (4MB/8MB/16MB/32MB) needs picking before Phase 5's
  audio budget can be finalized; it drives how much of the tracker-vs-sample tradeoff is
  even available.
- **Scope of Phase 4** — is the goal all 59 rooms, or a curated subset that tells a complete
  (if shorter) version of the story? Worth deciding explicitly rather than discovering the
  answer by running out of time partway through.
- **Localization** — the PC version supports Swedish/Danish/Norwegian/Spanish (partially, per
  its own README). Decide up front whether GBA v1 ships English-only (recommended, given the
  ROM-space and text-rendering cost multiplies per language) or reserves budget for it later.
- **The "escape" endgame** (`is_escaping()`, living pursuers chasing the player out) is a
  distinct late-game mode with its own state — flag it explicitly in Phase 4's scope rather
  than assuming it falls out of the normal room-porting process for free.
- **Relationship to `sanguine_gb`** — no code or asset dependency is assumed between this
  project and the GBC game; they share canon (the coach-ride opening, the manor, the
  Countess) but not a codebase. If shared lore text ever needs to stay consistent between the
  two, that's a content-review step, not an engineering dependency.
