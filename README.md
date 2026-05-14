# BrotatoClone

[English](README.md) | [繁體中文](README.zh-TW.md)

A Brotato-inspired survivor game implemented with C++17 on top of the PTSD
framework.

This project started from the PTSD template and extends it into a playable
20-wave run with progression systems, shop flow, boss fight, and UI overlays.

## Framework and Tech Stack

- Language: C++17
- Build system: CMake (minimum 3.16)
- Framework: PTSD (Practical Tools for Simple Design)
- Rendering/Input/Audio stack: provided through PTSD dependencies

PTSD reference:
https://github.com/ntut-open-source-club/practical-tools-for-simple-design

PTSD template base used for this project:
https://github.com/ntut-open-source-club/ptsd-template

## Current Gameplay Scope

- Title screen and full game loop
- 20-wave run structure
- 30-second normal waves
- Final boss on wave 20 (with phase transition)
- Auto-fire combat with 4 weapons:
   - Pistol
   - Shotgun
   - SMG
   - Knife
- Weapon tier and merging system:
   - Two identical Tier I weapons merge into Tier II
   - Two identical Tier II weapons merge into Tier III
   - Higher tiers increase damage and reduce cooldown
- Enemy roster progression:
   - Chaser
   - Bruiser
   - Spitter
   - Guardian
   - Bomber
   - Boss
- Wave-end progression:
   - Stat selection (1 or 2 picks depending on performance)
   - Shop phase with reroll, purchases, and weapon selling
- Economy and sustain systems:
   - Material orbs (gold drops)
   - Pickup range and wave-end material auto-collection
   - Tree destruction and fruit healing
   - Lifesteal, regen, armor, dodge, crit, and other scaling stats
- Random arena themes:
   - Dirt
   - Forest
   - Volcano
   - Dreamy lands
   - Boneyard
   - Darklands
- UI and feedback:
   - HUD (HP, gold, wave, timer)
   - Boss HP bar
   - Pause menu with full stat snapshot and owned item list
   - Damage numbers, hit/death effects, muzzle flashes
   - SFX for shooting, hits, purchases, rerolls, and wave transitions

## Gameplay Systems

### Combat

The player moves manually while weapons fire automatically at enemies or trees
within range. Weapon targeting prioritizes enemies by default. Holding T forces
weapons to prioritize trees when a tree is in range.

Enemies drop material orbs when killed. These orbs grant both gold and XP when
picked up. Orbs magnetize toward the player inside pickup range, and any
remaining orbs are automatically pulled in at the end of each wave before the
stat/shop phase begins.

### Weapons and Merging

Weapons have three tiers:

- Tier I: base weapon
- Tier II: made by merging two identical Tier I weapons
- Tier III: made by merging two identical Tier II weapons

Buying a weapon automatically attempts to merge it with matching owned weapons.
If all six weapon slots are full, the player can still buy a weapon when that
weapon can immediately merge with an existing same-type, same-tier weapon.

Tier upgrades currently improve the weapon directly:

- Damage increases by about 50% per tier
- Cooldown becomes about 20% faster per tier

Weapon tiers are color-coded in the shop/weapon list:

- Tier I: white
- Tier II: green
- Tier III: blue

### Shop and Progression

After each normal wave, the player receives stat upgrade choices. Strong wave
performance can grant two stat picks instead of one.

The shop then opens, allowing the player to:

- Buy item cards
- Buy weapon cards
- Reroll shop choices
- Sell owned weapons
- Start the next wave

There is currently no save/load system. Each run is a standalone session.

## Wave Design

The run uses 20 waves. Normal waves last 30 seconds, and wave 20 is the final
boss fight.

Enemy spawning is controlled by wave profiles rather than a flat random mix.
Each profile defines total enemies, live enemy cap, spawn speed, batch size, and
enemy type weights.

Current wave pacing:

- Waves 1-3: early swarm
   - Mostly Chasers
   - Low HP enemy flood
   - Small amount of Bruisers
