#pragma once
#include "Entity/Entity.hpp"
#include "pch.hpp"

// Base enemy class
class Enemy : public Entity {
public:
    Enemy(const std::string &imagePath, int maxHp, float speed, int damage,
          int goldValue)
        : Entity(imagePath, 3.f, maxHp), m_Speed(speed), m_Damage(damage),
          m_GoldValue(goldValue) {}

    virtual ~Enemy() = default;

    bool Update(float dt) override;

    void SetTarget(glm::vec2 target) { m_Target = target; }
    int GetDamage() const { return m_Damage; }
    void SetDamage(int damage) { m_Damage = std::max(1, damage); }
    int GetGoldValue() const { return m_GoldValue; }

    // Returns true if reached player (deals contact damage)
    bool IsAtTarget() const;

    // Contact damage cooldown (per-enemy)
    bool IsAttackReady() const { return m_AttackTimer >= m_AttackCooldown; }
    void ResetAttackTimer() { m_AttackTimer = 0.f; }

    // Hit flash effect
    void TriggerHitFlash() { m_HitFlashTimer = 0.14f; }

protected:
    float m_Speed;
    int m_Damage;
    int m_GoldValue;
    glm::vec2 m_Target{0, 0};

    float m_AttackCooldown = 1.0f;
    float m_AttackTimer    = 0.f;

    float     m_HitFlashTimer = 0.f;
    glm::vec2 m_BaseScale     = {1.f, 1.f}; // set by derived ctor
};

// Chaser: fast, low hp, low damage
class Chaser : public Enemy {
public:
    explicit Chaser(glm::vec2 spawnPos);
};

// Bruiser: slow, high hp, high damage
class Bruiser : public Enemy {
public:
    explicit Bruiser(glm::vec2 spawnPos);
};

// Spitter: ranged enemy that shoots projectiles
class Spitter : public Enemy {
public:
    explicit Spitter(glm::vec2 spawnPos);

    bool Update(float dt) override;

    bool ShouldShoot()      { return m_ShootTimer >= m_ShootCooldown; }
    void ResetShootTimer()  { m_ShootTimer = 0.f; }

private:
    float m_ShootRange = 300.f;
    float m_ShootCooldown = 2.5f;
    float m_ShootTimer = 0.f;
};

// Guardian: heavy armored enemy, high HP, moderate speed
class Guardian : public Enemy {
public:
    explicit Guardian(glm::vec2 spawnPos);
};

// Bomber: fast suicide enemy — deals big damage then dies on first hit
class Bomber : public Enemy {
public:
    explicit Bomber(glm::vec2 spawnPos);
    bool IsBomber() const { return true; }
};

// Boss: final-wave mega enemy with ranged multi-shot + phase change
class Boss : public Enemy {
public:
    explicit Boss(glm::vec2 spawnPos, int wave);
    bool Update(float dt) override;

    bool  ShouldShoot() const { return m_ShootTimer >= m_ShootCooldown; }
    void  ResetShootTimer()   { m_ShootTimer = 0.f; }
    float GetHpFraction()const { return m_MaxHp > 0 ? static_cast<float>(m_Hp)/m_MaxHp : 0.f; }
    int   GetPhase()    const { return m_Phase; }

private:
    int   m_Phase         = 1;
    float m_ShootCooldown = 1.2f;
    float m_ShootTimer    = 0.f;
};
