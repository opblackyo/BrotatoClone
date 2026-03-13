#pragma once
#include "pch.hpp"
#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include <unordered_set>

// A projectile in the world (player-fired or enemy-fired)
class Projectile : public Util::GameObject {
public:
    Projectile(const std::string &imagePath, glm::vec2 pos, glm::vec2 dir,
               float speed, int damage, bool isPlayerOwned,
               float lifetime = 3.5f);

    // Returns false when should be removed
    bool Update(float dt);

    bool IsPlayerOwned() const { return m_PlayerOwned; }
    int GetDamage() const { return m_Damage; }
    glm::vec2 GetPos() const { return m_Transform.translation; }
    float GetRadius() const { return 8.f; }
    bool IsExpired() const { return m_Lifetime <= 0.f; }
    bool IsCrit() const { return m_IsCrit; }
    void SetCrit(bool v) { m_IsCrit = v; }
    void SetPierce(int extraTargets) {
        const int maxHits = extraTargets + 1;
        m_MaxHits = (maxHits > 1) ? maxHits : 1;
    }
    bool CanHitTarget(const void *target) const {
        return m_HitTargets.find(target) == m_HitTargets.end();
    }
    bool RegisterHit(const void *target) {
        m_HitTargets.insert(target);
        ++m_HitsDone;
        return m_HitsDone < m_MaxHits;
    }

private:
    glm::vec2 m_Velocity;
    int m_Damage;
    bool m_PlayerOwned;
    float m_Lifetime = 3.5f;
    bool m_IsCrit    = false;
    int m_MaxHits    = 1;
    int m_HitsDone   = 0;
    std::unordered_set<const void *> m_HitTargets;
};
