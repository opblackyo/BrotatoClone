#include "Scene/GameScene.hpp"
#include "Weapon/Weapon.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Time.hpp"
#include "Util/Image.hpp"
#include "Util/Color.hpp"
#include <random>
#include <cstdlib>
#include <cmath>
#include <algorithm>

static constexpr float WIN_W = 1280.f;
static constexpr float WIN_H = 720.f;

GameScene::GameScene() {
    m_Player = std::make_shared<Player>();

    m_Background = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Image>(
            RESOURCE_DIR "/images/backgrounds/arena_bg.png"),
        0.f);
    m_Background->m_Transform.scale = {WIN_W / 512.f, WIN_H / 512.f};

    // Add HUD and shop objects to renderer immediately
    for (auto &obj : m_HUD.GetObjects())
        m_Renderer.AddChild(obj);
    for (auto &obj : m_ShopScene.GetObjects())
        m_Renderer.AddChild(obj);
    m_Renderer.AddChild(m_Background);
    m_Renderer.AddChild(m_Player);
    for (auto &obj : m_StatSelectScene.GetObjects())
        m_Renderer.AddChild(obj);
    for (auto &obj : m_PauseMenu.GetObjects())
        m_Renderer.AddChild(obj);

    // Wave 1
    m_CurrentWave = 1;
    m_ShopScene.Refresh(m_CurrentWave, m_Player->GetLuck());
    StartWave();

    // Initialize sound effects
    m_SfxPistol  = std::make_shared<Util::SFX>(RESOURCE_DIR "/sounds/shoot_pistol.wav");
    m_SfxSMG     = std::make_shared<Util::SFX>(RESOURCE_DIR "/sounds/shoot_smg.wav");
    m_SfxShotgun = std::make_shared<Util::SFX>(RESOURCE_DIR "/sounds/shoot_shotgun.wav");
    m_SfxKnife   = std::make_shared<Util::SFX>(RESOURCE_DIR "/sounds/shoot_knife.wav");
    m_SfxHit     = std::make_shared<Util::SFX>(RESOURCE_DIR "/sounds/enemy_hit.wav");
    m_SfxDie     = std::make_shared<Util::SFX>(RESOURCE_DIR "/sounds/enemy_die.wav");
    m_SfxBuy     = std::make_shared<Util::SFX>(RESOURCE_DIR "/sounds/shop_buy.wav");
    m_SfxReroll  = std::make_shared<Util::SFX>(RESOURCE_DIR "/sounds/shop_reroll.wav");
    m_SfxWaveEnd = std::make_shared<Util::SFX>(RESOURCE_DIR "/sounds/wave_end.wav");
    m_SfxPistol->SetVolume(55);  m_SfxSMG->SetVolume(40);
    m_SfxShotgun->SetVolume(58); m_SfxKnife->SetVolume(50);
    m_SfxHit->SetVolume(38);     m_SfxDie->SetVolume(52);
    m_SfxBuy->SetVolume(70);     m_SfxReroll->SetVolume(62);
    m_SfxWaveEnd->SetVolume(72);
}

void GameScene::RegisterRenderer(Util::Renderer &renderer) {
    // Not used - we maintain our own internal renderer
    (void)renderer;
}

GameSceneState GameScene::Update(float dt) {
    // ESC → pause (handles WAVE / STAT_SELECT / SHOPPING states)
    if (m_State == GameSceneState::WAVE     ||
        m_State == GameSceneState::STAT_SELECT ||
        m_State == GameSceneState::SHOPPING) {
        if (Util::Input::IsKeyDown(Util::Keycode::ESCAPE)) {
            m_PrePauseState = m_State;
            m_PauseMenu.Show(*m_Player, m_OwnedItems, m_CurrentWave);
            m_State = GameSceneState::PAUSED;
            m_Renderer.Update();
            return m_State;
        }
    }

    switch (m_State) {
    case GameSceneState::WAVE:
        UpdateWave(dt);
        break;

    case GameSceneState::STAT_SELECT: {
        int chosen = m_StatSelectScene.Update();
        if (chosen >= 0) {
            std::string name = m_StatSelectScene.ApplyUpgrade(chosen, *m_Player);
            m_OwnedItems.push_back("[\u2605] " + name);
            if (m_StatSelectScene.IsDone()) {
                m_StatSelectScene.Close();
                m_ShopScene.Show(*m_Player);
                m_State = GameSceneState::SHOPPING;
            }
        }
        break;
    }

    case GameSceneState::SHOPPING: {
        int choice = m_ShopScene.Update(*m_Player);
        if (choice == 10) {
            // Reroll
            if (m_Player->SpendGold(m_ShopScene.GetRerollCost())) {
                m_SfxReroll->Play();
                m_ShopScene.Refresh(m_CurrentWave, m_Player->GetLuck());
                m_ShopScene.Show(*m_Player);
            }
        } else if (choice >= 0 && choice < 4) {
            // Buy item — only remove from shop if purchase succeeded
            if (ApplyShopItem(choice)) {
                m_SfxBuy->Play();
                m_ShopScene.RemoveItem(choice);
                m_ShopScene.Show(*m_Player);
            }
        } else if (choice >= 20 && choice < 20 + MAX_WEAPON_SLOTS) {
            // Sell equipped weapon from slot index (must keep at least one)
            const int slot = choice - 20;
            const int refund = m_Player->GetWeaponSellValue(slot);
            auto sold = m_Player->SellWeaponAt(slot);
            if (sold) {
                m_Player->AddGold(refund);
                m_SfxBuy->Play();
                SyncWeaponsToRenderer();
                m_ShopScene.Show(*m_Player);
            }
        } else if (choice == 4) {
            // GO — advance to next wave
            m_ShopScene.Hide();
            m_CurrentWave++;
            if (m_CurrentWave > GameConfig::TOTAL_WAVES) {
                m_State = GameSceneState::VICTORY;
            } else {
                m_ShopScene.Refresh(m_CurrentWave, m_Player->GetLuck());
                StartWave();
                m_State = GameSceneState::WAVE;
            }
        }
        break;
    }

    case GameSceneState::PAUSED: {
        int r = m_PauseMenu.Update();
        if (r == 1) {
            m_PauseMenu.Hide();
            m_State = m_PrePauseState;
        } else if (r == 2) {
            std::exit(0);
        }
        break;
    }

    case GameSceneState::GAME_OVER:
    case GameSceneState::VICTORY:
        break;
    }

    const bool showGameplayHud =
        (m_State == GameSceneState::WAVE) ||
        (m_State == GameSceneState::PAUSED && m_PrePauseState == GameSceneState::WAVE);
    m_HUD.SetGameplayVisible(showGameplayHud);

    m_Renderer.Update();
    return m_State;
}

