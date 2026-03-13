#include "Entity/Player.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include <cmath>

static int WeaponSellBase(WeaponType t) {
    switch (t) {
    case WeaponType::PISTOL:  return 2;
    case WeaponType::SHOTGUN: return 4;
    case WeaponType::SMG:     return 7;
    case WeaponType::KNIFE:   return 3;
    }
    return 2;
}

Player::Player()
    : Entity(RESOURCE_DIR "/images/player/well_rounded.png",
             2.f,
             GameConfig::PLAYER_MAX_HP) {
    m_Transform.scale = glm::vec2(0.62f, 0.62f);
    m_CollisionRadius = 22.f;
    AddWeapon(std::make_shared<Pistol>());
}

bool Player::Update(float dt) {
    HandleMovement(dt);
    // Tick invincibility frames
    if (m_InvFrameTimer > 0.f) m_InvFrameTimer -= dt;
    // HP regen
    if (m_HpRegen > 0.f && m_Hp < m_MaxHp) {
        m_RegenAccum += m_HpRegen * dt;
        if (m_RegenAccum >= 1.f) {
            int r = static_cast<int>(m_RegenAccum);
            Heal(r);
            m_RegenAccum -= static_cast<float>(r);
        }
    }
    ClampToArena();
    return IsAlive();
}

void Player::HandleMovement(float dt) {
    glm::vec2 dir{0, 0};
    if (Util::Input::IsKeyPressed(Util::Keycode::W) ||
        Util::Input::IsKeyPressed(Util::Keycode::UP))
        dir.y += 1.f;
    if (Util::Input::IsKeyPressed(Util::Keycode::S) ||
        Util::Input::IsKeyPressed(Util::Keycode::DOWN))
        dir.y -= 1.f;
    if (Util::Input::IsKeyPressed(Util::Keycode::A) ||
        Util::Input::IsKeyPressed(Util::Keycode::LEFT))
        dir.x -= 1.f;
    if (Util::Input::IsKeyPressed(Util::Keycode::D) ||
        Util::Input::IsKeyPressed(Util::Keycode::RIGHT))
        dir.x += 1.f;

    // Face movement direction on horizontal input.
    if (dir.x < 0.f)
        m_Transform.scale.x = -std::abs(m_Transform.scale.x);
    else if (dir.x > 0.f)
        m_Transform.scale.x = std::abs(m_Transform.scale.x);

    float len = glm::length(dir);
    if (len > 0.f)
        m_Transform.translation += (dir / len) * m_Speed * dt;
}

void Player::ClampToArena() {
    auto &t = m_Transform.translation;
    t.x = std::clamp(t.x, -GameConfig::ARENA_HALF_W, GameConfig::ARENA_HALF_W);
    t.y = std::clamp(t.y, -GameConfig::ARENA_HALF_H, GameConfig::ARENA_HALF_H);
}

void Player::AddWeapon(std::shared_ptr<Weapon> weapon) {
    if (HasWeaponSlot())
        m_Weapons.push_back(std::move(weapon));
}

std::shared_ptr<Weapon> Player::SellWeaponAt(int index) {
    if (!CanSellWeapon()) return nullptr;
    if (index < 0 || index >= static_cast<int>(m_Weapons.size())) return nullptr;

    auto sold = m_Weapons[index];
    m_Weapons.erase(m_Weapons.begin() + index);
    return sold;
}

int Player::GetWeaponSellValue(int index) const {
    if (index < 0 || index >= static_cast<int>(m_Weapons.size())) return 0;
    const auto &w = m_Weapons[index];
    const int base = WeaponSellBase(w->GetType());
    const int levelBonus = std::max(0, w->GetLevel() - 1) * std::max(1, base / 2);
    return base + levelBonus;
}

bool Player::SpendGold(int cost) {
    if (m_Gold >= cost) {
        m_Gold -= cost;
        return true;
    }
    return false;
}

void Player::TakeDamage(int raw) {
    if (m_InvFrameTimer > 0.f) return;   // blocked by invincibility frames
    int reduced = std::max(1, raw - m_Armor);
    m_Hp -= reduced;
    m_InvFrameTimer = 0.5f;              // 0.5 s of invincibility after hit
}

std::vector<float> Player::GetWeaponCooldownProgress() const {
    std::vector<float> result;
    for (const auto &w : m_Weapons)
        result.push_back(w->GetCooldownProgress());
    return result;
}

