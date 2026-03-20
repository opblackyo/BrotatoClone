# BrotatoClone

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
   - Tree destruction and fruit healing
   - Lifesteal, regen, armor, dodge, crit, and other scaling stats
- UI and feedback:
   - HUD (HP, gold, wave, timer)
   - Boss HP bar
   - Pause menu with full stat snapshot and owned item list
   - Damage numbers, hit/death effects, muzzle flashes
   - SFX for shooting, hits, purchases, rerolls, and wave transitions

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