void GameScene::StartWave() {
    // Clean up renderer from previous wave
    for (auto &e : m_Enemies)     m_Renderer.RemoveChild(e);
    for (auto &p : m_Projectiles) m_Renderer.RemoveChild(p);
    for (auto &t : m_Trees)  { if (t.obj)  m_Renderer.RemoveChild(t.obj);  }
    for (auto &f : m_Fruits) { if (f.obj)  m_Renderer.RemoveChild(f.obj);  }
    for (auto &o : m_XpOrbs) { if (o.obj) m_Renderer.RemoveChild(o.obj);  }
    for (auto &mf : m_MuzzleFlashes) { if (mf.obj) m_Renderer.RemoveChild(mf.obj); }
    m_Trees.clear();
    m_Fruits.clear();
    m_XpOrbs.clear();
    m_MuzzleFlashes.clear();

    m_WaveTimer = GameConfig::WAVE_DURATION;
    m_SpawnTimer = 0.f;
    m_EnemiesSpawnedThisWave = 0;
    m_TreeSpawnTimer = 0.f;
    m_TreesSpawnedThisWave = 0;
    m_MaxTreesThisWave = std::clamp(2 + m_CurrentWave / 5, 2, 6);
    m_FirstTreeSpawnDelay = std::max(1.8f, 3.4f - 0.06f * m_CurrentWave);
    m_TreeSpawnInterval = std::max(3.2f, 6.0f - 0.08f * m_CurrentWave);
    // Boss wave: only 1 enemy spawns (the boss)
    if (m_CurrentWave >= GameConfig::BOSS_WAVE)
        m_MaxEnemiesPerWave = 1;
    else
        m_MaxEnemiesPerWave = std::clamp(10 + m_CurrentWave * 4, 14, 110);
    m_SpawnInterval = std::max(0.55f, 1.55f - m_CurrentWave * 0.07f);
    m_Enemies.clear();
    m_Projectiles.clear();
    m_Boss.reset();
    m_RendererDirty = true;
    SyncRendererObjects();
    SyncWeaponsToRenderer();
}

void GameScene::EndWave() {
    // Auto-collect any remaining gold/material orbs at wave end.
    int autoCollectedGold = 0;
    for (auto &orb : m_XpOrbs) {
        if (!orb.alive) continue;
        autoCollectedGold += orb.value;
        orb.alive = false;
        if (orb.obj) {
            orb.obj->SetVisible(false);
            m_Renderer.RemoveChild(orb.obj);
        }
    }
    m_XpOrbs.clear();
    if (autoCollectedGold > 0)
        m_Player->AddGold(autoCollectedGold);

    m_Player->AddGold(GameConfig::GOLD_PER_WAVE + m_CurrentWave * 2);
    m_SfxWaveEnd->Play();
    // Level-up: 2 stat picks if player killed >=75% of enemies this wave
    int picks = (m_KillsThisWave >= m_MaxEnemiesPerWave * 3 / 4) ? 2 : 1;
    m_KillsThisWave = 0;
    m_StatSelectScene.Open(picks);
    m_State = GameSceneState::STAT_SELECT;
}

