#pragma once
#include "pch.hpp"
#include "Util/Text.hpp"
#include "Util/GameObject.hpp"
#include "Util/Image.hpp"

// Simple HUD overlay: HP bar rendered via Text labels + bars
class HUD {
public:
    HUD();

    void Update(int hp, int maxHp, int gold, int wave, int totalWaves,
                float waveTimer);

    // Show/hide regular gameplay HUD elements (HP/gold/wave/timer).
    void SetGameplayVisible(bool visible);

    // Boss HP bar
    void ShowBossBar(const std::string &bossName);
    void HideBossBar();
    void UpdateBossBar(int hp, int maxHp);

    // Called each frame to draw all HUD elements
    void Draw();

    // Renderable objects to add to Renderer  
    std::vector<std::shared_ptr<Util::GameObject>> GetObjects() const;

private:
    // Labels
    std::shared_ptr<Util::Text> m_HpText;
    std::shared_ptr<Util::Text> m_GoldText;
    std::shared_ptr<Util::Text> m_WaveText;
    std::shared_ptr<Util::Text> m_TimerText;

    // HP bar background and fill (colored rectangles via Image)
    std::shared_ptr<Util::Image> m_HpBarBg;
    std::shared_ptr<Util::Image> m_HpBarFill;

    // GameObjects wrapping the above
    std::shared_ptr<Util::GameObject> m_HpBgObj;
    std::shared_ptr<Util::GameObject> m_HpFillObj;
    std::shared_ptr<Util::GameObject> m_HpTextObj;
    std::shared_ptr<Util::GameObject> m_GoldTextObj;
    std::shared_ptr<Util::GameObject> m_WaveTextObj;
    std::shared_ptr<Util::GameObject> m_TimerTextObj;

    // Boss HP bar (shown only on boss wave)
    std::shared_ptr<Util::Text>       m_BossNameText;
    std::shared_ptr<Util::GameObject> m_BossNameObj;
    std::shared_ptr<Util::GameObject> m_BossBarBg;
    std::shared_ptr<Util::GameObject> m_BossBarFill;
    std::shared_ptr<Util::Text>       m_BossHpText;
    std::shared_ptr<Util::GameObject> m_BossHpObj;
};
