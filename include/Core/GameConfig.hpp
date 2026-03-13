#pragma once

namespace GameConfig {
// Window
constexpr int WINDOW_WIDTH = 1280;
constexpr int WINDOW_HEIGHT = 720;

// Game area (arena)
constexpr float ARENA_HALF_W = 580.f;
constexpr float ARENA_HALF_H = 320.f;

// Player
constexpr float PLAYER_SPEED = 220.f;
constexpr int PLAYER_MAX_HP = 20;

// Waves
constexpr int   TOTAL_WAVES    = 20;
constexpr int   BOSS_WAVE      = 20;  // final wave spawns the boss
constexpr float WAVE_DURATION  = 30.f; // seconds per normal wave

// Shop
constexpr int SHOP_ITEMS = 4;
constexpr int START_GOLD = 0;
constexpr int GOLD_PER_KILL = 2;
constexpr int GOLD_PER_WAVE = 10;
} // namespace GameConfig
