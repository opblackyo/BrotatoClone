#include "Weapon/Projectile.hpp"
#include <cmath>

Projectile::Projectile(const std::string &imagePath, glm::vec2 pos,
                       glm::vec2 dir, float speed, int damage,
                       bool isPlayerOwned, float lifetime)
    : Util::GameObject(std::make_shared<Util::Image>(imagePath), 5.f),
      m_Damage(damage), m_PlayerOwned(isPlayerOwned), m_Lifetime(lifetime) {
    m_Transform.translation = pos;
    float len = glm::length(dir);
    if (len > 0.f) {
        m_Velocity = (dir / len) * speed;
        // Rotate sprite so its length axis points along travel direction
        m_Transform.rotation = atan2f(dir.y, dir.x);
    }
    if (isPlayerOwned) {
        // Player projectiles: short tracer style by default.
        m_Transform.scale = glm::vec2(0.90f, 0.10f);
    } else {
        // 敵人子彈：圓球，不旋轉
        m_Transform.scale    = glm::vec2(0.55f, 0.55f);
        m_Transform.rotation = 0.f;
    }
}

bool Projectile::Update(float dt) {
    m_Lifetime -= dt;
    m_Transform.translation += m_Velocity * dt;
    return !IsExpired();
}
