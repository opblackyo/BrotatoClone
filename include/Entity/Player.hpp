#pragma once
#include "Core/GameConfig.hpp"
#include "Entity/Entity.hpp"
#include "Weapon/Weapon.hpp"
#include "pch.hpp"

static constexpr int MAX_WEAPON_SLOTS = 6;

class Player : public Entity {
public:
    Player();
    bool Update(float dt) override;

    // Weapons (max 6 slots)
    bool AddWeapon(std::shared_ptr<Weapon> weapon);
    std::shared_ptr<Weapon> SellWeaponAt(int index);
    const std::vector<std::shared_ptr<Weapon>> &GetWeapons() const { return m_Weapons; }
    int  GetWeaponCount() const { return static_cast<int>(m_Weapons.size()); }
    bool HasWeaponSlot()  const { return m_Weapons.size() < MAX_WEAPON_SLOTS; }
    bool CanAcceptWeapon(const Weapon &weapon) const;
    bool CanSellWeapon()  const { return m_Weapons.size() > 1; }
    int  GetWeaponSellValue(int index) const;

    // Economy
    int  GetGold()    const { return m_Gold; }
    void AddGold(int g)    { m_Gold += g; }
    bool SpendGold(int cost);

    // Stats
    float GetSpeed()           const { return m_Speed; }
    void  AddSpeed(float v)          { m_Speed = std::max(10.f, m_Speed + v); }

    int   GetArmor()           const { return m_Armor; }
    void  AddArmor(int v)            { m_Armor += v; }

    float GetMeleeDamageMult() const { return m_MeleeDamageMult; }
    void  AddMeleeDamage(float v)    { m_MeleeDamageMult = std::max(0.1f, m_MeleeDamageMult + v); }

    float GetRangedDamageMult()const { return m_RangedDamageMult; }
    void  AddRangedDamage(float v)   { m_RangedDamageMult = std::max(0.1f, m_RangedDamageMult + v); }

    float GetDamageMult()      const { return m_RangedDamageMult; } // compat

    float GetAttackSpeedMult() const { return m_AttackSpeedMult; }
    void  AddAttackSpeed(float v)    {
        float gain = v;
        if (m_AttackSpeedMult >= 1.4f) gain *= 0.55f;
        if (m_AttackSpeedMult >= 1.7f) gain *= 0.35f;
        m_AttackSpeedMult = std::clamp(m_AttackSpeedMult + gain, 0.2f, 1.75f);
    }

    float GetCritChance()      const { return m_CritChance; }
    void  AddCrit(float v)           { m_CritChance = std::min(m_CritChance + v, 0.95f); }

    float GetHarvesting()      const { return m_Harvesting; }
    void  AddHarvesting(float v)     { m_Harvesting = std::max(0.5f, m_Harvesting + v); }

    // --- NEW stats (matches Brotato stat panel) ---
    float GetHpRegen()     const { return m_HpRegen; }
    void  AddHpRegen(float v)    { m_HpRegen = std::max(0.f, m_HpRegen + v); }

    float GetLifesteal()   const { return m_Lifesteal; }
    void  AddLifesteal(float v)  { m_Lifesteal = std::min(m_Lifesteal + v, 0.5f); }

    float GetPctDamage()   const { return m_PctDamage; } // 0.2 = +20% all dmg
    void  AddPctDamage(float v)  { m_PctDamage += v; }

    float GetDodge()       const { return m_Dodge; }
    void  AddDodge(float v)      { m_Dodge = std::min(m_Dodge + v, 0.75f); }

    int   GetLuck()        const { return m_Luck; }
    void  AddLuck(int v)         { m_Luck += v; }

    float GetRangeMult()   const { return m_RangeMult; }
    void  AddRangeMult(float v)  { m_RangeMult = std::max(0.2f, m_RangeMult + v); }

    int   GetEngineering() const { return m_Engineering; }
    void  AddEngineering(int v)  { m_Engineering += v; }

    void  Heal(int hp)               { m_Hp = std::min(m_Hp + hp, m_MaxHp); }
    void  TakeDamage(int raw);       // armor-reduced, with invincibility frames
    bool  IsInvincible() const       { return m_InvFrameTimer > 0.f; }

    std::vector<float> GetWeaponCooldownProgress() const;
    void AddXP(int amount);
    void LevelUp();

    int GetXP() const { return m_XP; }
    int GetMaxXP() const { return m_MaxXP; }
    int GetLevel() const { return m_Level; }

private:
    void HandleMovement(float dt);
    void ClampToArena();
    void AutoMergeWeapons();

    int m_XP = 0;
    int m_MaxXP = 20; // 第一級升級需要的經驗值
    int m_Level = 1;

    std::vector<std::shared_ptr<Weapon>> m_Weapons;

    float m_Speed            = GameConfig::PLAYER_SPEED;
    int   m_Gold             = GameConfig::START_GOLD;
    int   m_Armor            = 0;
    float m_MeleeDamageMult  = 1.f;
    float m_RangedDamageMult = 1.f;
    float m_AttackSpeedMult  = 0.5f;
    float m_CritChance       = 0.05f;  // 5% default
    float m_Harvesting       = 1.f;
    // New stats
    float m_HpRegen          = 0.f;
    float m_Lifesteal        = 0.f;
    float m_PctDamage        = 0.f;
    float m_Dodge            = 0.f;
    int   m_Luck             = 0;
    float m_RangeMult        = 1.f;
    int   m_Engineering      = 0;
    float m_RegenAccum       = 0.f;
    float m_InvFrameTimer    = 0.f;  // invincibility after hit
};
