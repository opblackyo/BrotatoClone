#include "Weapon/Weapon.hpp"
#include <cmath>
#include <random>

static std::mt19937 &gRng() {
    static std::mt19937 rng(std::random_device{}());
    return rng;
}
static bool rollCrit(float chance) {
    std::uniform_real_distribution<float> d(0.f, 1.f);
    return d(gRng()) < chance;
}

// ---- Pistol ----
Pistol::Pistol()
    : Weapon(RESOURCE_DIR "/images/weapons/pistol.png", WeaponType::PISTOL,
             0.24f, 5, 300.f) {
    m_Timer = m_Cooldown;
    m_Name  = "Pistol";
    m_Transform.scale = {0.5f, 0.5f};
}

std::vector<std::shared_ptr<Projectile>>
Pistol::TryFire(glm::vec2 origin, glm::vec2 target, float damageMult,
                float critChance) {
    if (!IsReady()) return {};
    ResetTimer();
    bool crit  = rollCrit(critChance);
    int  dmg   = static_cast<int>(m_Damage * damageMult * (crit ? 2.f : 1.f));
    auto proj  = std::make_shared<Projectile>(
        RESOURCE_DIR "/images/projectiles/bullet.png",
        origin, target - origin, 1300.f, dmg, true, 0.24f);
    proj->SetPierce(1); // Can hit one extra enemy behind the first target.
    proj->m_Transform.scale = {0.58f, 0.09f};
    proj->SetCrit(crit);
    if (crit) proj->m_Transform.scale = {0.72f, 0.11f};
    return {proj};
}

// ---- Shotgun ----
Shotgun::Shotgun()
    : Weapon(RESOURCE_DIR "/images/weapons/shotgun.png", WeaponType::SHOTGUN,
             1.2f, 9, 360.f) {
    m_Timer = m_Cooldown;
    m_Name  = "Shotgun";
    m_Transform.scale = {0.6f, 0.6f};
}

std::vector<std::shared_ptr<Projectile>>
Shotgun::TryFire(glm::vec2 origin, glm::vec2 target, float damageMult,
                 float critChance) {
    if (!IsReady()) return {};
    ResetTimer();
    const float baseAngle = atan2f((target - origin).y, (target - origin).x);
    std::vector<std::shared_ptr<Projectile>> projs;
    for (int i = 0; i < 5; i++) {
        float angle = baseAngle + 0.30f * (-1.f + 0.5f * i); // -0.3..+0.3 rad
        glm::vec2 dir = {cosf(angle), sinf(angle)};
        bool crit = rollCrit(critChance);
        int  dmg  = static_cast<int>(m_Damage * damageMult * (crit ? 2.f : 1.f));
        auto p    = std::make_shared<Projectile>(
            RESOURCE_DIR "/images/projectiles/bullet.png",
            origin, dir, 880.f, dmg, true, 1.2f);
        p->SetCrit(crit);
        p->m_Transform.scale = {1.45f, 0.11f};
        p->m_Transform.rotation = angle;
        projs.push_back(p);
    }
    return projs;
}

// ---- SMG ----
SMG::SMG()
    : Weapon(RESOURCE_DIR "/images/weapons/smg.png", WeaponType::SMG,
             0.22f, 5, 430.f) {
    m_Timer = m_Cooldown;
    m_Name  = "SMG";
    m_Transform.scale = {0.45f, 0.45f};
}

std::vector<std::shared_ptr<Projectile>>
SMG::TryFire(glm::vec2 origin, glm::vec2 target, float damageMult,
             float critChance) {
    if (!IsReady()) return {};
    ResetTimer();
    bool crit = rollCrit(critChance);
    int  dmg  = static_cast<int>(m_Damage * damageMult * (crit ? 2.f : 1.f));
    auto proj = std::make_shared<Projectile>(
        RESOURCE_DIR "/images/projectiles/bullet.png",
        origin, target - origin, 1100.f, dmg, true);
    proj->SetCrit(crit);
    proj->m_Transform.scale = {0.82f, 0.09f};
    return {proj};
}

// ---- Knife ----
Knife::Knife()
    : Weapon(RESOURCE_DIR "/images/weapons/knife.png", WeaponType::KNIFE,
             0.5f, 22, 80.f) {
    m_Timer = m_Cooldown;
    m_Name  = "Knife";
    m_Transform.scale = {0.5f, 0.5f};
}

std::vector<std::shared_ptr<Projectile>>
Knife::TryFire(glm::vec2 origin, glm::vec2 target, float damageMult,
               float critChance) {
    if (!IsReady()) return {};
    ResetTimer();
    bool crit = rollCrit(critChance);
    int  dmg  = static_cast<int>(m_Damage * damageMult * (crit ? 2.f : 1.f));
    auto proj = std::make_shared<Projectile>(
        RESOURCE_DIR "/images/weapons/knife.png",
        origin, target - origin, 350.f, dmg, true, 0.13f);
    proj->SetCrit(crit);
    proj->m_Transform.scale = {0.8f, 0.8f};
    return {proj};
}
