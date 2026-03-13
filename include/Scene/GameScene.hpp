#pragma once
#include "pch.hpp"
#include "Entity/Player.hpp"
#include "Entity/Enemy.hpp"
#include "Weapon/Projectile.hpp"
#include "UI/HUD.hpp"
#include "UI/ShopScene.hpp"
#include "UI/StatSelectScene.hpp"
#include "UI/PauseMenu.hpp"
#include "Core/GameConfig.hpp"
#include "Util/Renderer.hpp"
#include "Util/SFX.hpp"

enum class GameSceneState {
    WAVE,        // Active wave - enemies attacking
    STAT_SELECT, // Pick a free stat upgrade after each wave
    SHOPPING,    // Between waves - item shop
    PAUSED,      // ESC pause overlay
    GAME_OVER,
    VICTORY
};

class GameScene {
public:
    GameScene();

    // Returns current state (GAME_OVER / VICTORY tells App to switch)
    GameSceneState Update(float dt);

    // Renderer integration: call once to hook up the renderer
    void RegisterRenderer(Util::Renderer &renderer);

private:
    // --- Wave logic ---
    void StartWave();
    void EndWave();
    void SpawnEnemy(int forcedType = -1);
    glm::vec2 RandomSpawnPos() const;

    // --- Update subsystems ---
    void UpdateWave(float dt);
    void UpdateProjectiles(float dt);
    void CheckCollisions();
    void CleanupDead();
    bool ApplyShopItem(int idx);   // returns true if purchase succeeded
    void BuyWeapon(int weaponIdx);  // buy weapon from shop
    void SpawnDamageNumber(glm::vec2 pos, int damage, bool isCrit);
    void SpawnTrees();

    // --- Rendering helpers ---
    void SyncRendererObjects();
    void SyncWeaponsToRenderer();

    // --- Members ---
    std::shared_ptr<Player> m_Player;
    std::vector<std::shared_ptr<Enemy>> m_Enemies;
    std::vector<std::shared_ptr<Projectile>> m_Projectiles;

    HUD m_HUD;
    ShopScene m_ShopScene;
    Util::Renderer m_Renderer;

    GameSceneState m_State = GameSceneState::WAVE;
    int m_CurrentWave = 0;
    float m_WaveTimer = 0.f;

    // Spawning
    float m_SpawnTimer = 0.f;
    float m_SpawnInterval = 2.5f; // seconds between spawns
    int m_EnemiesSpawnedThisWave = 0;
    int m_MaxEnemiesPerWave = 8; // grows each wave

    // Tree spawning (spawn over time instead of all at wave start)
    float m_TreeSpawnTimer = 0.f;
    float m_TreeSpawnInterval = 5.5f;
    float m_FirstTreeSpawnDelay = 3.5f;
    int   m_TreesSpawnedThisWave = 0;
    int   m_MaxTreesThisWave = 0;

    // Background
    std::shared_ptr<Util::GameObject> m_Background;

    // Weapon GameObjects currently tracked in renderer
    std::vector<std::shared_ptr<Weapon>> m_RegisteredWeapons;

    // Dirty flag: re-add objects to renderer when entity lists change
    bool m_RendererDirty = true;

    // Sound effects
    std::shared_ptr<Util::SFX> m_SfxPistol;
    std::shared_ptr<Util::SFX> m_SfxSMG;
    std::shared_ptr<Util::SFX> m_SfxShotgun;
    std::shared_ptr<Util::SFX> m_SfxKnife;
    std::shared_ptr<Util::SFX> m_SfxHit;
    std::shared_ptr<Util::SFX> m_SfxDie;
    std::shared_ptr<Util::SFX> m_SfxBuy;
    std::shared_ptr<Util::SFX> m_SfxReroll;
    std::shared_ptr<Util::SFX> m_SfxWaveEnd;

    // Death particle effects
    struct DeathFX { std::shared_ptr<Util::GameObject> obj; float timer; };
    std::vector<DeathFX> m_DeathFX;

    // --- Stat selection + Pause ---
    StatSelectScene    m_StatSelectScene;
    PauseMenu          m_PauseMenu;
    GameSceneState     m_PrePauseState = GameSceneState::WAVE;
    std::vector<std::string> m_OwnedItems;
    int m_KillsThisWave = 0;

    // --- Boss ---
    std::shared_ptr<Enemy> m_Boss;   // non-null during boss wave, shared with m_Enemies

    // --- Floating damage numbers ---
    struct FloatText {
        std::shared_ptr<Util::GameObject> obj;
        float life;  // seconds remaining
    };
    std::vector<FloatText> m_FloatTexts;

    // --- Muzzle flash effects ---
    struct MuzzleFlash {
        std::shared_ptr<Util::GameObject> obj;
        float life;
        float maxLife;
        float baseScale;
    };
    std::vector<MuzzleFlash> m_MuzzleFlashes;

    // --- Trees and Fruits ---
    struct Tree {
        std::shared_ptr<Util::GameObject> obj;
        glm::vec2 pos{0.f, 0.f};
        bool alive = true;
        int hitsTaken = 0;
        int hitsToBreak = 8;
    };
    struct Fruit {
        std::shared_ptr<Util::GameObject> obj;
        glm::vec2 pos{0.f, 0.f};
        bool alive    = true;
        int  healAmount = 3;
    };
    // --- XP / Material Orbs (green drops from enemies = gold in Brotato) ---
    struct XpOrb {
        std::shared_ptr<Util::GameObject> obj;
        glm::vec2 pos{0.f, 0.f};
        int       value = 1;  // gold/materials to award on pickup
        bool      alive = true;
    };
    std::vector<Tree>  m_Trees;
    std::vector<Fruit> m_Fruits;
    std::vector<XpOrb> m_XpOrbs;
};
