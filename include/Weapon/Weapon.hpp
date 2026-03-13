#pragma once
#include "Weapon/Projectile.hpp"
#include "pch.hpp"
#include "Util/GameObject.hpp"
#include "Util/Image.hpp"

enum class WeaponType { PISTOL, SHOTGUN, SMG, KNIFE };

class Weapon : public Util::GameObject {
public:
    Weapon(const std::string &imagePath, WeaponType type, float cooldown,
           int damage, float range)
        : Util::GameObject(std::make_shared<Util::Image>(imagePath), 4.f),
          m_Type(type), m_Cooldown(cooldown), m_Damage(damage), m_Range(range) {}

    virtual ~Weapon() = default;

    WeaponType  GetType()       const { return m_Type; }
    bool        IsRanged()      const { return m_Type != WeaponType::KNIFE; }
    int         GetBaseDamage() const { return m_Damage; }
    float       GetRange()      const { return m_Range; }
    std::string GetName()       const { return m_Name; }
    int         GetLevel()      const { return m_Level; }

    // Merge/upgrade: +50% damage, -20% cooldown per level
    void LevelUp() {
        m_Level++;
        m_Damage   = static_cast<int>(m_Damage * 1.5f);
        m_Cooldown = std::max(0.08f, m_Cooldown * 0.8f);
        m_Name += " *";  // mark each upgrade level
    }

    float GetCooldownProgress() const {
        return (m_Cooldown > 0.f) ? m_Timer / m_Cooldown : 1.f;
    }
    bool  IsReady()          const { return m_Timer >= m_Cooldown; }
    void  ResetTimer()             { m_Timer = 0.f; }
    void  TickTimer(float dt)      { m_Timer = std::min(m_Timer + dt, m_Cooldown); }

    virtual std::vector<std::shared_ptr<Projectile>>
    TryFire(glm::vec2 origin, glm::vec2 target, float damageMult,
            float critChance = 0.05f) = 0;

protected:
    WeaponType  m_Type;
    float       m_Cooldown;
    int         m_Damage;
    float       m_Range;
    float       m_Timer = 0.f;
    std::string m_Name  = "Weapon";
    int         m_Level = 1;
};

class Pistol : public Weapon {
public:
    Pistol();
    std::vector<std::shared_ptr<Projectile>>
    TryFire(glm::vec2 origin, glm::vec2 target, float damageMult,
            float critChance = 0.05f) override;
};

class Shotgun : public Weapon {
public:
    Shotgun();
    std::vector<std::shared_ptr<Projectile>>
    TryFire(glm::vec2 origin, glm::vec2 target, float damageMult,
            float critChance = 0.05f) override;
};

class SMG : public Weapon {
public:
    SMG();
    std::vector<std::shared_ptr<Projectile>>
    TryFire(glm::vec2 origin, glm::vec2 target, float damageMult,
            float critChance = 0.05f) override;
};

class Knife : public Weapon {
public:
    Knife();
    std::vector<std::shared_ptr<Projectile>>
    TryFire(glm::vec2 origin, glm::vec2 target, float damageMult,
            float critChance = 0.05f) override;
};