void GameScene::SpawnTrees() {
    static std::mt19937 tRng(std::random_device{}());
    std::uniform_real_distribution<float> rx(
        -GameConfig::ARENA_HALF_W * 0.78f, GameConfig::ARENA_HALF_W * 0.78f);
    std::uniform_real_distribution<float> ry(
        -GameConfig::ARENA_HALF_H * 0.74f, GameConfig::ARENA_HALF_H * 0.74f);
    std::uniform_real_distribution<float> scaleDist(0.34f, 0.42f);
    std::uniform_int_distribution<int> flipDist(0, 1);

    // Spawn one tree per call, with spacing checks to avoid clumping.
    glm::vec2 spawnPos{0.f, 0.f};
    bool foundPos = false;
    const glm::vec2 playerPos = m_Player->GetPos();
    constexpr float minDistToPlayer = 170.f;
    constexpr float minDistToOtherTrees = 160.f;

    for (int tries = 0; tries < 28 && !foundPos; ++tries) {
        glm::vec2 candidate = {rx(tRng), ry(tRng)};
        if (glm::length(candidate - playerPos) < minDistToPlayer)
            continue;

        bool tooClose = false;
        for (const auto &tree : m_Trees) {
            if (!tree.alive) continue;
            if (glm::length(candidate - tree.pos) < minDistToOtherTrees) {
                tooClose = true;
                break;
            }
        }
        if (tooClose) continue;

        spawnPos = candidate;
        foundPos = true;
    }

    if (!foundPos) return;

    Tree t;
    t.pos = spawnPos;
    t.obj = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Image>(RESOURCE_DIR "/images/environment/tree.png"), 1.6f);
    float scale = scaleDist(tRng);
    t.obj->m_Transform.translation = t.pos;
    t.obj->m_Transform.scale = {
        flipDist(tRng) == 0 ? scale : -scale,
        scale
    };
    m_Renderer.AddChild(t.obj);
    m_Trees.push_back(t);
    ++m_TreesSpawnedThisWave;
}

glm::vec2 GameScene::RandomSpawnPos() const {    // Spawn just outside the arena edges
    static std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> side(0, 3);
    std::uniform_real_distribution<float>
        edgeX(-GameConfig::ARENA_HALF_W, GameConfig::ARENA_HALF_W);
    std::uniform_real_distribution<float>
        edgeY(-GameConfig::ARENA_HALF_H, GameConfig::ARENA_HALF_H);

    const float margin = 60.f;
    switch (side(rng)) {
    case 0: return {edgeX(rng), -GameConfig::ARENA_HALF_H - margin};
    case 1: return {edgeX(rng),  GameConfig::ARENA_HALF_H + margin};
    case 2: return {-GameConfig::ARENA_HALF_W - margin, edgeY(rng)};
    default:return { GameConfig::ARENA_HALF_W + margin, edgeY(rng)};
    }
}

void GameScene::SpawnEnemy(int forcedType) {
    glm::vec2 pos = RandomSpawnPos();
    int wave = m_CurrentWave;

    // Final wave: spawn the Boss once
    if (wave >= GameConfig::BOSS_WAVE) {
        auto boss = std::make_shared<Boss>(pos, wave);
        m_Boss = boss;
        m_Enemies.push_back(boss);
        m_Renderer.AddChild(boss);
        m_HUD.ShowBossBar("\u26a1 FINAL BOSS \u26a1");
        ++m_EnemiesSpawnedThisWave;
        return;
    }

    // Regular waves: pick enemy type based on wave progress
    // Types unlocked per wave tier:
    // wave 1-3  : Chaser, Bruiser
    // wave 4-7  : + Spitter
    // wave 8-11 : + Guardian
    // wave 12+  : + Bomber
    int maxType = (wave >= 12) ? 5 : (wave >= 8) ? 4 : (wave >= 4) ? 3 : 2;
    int type = forcedType;
    if (type < 0)
        type = (m_EnemiesSpawnedThisWave + wave) % maxType;
    type = std::clamp(type, 0, maxType - 1);

    std::shared_ptr<Enemy> enemy;
    int baseHp = 1;
    int baseDamage = 1;
    float hpIncreasePerWave = 1.0f;
    float damageIncreasePerWave = 0.6f;
    switch (type) {
        case 0:
            enemy = std::make_shared<Chaser>(pos);
            baseHp = 1; baseDamage = 1;
            hpIncreasePerWave = 1.0f; damageIncreasePerWave = 0.6f; // chaser_stats.tres
            break;
        case 1:
            enemy = std::make_shared<Bruiser>(pos);
            baseHp = 20; baseDamage = 2;
            hpIncreasePerWave = 11.0f; damageIncreasePerWave = 0.85f; // bruiser_stats.tres
            break;
        case 2:
            enemy = std::make_shared<Spitter>(pos);
            baseHp = 8; baseDamage = 1;
            hpIncreasePerWave = 1.0f; damageIncreasePerWave = 0.6f; // spitter_stats.tres
            break;
        case 3:
            enemy = std::make_shared<Guardian>(pos);
            baseHp = 30; baseDamage = 1;
            hpIncreasePerWave = 22.0f; damageIncreasePerWave = 1.15f; // horned_bruiser-like tier
            break;
        default:
            enemy = std::make_shared<Bomber>(pos);
            baseHp = 4; baseDamage = 1;
            hpIncreasePerWave = 2.5f; damageIncreasePerWave = 0.85f; // charger_stats.tres
            break;
    }

    // Brotato-source growth rates adapted to this clone's lower player HP baseline.
    const int waveOffset = std::max(0, wave - 1);
    const int scaledHp = std::max(1,
        static_cast<int>(std::round(baseHp + waveOffset * hpIncreasePerWave * 0.72f)));
    const int scaledDamage = std::max(1,
        static_cast<int>(std::round((baseDamage + waveOffset * damageIncreasePerWave) * 0.62f)));

    enemy->SetMaxHp(scaledHp);
    enemy->SetHp(scaledHp);
    enemy->SetDamage(scaledDamage);

    m_Enemies.push_back(enemy);
    m_Renderer.AddChild(enemy);
    ++m_EnemiesSpawnedThisWave;
}

