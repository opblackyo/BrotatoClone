#pragma once
#include "pch.hpp"
#include "Util/GameObject.hpp"
#include "Util/Image.hpp"

// Base entity class - all game objects (player, enemies) inherit from this.
class Entity : public Util::GameObject {
public:
    Entity(const std::string &imagePath, float zIndex, int maxHp)
        : Util::GameObject(
              std::make_shared<Util::Image>(imagePath), zIndex),
          m_MaxHp(maxHp), m_Hp(maxHp) {}

    virtual ~Entity() = default;

    // Returns false when entity should be removed
    virtual bool Update(float dt) = 0;

    void TakeDamage(int dmg) { m_Hp -= dmg; }
    bool IsAlive() const { return m_Hp > 0; }
    int GetHp() const { return m_Hp; }
    int GetMaxHp() const { return m_MaxHp; }
    void SetMaxHp(int v) { m_MaxHp = v; }
    void SetHp(int hp) { m_Hp = std::clamp(hp, 0, m_MaxHp); }

    glm::vec2 GetPos() const { return m_Transform.translation; }
    void SetPos(glm::vec2 pos) { m_Transform.translation = pos; }

    // Axis-aligned bounding box radius for collision
    float GetCollisionRadius() const { return m_CollisionRadius; }

protected:
    int m_MaxHp;
    int m_Hp;
    float m_CollisionRadius = 24.f;
};
