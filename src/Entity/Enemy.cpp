#include "Entity/Enemy.hpp"
#include "Util/Time.hpp"

// ---- Enemy base ----
bool Enemy::Update(float dt) {
    if (!IsAlive())
        return false;

    m_AttackTimer += dt;

    // Hit flash: pulse scale
    if (m_HitFlashTimer > 0.f) {
        m_HitFlashTimer -= dt;
        float t = std::max(0.f, m_HitFlashTimer / 0.14f); // 1 → 0
        float s = 1.f + 0.45f * t;
        m_Transform.scale.x = (m_Transform.scale.x < 0.f ? -1.f : 1.f)
                              * std::abs(m_BaseScale.x) * s;
        m_Transform.scale.y = m_BaseScale.y * s;
    } else {
        // Restore base scale (flip preserved below)
        m_Transform.scale.y = m_BaseScale.y;
        if (std::abs(m_Transform.scale.x) != m_BaseScale.x)
            m_Transform.scale.x = (m_Transform.scale.x < 0.f ? -1.f : 1.f)
                                  * m_BaseScale.x;
    }

    // Move toward target
    glm::vec2 dir = m_Target - m_Transform.translation;
    float dist = glm::length(dir);
    if (dist > 2.f) {
        m_Transform.translation += (dir / dist) * m_Speed * dt;
        // Flip sprite based on direction
        m_Transform.scale.x = (dir.x < 0) ? -std::abs(m_Transform.scale.x)
                                           : std::abs(m_Transform.scale.x);
    }
    return true;
}

bool Enemy::IsAtTarget() const {
    float dist = glm::length(m_Target - m_Transform.translation);
    return dist < m_CollisionRadius + 20.f;
}

// ---- Chaser ----
Chaser::Chaser(glm::vec2 spawnPos)
    : Enemy(RESOURCE_DIR "/images/enemies/chaser.png",
        1, 260.f, 1, 1) {
    m_Transform.translation = spawnPos;
    m_Transform.scale       = glm::vec2(0.55f, 0.55f);
    m_BaseScale             = glm::vec2(0.55f, 0.55f);
    m_CollisionRadius       = 20.f;
    m_AttackCooldown        = 0.65f;
}

// ---- Bruiser ----
Bruiser::Bruiser(glm::vec2 spawnPos)
    : Enemy(RESOURCE_DIR "/images/enemies/bruiser.png",
        20, 190.f, 2, 3) {
    m_Transform.translation = spawnPos;
    m_Transform.scale       = glm::vec2(0.65f, 0.65f);
    m_BaseScale             = glm::vec2(0.65f, 0.65f);
    m_CollisionRadius       = 28.f;
    m_AttackCooldown        = 1.0f;
}

// ---- Spitter ----
Spitter::Spitter(glm::vec2 spawnPos)
    : Enemy(RESOURCE_DIR "/images/enemies/spitter.png",
        8, 160.f, 1, 1) {
    m_Transform.translation = spawnPos;
    m_Transform.scale       = glm::vec2(0.55f, 0.55f);
    m_BaseScale             = glm::vec2(0.55f, 0.55f);
    m_CollisionRadius       = 22.f;
}

bool Spitter::Update(float dt) {
    if (!IsAlive())
        return false;

    m_AttackTimer += dt;
    m_ShootTimer  += dt;

    // Hit flash (reuse base logic)
    if (m_HitFlashTimer > 0.f) {
        m_HitFlashTimer -= dt;
        float t = std::max(0.f, m_HitFlashTimer / 0.14f);
        float s = 1.f + 0.45f * t;
        m_Transform.scale = m_BaseScale * s;
    } else {
        m_Transform.scale = m_BaseScale;
    }

    float dist = glm::length(m_Target - m_Transform.translation);

    // Only move closer if too far; keep distance for ranged attack
    if (dist > m_ShootRange * 0.6f) {
        glm::vec2 dir = m_Target - m_Transform.translation;
        float len = glm::length(dir);
        if (len > 0.f) {
            m_Transform.translation += (dir / len) * m_Speed * dt;
            m_Transform.scale.x = (dir.x < 0)
                                       ? -std::abs(m_Transform.scale.x)
                                       : std::abs(m_Transform.scale.x);
        }
    }
    return true;
}

// ---- Guardian ----
Guardian::Guardian(glm::vec2 spawnPos)
    : Enemy(RESOURCE_DIR "/images/enemies/bruiser.png",
        30, 150.f, 2, 3) {
    m_Transform.translation = spawnPos;
    m_Transform.scale       = glm::vec2(0.85f, 0.85f);
    m_BaseScale             = glm::vec2(0.85f, 0.85f);
    m_CollisionRadius       = 30.f;
    m_AttackCooldown        = 1.1f;
}

// ---- Bomber ----
Bomber::Bomber(glm::vec2 spawnPos)
    : Enemy(RESOURCE_DIR "/images/enemies/chaser.png",
        4, 300.f, 2, 1) {
    m_Transform.translation = spawnPos;
    m_Transform.scale       = glm::vec2(0.38f, 0.38f);
    m_BaseScale             = glm::vec2(0.38f, 0.38f);
    m_CollisionRadius       = 14.f;
    m_AttackCooldown        = 0.f; // explode on first touch
}

// ---- Boss ----
Boss::Boss(glm::vec2 spawnPos, int wave)
    : Enemy(RESOURCE_DIR "/images/enemies/bruiser.png",
            600 + wave * 40, 90.f, 4, 100) {
    m_Transform.translation = spawnPos;
    m_Transform.scale       = glm::vec2(1.5f, 1.5f);
    m_BaseScale             = glm::vec2(1.5f, 1.5f);
    m_CollisionRadius       = 50.f;
    m_AttackCooldown        = 1.4f;
}

bool Boss::Update(float dt) {
    if (!IsAlive()) return false;

    m_AttackTimer += dt;
    m_ShootTimer  += dt;

    // Phase 2 at 50% HP: faster movement + faster shooting
    if (m_Hp < m_MaxHp / 2 && m_Phase == 1) {
        m_Phase         = 2;
        m_ShootCooldown = 0.7f;
        m_Speed         = 120.f;
        m_AttackCooldown= 1.2f;
    }

    // Hit flash
    if (m_HitFlashTimer > 0.f) {
        m_HitFlashTimer -= dt;
        float t = std::max(0.f, m_HitFlashTimer / 0.14f);
        float s = 1.f + 0.45f * t;
        m_Transform.scale = m_BaseScale * s;
    } else {
        m_Transform.scale = m_BaseScale;
    }

    // Move toward target (keep some distance for pattern if phase 2)
    glm::vec2 dir = m_Target - m_Transform.translation;
    float dist    = glm::length(dir);
    float stopDist = (m_Phase == 2) ? 180.f : 60.f;
    if (dist > stopDist && dist > 0.f) {
        m_Transform.translation += (dir / dist) * m_Speed * dt;
        m_Transform.scale.x = (dir.x < 0) ? -std::abs(m_Transform.scale.x)
                                           :  std::abs(m_Transform.scale.x);
    }
    return true;
}