void GameScene::UpdateWave(float dt) {
    // Update wave timer
    m_WaveTimer -= dt;

    // Spawn enemies
    if (m_EnemiesSpawnedThisWave < m_MaxEnemiesPerWave) {
        m_SpawnTimer += dt;
        const int aliveEnemyCap = std::clamp(24 + m_CurrentWave * 3, 28, 90);
        while (m_SpawnTimer >= m_SpawnInterval &&
               m_EnemiesSpawnedThisWave < m_MaxEnemiesPerWave) {
            m_SpawnTimer -= m_SpawnInterval;

            // Keep a safety cap for older hardware / framework limits.
            if (static_cast<int>(m_Enemies.size()) >= aliveEnemyCap)
                break;

            int batchCount = std::clamp(1 + m_CurrentWave / 4, 1, 5);
            int remaining = m_MaxEnemiesPerWave - m_EnemiesSpawnedThisWave;
            batchCount = std::min(batchCount, remaining);

            for (int i = 0; i < batchCount; ++i) {
                if (static_cast<int>(m_Enemies.size()) >= aliveEnemyCap ||
                    m_EnemiesSpawnedThisWave >= m_MaxEnemiesPerWave)
                    break;

                // Batch spawns favor chasers so economy pacing feels closer
                // to Brotato without introducing too many heavy units at once.
                const bool forceSmall = (batchCount > 1 && i < batchCount - 1);
                SpawnEnemy(forceSmall ? 0 : -1);
            }
        }
    }

    // Spawn trees over time instead of all at wave start.
    if (m_TreesSpawnedThisWave < m_MaxTreesThisWave) {
        m_TreeSpawnTimer += dt;
        const float wait = (m_TreesSpawnedThisWave == 0) ? m_FirstTreeSpawnDelay
                                                         : m_TreeSpawnInterval;
        if (m_TreeSpawnTimer >= wait) {
            m_TreeSpawnTimer = 0.f;
            SpawnTrees();
        }
    }

    // Update player (movement + weapon timers)
    m_Player->Update(dt);

    // Auto-fire weapons; also orbit weapon icon around player
    glm::vec2 playerPos = m_Player->GetPos();
    const bool forceTreeTarget = Util::Input::IsKeyPressed(Util::Keycode::T);
    const auto &weaponList = m_Player->GetWeapons();
    for (int wi = 0; wi < static_cast<int>(weaponList.size()); wi++) {
        auto &weapon = weaponList[wi];

        // Tick weapon cooldown (scaled by attack speed)
        weapon->TickTimer(dt * m_Player->GetAttackSpeedMult());

        // 左側 wi=0,1,2；右側 wi=3,4,5，各上/中/下排列
        static const glm::vec2 WSLOT_OFF[6] = {
            {-36.f, +22.f}, {-36.f, 0.f}, {-36.f, -22.f},
            {+36.f, +22.f}, {+36.f, 0.f}, {+36.f, -22.f}
        };
        if (wi < 6)
            weapon->m_Transform.translation = playerPos + WSLOT_OFF[wi];

        // Build in-range target lists and distribute targets by weapon slot index,
        // so multiple weapons do not always focus the same enemy.
        const float weaponRange = weapon->GetRange() * m_Player->GetRangeMult();
        std::vector<std::pair<float, glm::vec2>> enemyTargets;
        std::vector<std::pair<float, glm::vec2>> treeTargets;
        enemyTargets.reserve(m_Enemies.size());
        treeTargets.reserve(m_Trees.size());

        for (const auto &e : m_Enemies) {
            if (!e->IsAlive()) continue;
            float d = glm::length(e->GetPos() - playerPos);
            if (d < weaponRange)
                enemyTargets.emplace_back(d, e->GetPos());
        }

        for (const auto &t : m_Trees) {
            if (!t.alive) continue;
            float d = glm::length(t.pos - playerPos);
            if (d < weaponRange)
                treeTargets.emplace_back(d, t.pos);
        }

        glm::vec2 target = playerPos + glm::vec2(1.f, 0.f);
        bool foundTarget = false;
        auto selectDistributed = [&](std::vector<std::pair<float, glm::vec2>> &pool) {
            std::sort(pool.begin(), pool.end(),
                      [](const auto &a, const auto &b) { return a.first < b.first; });
            const int targetIndex = wi % static_cast<int>(pool.size());
            target = pool[targetIndex].second;
            foundTarget = true;
        };

        // Press and hold T to proactively focus trees.
        if (forceTreeTarget && !treeTargets.empty()) {
            selectDistributed(treeTargets);
        } else if (!enemyTargets.empty()) {
            selectDistributed(enemyTargets);
        } else if (!treeTargets.empty()) {
            selectDistributed(treeTargets);
        }

        if (foundTarget) {

            glm::vec2 dir = target - playerPos;
            weapon->m_Transform.rotation = atan2f(dir.y, dir.x);

            float dmgMult = weapon->IsRanged() ? m_Player->GetRangedDamageMult()
                                               : m_Player->GetMeleeDamageMult();
            dmgMult *= (1.f + m_Player->GetPctDamage());
            auto projs = weapon->TryFire(playerPos, target, dmgMult,
                                         m_Player->GetCritChance());
            if (!projs.empty()) {
                switch (weapon->GetType()) {
                    case WeaponType::PISTOL:  m_SfxPistol->Play();  break;
                    case WeaponType::SHOTGUN: m_SfxShotgun->Play(); break;
                    case WeaponType::SMG:     m_SfxSMG->Play();     break;
                    case WeaponType::KNIFE:   m_SfxKnife->Play();   break;
                }

                if (weapon->IsRanged()) {
                    const float len = glm::length(dir);
                    if (len > 0.001f) {
                        glm::vec2 fireDir = dir / len;
                        float flashScale = 0.18f;
                        float flashLife  = 0.055f;
                        float flashOffset = 20.f;

                        if (weapon->GetType() == WeaponType::SMG) {
                            flashScale = 0.14f;
                            flashLife  = 0.045f;
                            flashOffset = 17.f;
                        } else if (weapon->GetType() == WeaponType::SHOTGUN) {
                            flashScale = 0.26f;
                            flashLife  = 0.070f;
                            flashOffset = 23.f;
                        }

                        auto flash = std::make_shared<Util::GameObject>(
                            std::make_shared<Util::Image>(RESOURCE_DIR "/images/particles/particle_1.png"), 6.2f);
                        flash->m_Transform.translation = playerPos + fireDir * flashOffset;
                        flash->m_Transform.scale       = {flashScale, flashScale};
                        flash->m_Transform.rotation    = atan2f(fireDir.y, fireDir.x);
                        m_Renderer.AddChild(flash);
                        m_MuzzleFlashes.push_back({flash, flashLife, flashLife, flashScale});
                    }
                }
            }
            for (auto &p : projs) {
                m_Projectiles.push_back(p);
                m_Renderer.AddChild(p);
            }
        }
    }

    // Update enemies: move toward player + handle contact damage
    for (auto &e : m_Enemies) {
        if (!e->IsAlive()) continue;
        e->SetTarget(playerPos);

        auto *spitter = dynamic_cast<Spitter *>(e.get());
        if (spitter && spitter->ShouldShoot()) {
            spitter->ResetShootTimer();
            glm::vec2 dir = playerPos - e->GetPos();
            auto proj = std::make_shared<Projectile>(
                RESOURCE_DIR "/images/projectiles/bullet_enemy.png",
                e->GetPos(), dir, 320.f, e->GetDamage(), false);
            m_Projectiles.push_back(proj);
            m_Renderer.AddChild(proj);
        }

        // Boss: multi-directional spread shot attack
        auto *boss = dynamic_cast<Boss *>(e.get());
        if (boss && boss->ShouldShoot()) {
            boss->ResetShootTimer();
            int numShots = (boss->GetPhase() == 2) ? 8 : 5;
            float baseAngle = atan2f(playerPos.y - e->GetPos().y,
                                     playerPos.x - e->GetPos().x);
            for (int bi = 0; bi < numShots; bi++) {
                float angle = baseAngle +
                    (static_cast<float>(bi) - (numShots - 1) * 0.5f) * 0.28f;
                glm::vec2 dir = {cosf(angle), sinf(angle)};
                auto proj = std::make_shared<Projectile>(
                    RESOURCE_DIR "/images/projectiles/bullet_enemy.png",
                    e->GetPos(), dir, 360.f, boss->GetDamage(), false);
                proj->m_Transform.scale = {0.65f, 0.65f};
                m_Projectiles.push_back(proj);
                m_Renderer.AddChild(proj);
            }
        }

        e->Update(dt);

        // Contact damage (using per-enemy attack timer) with dodge
        float dist = glm::length(e->GetPos() - playerPos);
        if (dist < e->GetCollisionRadius() + m_Player->GetCollisionRadius()) {
            if (e->IsAttackReady()) {
                float roll = static_cast<float>(std::rand() % 10000) / 10000.f;
                if (roll >= m_Player->GetDodge())
                    m_Player->TakeDamage(e->GetDamage());
                e->ResetAttackTimer();
                // Bomber dies after its first attack
                if (dynamic_cast<Bomber *>(e.get()))
                    e->SetHp(0);
            }
        }
    }

    // Update projectiles
    UpdateProjectiles(dt);
    CheckCollisions();
    CleanupDead();

    // Update death particle effects
    for (auto &fx : m_DeathFX) {
        fx.timer -= dt;
        float progress = 1.f - std::max(0.f, fx.timer) / 0.4f;
        float s = 0.3f + progress * 1.7f;
        fx.obj->m_Transform.scale = {s, s};
        if (fx.timer <= 0.f) fx.obj->SetVisible(false);
    }
    m_DeathFX.erase(std::remove_if(m_DeathFX.begin(), m_DeathFX.end(),
        [&](const DeathFX &f) {
            if (f.timer <= 0.f) { m_Renderer.RemoveChild(f.obj); return true; }
            return false;
        }), m_DeathFX.end());

    // Animate floating damage numbers (rise + shrink)
    for (auto &ft : m_FloatTexts) {
        ft.life -= dt;
        ft.obj->m_Transform.translation.y += 55.f * dt;
        float progress = 1.f - std::max(0.f, ft.life) / 0.85f;
        float s = 1.f - progress * 0.72f;
        ft.obj->m_Transform.scale = {s, s};
        if (ft.life <= 0.f) ft.obj->SetVisible(false);
    }
    m_FloatTexts.erase(std::remove_if(m_FloatTexts.begin(), m_FloatTexts.end(),
        [&](const FloatText &f) {
            if (f.life <= 0.f) { m_Renderer.RemoveChild(f.obj); return true; }
            return false;
        }), m_FloatTexts.end());

    // Muzzle flash: short fire burst at gun barrel.
    for (auto &mf : m_MuzzleFlashes) {
        mf.life -= dt;
        const float maxLife = (mf.maxLife > 0.0001f) ? mf.maxLife : 0.0001f;
        float t = std::clamp(mf.life / maxLife, 0.f, 1.f);
        float s = mf.baseScale * (0.55f + t * 0.65f);
        mf.obj->m_Transform.scale = {s, s};
        if (mf.life <= 0.f) mf.obj->SetVisible(false);
    }
    m_MuzzleFlashes.erase(std::remove_if(m_MuzzleFlashes.begin(), m_MuzzleFlashes.end(),
        [&](const MuzzleFlash &mf) {
            if (mf.life <= 0.f) { m_Renderer.RemoveChild(mf.obj); return true; }
            return false;
        }), m_MuzzleFlashes.end());

    // Update boss HP bar
    if (m_Boss && m_Boss->IsAlive())
        m_HUD.UpdateBossBar(m_Boss->GetHp(), m_Boss->GetMaxHp());

    // Fruit pickup: player walks over dropped fruit to heal
    for (auto &f : m_Fruits) {
        if (!f.alive) continue;
        float dist = glm::length(f.pos - playerPos);
        if (dist < 30.f + m_Player->GetCollisionRadius() &&
            m_Player->GetHp() < m_Player->GetMaxHp()) {
            f.alive = false;
            f.obj->SetVisible(false);
            m_Renderer.RemoveChild(f.obj);
            m_Player->Heal(f.healAmount);
        }
    }
    m_Fruits.erase(std::remove_if(m_Fruits.begin(), m_Fruits.end(),
        [](const Fruit &f) { return !f.alive; }), m_Fruits.end());

    // XP/Material orb pickup: auto-magnet when close, like Brotato
    // Pickup radius = player collision radius + base pickup range
    // (larger pickup range = harvesting stat in real Brotato;
    //  here we use a fixed 80px + 20px per harvesting level approximation)
    const float pickupRange = 80.f + m_Player->GetCollisionRadius()
                              + (m_Player->GetHarvesting() - 1.f) * 60.f;
    for (auto &orb : m_XpOrbs) {
        if (!orb.alive) continue;
        float dist = glm::length(orb.pos - playerPos);
        // Magnet: move toward player if within range
        if (dist < pickupRange) {
            glm::vec2 dir = playerPos - orb.pos;
            float speed = 300.f * (1.f - dist / pickupRange) + 60.f;
            orb.pos += glm::normalize(dir) * speed * dt;
            orb.obj->m_Transform.translation = orb.pos;
            dist = glm::length(orb.pos - playerPos);
        }
        // Collect when touching player
        if (dist < m_Player->GetCollisionRadius()) {
            orb.alive = false;
            orb.obj->SetVisible(false);
            m_Renderer.RemoveChild(orb.obj);
            m_Player->AddGold(orb.value);
        }
    }
    m_XpOrbs.erase(std::remove_if(m_XpOrbs.begin(), m_XpOrbs.end(),
        [](const XpOrb &o) { return !o.alive; }), m_XpOrbs.end());

    // Update HUD
    m_HUD.Update(m_Player->GetHp(), m_Player->GetMaxHp(),
                 m_Player->GetGold(), m_CurrentWave,
                 GameConfig::TOTAL_WAVES, m_WaveTimer);

    // Check end conditions
    if (!m_Player->IsAlive()) {
        m_State = GameSceneState::GAME_OVER;
        return;
    }

    // Boss wave victory: boss killed → win
    if (m_CurrentWave >= GameConfig::BOSS_WAVE && m_Boss && !m_Boss->IsAlive()) {
        m_HUD.HideBossBar();
        m_State = GameSceneState::VICTORY;
        return;
    }

    // Normal wave end (boss wave never auto-expires)
    bool allDone = (m_CurrentWave < GameConfig::BOSS_WAVE) &&
                   ((m_WaveTimer <= 0.f) ||
                    (m_EnemiesSpawnedThisWave >= m_MaxEnemiesPerWave &&
                     m_Enemies.empty()));
    if (allDone) {
        EndWave();
    }
}