- Waves 4-6: first mixed pressure
   - Chasers and Bruisers continue
   - Spitters start appearing
   - Player begins needing more movement awareness
- Waves 7-9: ranged pressure
   - Spitter weight increases
   - Player is pushed to dodge projectiles while clearing melee enemies
- Waves 10-12: durable and explosive enemies
   - Guardians and Bombers are introduced
   - Enemy mix starts testing damage output and positioning
- Waves 13-16: full mixed waves
   - Chasers, Bruisers, Spitters, Guardians, and Bombers all appear
   - Spawn rate and live enemy cap are higher
- Waves 17-19: boss preparation pressure test
   - Faster spawns
   - Heavier Guardian/Bomber presence
   - Designed to test whether the build can survive the boss transition
- Wave 20: boss
   - One boss enemy
   - Boss has ranged multi-shot attacks and a second phase at low HP

## Map System

The arena background is randomly selected at the start of each wave. The system
uses Brotato-style single-tone arena themes with sparse ground decorations
rather than a full map background image.

Each arena theme contains:

- A large solid-color background image
- Subtle shading/noise
- A small number of non-overlapping ground decoration sprites

Decorations are visual only and do not block movement.

## Controls

### Title
- Enter or Space: start run

### In Wave
- W/A/S/D or Arrow Keys: move
- Hold T: prioritize tree targeting
- Esc: open pause menu

### Stat Selection
- 1/2/3: pick upgrade card
- Left Click: pick upgrade card

### Shop
- 1/2/3/4: buy item card
- Left Click: buy item card
- 0 or Enter: go to next wave
- Left Click on REROLL: reroll shop items
- Left Click on owned weapon slot: sell that weapon

### Pause
- Esc: resume
- Left Click RESUME: resume
- Left Click QUIT: quit immediately

### Result Overlay
- R: restart run
- Esc: quit

## Build and Run

Important: this project currently supports Debug builds only.

Release currently fails by design because relative resource path handling is
still work in progress in CMake.

### Prerequisites

- Git
- CMake 3.16+
- C++ compiler (MSVC, Clang, or GCC)
- OpenGL-capable environment
- Optional: Ninja

### Clone

```bash
git clone <YOUR_REPO_URL> --recursive
cd BrotatoClone
```

### Configure (recommended)

Without Ninja:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
```

With Ninja:

```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
```

### Build

```bash
cmake --build build --config Debug -j
```

### Run

From repository root (PowerShell):

```powershell
.\build\BrotatoClone.exe
```

Or run from the build directory:

```powershell
cd build
.\BrotatoClone.exe
```

## Windows Batch Scripts

This repository includes:

- configure.bat
- build.bat

These scripts are currently machine-specific and point to CLion-bundled CMake
and Ninja paths on drive D:. Update those paths before using the scripts on a
different machine.

## Project Structure

- src
   - Core gameplay implementation
   - Scene flow, entities, weapons, UI systems
- include
   - Public headers for gameplay systems
- Resources
   - Runtime assets (fonts/images/sounds)
- PTSD
   - Framework source fetched by CMake FetchContent
- CMakeLists.txt
   - Build entry and dependency integration

## Gameplay Flow

1. Title screen
2. Wave combat
3. Stat selection
4. Shop
5. Next wave
6. Wave 20 boss
7. Victory or game over

## Known Limitations

- Debug build only (Release path unresolved)
- No save/load persistence
- No automated tests are wired in this repository yet
- Batch scripts are not portable without local path edits

## Troubleshooting

### Game starts then exits with text creation errors

If logs contain messages similar to:

- Failed to create text
- Text has zero width

check the following:

- Build type is Debug
- Resources directory is present and unchanged
- Resources/fonts/Inter.ttf exists
- Build was reconfigured after any path changes

### CMake configure fails on first setup

The first configure downloads PTSD and third-party dependencies through
FetchContent. Ensure network access is available and rerun configure.

## Credits

- PTSD framework by NTUT Open-Source Club
- Brotato-inspired game design and pacing references

## License

MIT. See LICENSE for details.