void GameScene::UpdateProjectiles(float dt) {
    for (auto &p : m_Projectiles) {
        p->Update(dt);
    }
}

void GameScene::CheckCollisions() {
    for (auto &proj : m_Projectiles) {
        if (proj->IsExpired()) continue;

        if (proj->IsPlayerOwned()) {
            // Check against enemies
            for (auto &e : m_Enemies) {
                if (proj->IsExpired()) break;
                if (!e->IsAlive()) continue;
                if (!proj->CanHitTarget(e.get())) continue;
                float dist = glm::length(proj->GetPos() - e->GetPos());
                if (dist < proj->GetRadius() + e->GetCollisionRadius()) {
                    e->TakeDamage(proj->GetDamage());
                    e->TriggerHitFlash();
                    m_SfxHit->Play();
                    SpawnDamageNumber(e->GetPos(), proj->GetDamage(), proj->IsCrit());
                    // Lifesteal
                    if (m_Player->GetLifesteal() > 0.f) {
                        int steal = std::max(1, static_cast<int>(proj->GetDamage() * m_Player->GetLifesteal()));
                        m_Player->Heal(steal);
                    }

                    // Register this enemy hit. Projectiles with pierce can continue.
                    if (!proj->RegisterHit(e.get()))
                        proj->Update(999.f);
                }
            }
        } else {
            // Enemy projectile hits player (with dodge)
            float dist = glm::length(proj->GetPos() - m_Player->GetPos());
            if (dist < proj->GetRadius() + m_Player->GetCollisionRadius()) {
                float roll = static_cast<float>(std::rand() % 10000) / 10000.f;
                if (roll >= m_Player->GetDodge())
                    m_Player->TakeDamage(proj->GetDamage());
                proj->Update(999.f);
            }
        }
    }

    // Player projectiles can hit trees → destroy tree, drop fruit
    for (auto &proj : m_Projectiles) {
        if (!proj->IsPlayerOwned() || proj->IsExpired()) continue;
        for (auto &t : m_Trees) {
            if (!t.alive) continue;
            float dist = glm::length(proj->GetPos() - t.pos);
            if (dist < proj->GetRadius() + 32.f) {
                t.hitsTaken += 1;
                proj->Update(999.f);
                if (t.hitsTaken >= t.hitsToBreak) {
                    t.alive = false;
                    t.obj->SetVisible(false);
                    m_Renderer.RemoveChild(t.obj);
                    // Spawn a fruit when the tree is fully destroyed.
                    Fruit f;
                    f.pos = t.pos;
                    f.obj = std::make_shared<Util::GameObject>(
                        std::make_shared<Util::Image>(RESOURCE_DIR "/images/environment/fruit.png"), 3.8f);
                    f.obj->m_Transform.translation = f.pos;
                    f.obj->m_Transform.scale       = {0.55f, 0.55f};
                    m_Renderer.AddChild(f.obj);
                    m_Fruits.push_back(f);
                }
                break; // one tree per projectile
            }
        }
    }
    // Clean up dead trees
    m_Trees.erase(std::remove_if(m_Trees.begin(), m_Trees.end(),
        [](const Tree &t) { return !t.alive; }), m_Trees.end());
}

void GameScene::CleanupDead() {
    // Remove dead enemies
    auto enemyIt = std::remove_if(m_Enemies.begin(), m_Enemies.end(),
                                  [&](const std::shared_ptr<Enemy> &e) {
                                      if (!e->IsAlive()) {
                                          // Death sound + particle burst
                                          m_SfxDie->Play();
                                          m_KillsThisWave++;
                                          auto fx = std::make_shared<Util::GameObject>(
                                              std::make_shared<Util::Image>(
                                                  RESOURCE_DIR "/images/particles/particle_1.png"), 6.f);
                                          fx->m_Transform.translation = e->GetPos();
                                          fx->m_Transform.scale = {0.3f, 0.3f};
                                          m_Renderer.AddChild(fx);
                                          m_DeathFX.push_back({fx, 0.4f});

                                          // Spawn XP/material orb(s) — green drops like Brotato
                                          int goldDropped = static_cast<int>(
                                              e->GetGoldValue() * m_Player->GetHarvesting());
                                          // Drop 1 orb per enemy; larger enemies drop bigger value
                                          XpOrb orb;
                                          orb.pos   = e->GetPos();
                                          orb.value = goldDropped;
                                          orb.obj   = std::make_shared<Util::GameObject>(
                                              std::make_shared<Util::Image>(
                                                  RESOURCE_DIR "/images/particles/xp_orb.png"), 3.6f);
                                          orb.obj->m_Transform.translation = orb.pos;
                                          orb.obj->m_Transform.scale       = {0.45f, 0.45f};
                                          m_Renderer.AddChild(orb.obj);
                                          m_XpOrbs.push_back(orb);

                                          m_Renderer.RemoveChild(e);
                                          return true;
                                      }
                                      return false;
                                  });
    m_Enemies.erase(enemyIt, m_Enemies.end());

    // Remove expired projectiles
    auto projIt = std::remove_if(
        m_Projectiles.begin(), m_Projectiles.end(),
        [&](const std::shared_ptr<Projectile> &p) {
            if (p->IsExpired()) {
                m_Renderer.RemoveChild(p);
                return true;
            }
            return false;
        });
    m_Projectiles.erase(projIt, m_Projectiles.end());
}

bool GameScene::ApplyShopItem(int idx) {
    const auto &items = m_ShopScene.GetItems();
    if (idx < 0 || idx >= static_cast<int>(items.size())) return false;
    const ShopItem &item = items[idx];

    const bool isWeapon = item.effectType == "add_pistol" ||
                          item.effectType == "add_shotgun" ||
                          item.effectType == "add_smg" ||
                          item.effectType == "add_knife";

    if (isWeapon && !m_Player->HasWeaponSlot()) return false;

    if (!m_Player->SpendGold(item.cost)) return false;
    m_OwnedItems.push_back(item.name); // track for pause menu

    // All items are passive stat items (no direct-heal or weapon adds in shop)
    const std::string &t = item.effectType;
    const int          v = item.effectValue;

    if (isWeapon) {
        std::shared_ptr<Weapon> newWeapon;
        if      (t == "add_pistol")  newWeapon = std::make_shared<Pistol>();
        else if (t == "add_shotgun") newWeapon = std::make_shared<Shotgun>();
        else if (t == "add_smg")     newWeapon = std::make_shared<SMG>();
        else if (t == "add_knife")   newWeapon = std::make_shared<Knife>();

        if (newWeapon) {
            m_Player->AddWeapon(newWeapon);
            SyncWeaponsToRenderer();
        }
        return true;
    }

    if (t == "hp_up") {
        m_Player->SetMaxHp(m_Player->GetMaxHp() + v);
        m_Player->Heal(v);
    } else if (t == "armor_up")
        m_Player->AddArmor(v);
    else if (t == "ranged_dmg")
        m_Player->AddRangedDamage(v / 100.f);
    else if (t == "melee_dmg")
        m_Player->AddMeleeDamage(v / 100.f);
    else if (t == "pct_damage")
        m_Player->AddPctDamage(v / 100.f);
    else if (t == "elem_dmg")
        m_Player->AddPctDamage(v / 100.f);   // map elemental → global % for now
    else if (t == "speed_up")
        m_Player->AddSpeed(static_cast<float>(v));
    else if (t == "attack_speed")
        m_Player->AddAttackSpeed(v / 100.f);
    else if (t == "crit_up")
        m_Player->AddCrit(v / 100.f);
    else if (t == "harvesting")
        m_Player->AddHarvesting(v / 100.f);
    else if (t == "hp_regen")
        m_Player->AddHpRegen(v / 10.f);   // 10 → +1.0 HP/s, 3 → +0.3 HP/s
    else if (t == "lifesteal")
        m_Player->AddLifesteal(v / 100.f);
    else if (t == "dodge")
        m_Player->AddDodge(v / 100.f);
    else if (t == "luck")
        m_Player->AddLuck(v);
    else if (t == "range")
        m_Player->AddRangeMult(v / 100.f);
    else if (t == "engineering")
        m_Player->AddEngineering(v);
    return true;
}

void GameScene::BuyWeapon(int weaponIdx) {
    const auto &weapItems = m_ShopScene.GetWeaponItems();
    if (weaponIdx < 0 || weaponIdx >= static_cast<int>(weapItems.size())) return;
    const ShopItem &witem = weapItems[weaponIdx];
    if (!m_Player->HasWeaponSlot()) return;   // full — can't buy
    if (!m_Player->SpendGold(witem.cost)) return;

    std::shared_ptr<Weapon> newWeapon;
    if      (witem.effectType == "add_pistol")  newWeapon = std::make_shared<Pistol>();
    else if (witem.effectType == "add_shotgun") newWeapon = std::make_shared<Shotgun>();
    else if (witem.effectType == "add_smg")     newWeapon = std::make_shared<SMG>();
    else if (witem.effectType == "add_knife")   newWeapon = std::make_shared<Knife>();

    if (newWeapon) {
        m_Player->AddWeapon(newWeapon);
        m_SfxBuy->Play();
        SyncWeaponsToRenderer();
        m_ShopScene.RemoveWeaponItem(weaponIdx);
        m_ShopScene.Show(*m_Player);
    }
}

// Spawn a floating damage number at position
void GameScene::SpawnDamageNumber(glm::vec2 pos, int damage, bool isCrit) {
    static const std::string FONT_PATH = RESOURCE_DIR "/fonts/Inter.ttf";
    auto txt = std::make_shared<Util::Text>(
        FONT_PATH,
        isCrit ? 22 : 16,
        std::to_string(damage),
        isCrit ? Util::Color::FromRGB(255, 90, 20)
               : Util::Color::FromRGB(255, 235, 55));
    auto obj = std::make_shared<Util::GameObject>(txt, 7.5f);
    obj->m_Transform.translation = pos + glm::vec2(0.f, 18.f);
    m_Renderer.AddChild(obj);
    m_FloatTexts.push_back({obj, 0.85f});
}

void GameScene::SyncRendererObjects() {
    // Only used at StartWave; individual entities are added/removed dynamically
}

void GameScene::SyncWeaponsToRenderer() {
    for (auto &w : m_RegisteredWeapons)
        m_Renderer.RemoveChild(w);
    m_RegisteredWeapons.clear();
    for (auto &w : m_Player->GetWeapons()) {
        m_Renderer.AddChild(w);
        m_RegisteredWeapons.push_back(w);
    }
}

// ShowShopIcons() removed — shop now manages its own icons via ShopScene
