#include "UI/HUD.hpp"
#include "Util/Color.hpp"
#include <sstream>

// Using PTSD_Config window size for positioning
static constexpr float WIN_W = 1280.f;
static constexpr float WIN_H = 720.f;

HUD::HUD() {
    // HP bar background
    m_HpBarBg = std::make_shared<Util::Image>(
        RESOURCE_DIR "/images/ui/hp_bar_bg.png");
    m_HpBgObj = std::make_shared<Util::GameObject>(m_HpBarBg, 9.f);
    m_HpBgObj->m_Transform.translation = {-WIN_W / 2.f + 160.f, WIN_H / 2.f - 30.f};
    m_HpBgObj->m_Transform.scale = {0.5f, 0.4f};

    // HP bar fill (we scale X to represent remaining HP fraction)
    m_HpBarFill = std::make_shared<Util::Image>(
        RESOURCE_DIR "/images/ui/hp_bar_fill.png");
    m_HpFillObj = std::make_shared<Util::GameObject>(m_HpBarFill, 9.1f);
    m_HpFillObj->m_Transform.translation = {-WIN_W / 2.f + 160.f, WIN_H / 2.f - 30.f};
    m_HpFillObj->m_Transform.scale = {0.5f, 0.4f};

    // Text labels
    m_HpText = std::make_shared<Util::Text>(
        RESOURCE_DIR "/fonts/Inter.ttf", 20,
        "HP: 100/100", Util::Color::FromRGB(255, 80, 80));
    m_HpTextObj = std::make_shared<Util::GameObject>(m_HpText, 9.5f);
    m_HpTextObj->m_Transform.translation = {-WIN_W / 2.f + 55.f, WIN_H / 2.f - 30.f};

    m_GoldText = std::make_shared<Util::Text>(
        RESOURCE_DIR "/fonts/Inter.ttf", 20,
        "Gold: 0", Util::Color::FromRGB(255, 220, 50));
    m_GoldTextObj = std::make_shared<Util::GameObject>(m_GoldText, 9.5f);
    m_GoldTextObj->m_Transform.translation = {WIN_W / 2.f - 80.f, WIN_H / 2.f - 30.f};

    m_WaveText = std::make_shared<Util::Text>(
        RESOURCE_DIR "/fonts/Inter.ttf", 22,
        "Wave 1/5", Util::Color::FromRGB(200, 200, 255));
    m_WaveTextObj = std::make_shared<Util::GameObject>(m_WaveText, 9.5f);
    m_WaveTextObj->m_Transform.translation = {0.f, WIN_H / 2.f - 30.f};

    m_TimerText = std::make_shared<Util::Text>(
        RESOURCE_DIR "/fonts/Inter.ttf", 20,
        "30.0", Util::Color::FromRGB(255, 255, 255));
    m_TimerTextObj = std::make_shared<Util::GameObject>(m_TimerText, 9.5f);
    m_TimerTextObj->m_Transform.translation = {0.f, WIN_H / 2.f - 55.f};

    // --- Boss HP bar (hidden by default, shown on boss wave) ---
    m_BossBarBg = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Image>(RESOURCE_DIR "/images/ui/hp_bar_bg.png"), 9.2f);
    m_BossBarBg->m_Transform.translation = {0.f, -(WIN_H / 2.f - 45.f)};
    m_BossBarBg->m_Transform.scale       = {1.5f, 0.4f};
    m_BossBarBg->SetVisible(false);

    m_BossBarFill = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Image>(RESOURCE_DIR "/images/ui/hp_bar_fill.png"), 9.3f);
    m_BossBarFill->m_Transform.translation = {0.f, -(WIN_H / 2.f - 45.f)};
    m_BossBarFill->m_Transform.scale       = {1.5f, 0.4f};
    m_BossBarFill->SetVisible(false);

    m_BossNameText = std::make_shared<Util::Text>(
        RESOURCE_DIR "/fonts/Inter.ttf", 18, "BOSS",
        Util::Color::FromRGB(255, 80, 80));
    m_BossNameObj = std::make_shared<Util::GameObject>(m_BossNameText, 9.5f);
    m_BossNameObj->m_Transform.translation = {0.f, -(WIN_H / 2.f - 22.f)};
    m_BossNameObj->SetVisible(false);

    m_BossHpText = std::make_shared<Util::Text>(
        RESOURCE_DIR "/fonts/Inter.ttf", 15, "9999 / 9999",
        Util::Color::FromRGB(255, 200, 200));
    m_BossHpObj = std::make_shared<Util::GameObject>(m_BossHpText, 9.5f);
    m_BossHpObj->m_Transform.translation = {0.f, -(WIN_H / 2.f - 45.f)};
    m_BossHpObj->SetVisible(false);
}

void HUD::Update(int hp, int maxHp, int gold, int wave, int totalWaves,
                 float waveTimer) {
    // Scale HP bar fill proportionally
    float hpFrac = (maxHp > 0) ? static_cast<float>(hp) / maxHp : 0.f;
    m_HpFillObj->m_Transform.scale.x = 0.5f * hpFrac;

    m_HpText->SetText("HP: " + std::to_string(hp) + "/" +
                      std::to_string(maxHp));
    m_GoldText->SetText("$" + std::to_string(gold));
    m_WaveText->SetText("Wave " + std::to_string(wave) + "/" +
                        std::to_string(totalWaves));

    std::ostringstream ss;
    ss << std::fixed;
    ss.precision(1);
    ss << waveTimer;
    m_TimerText->SetText(ss.str() + "s");
}

void HUD::SetGameplayVisible(bool visible) {
    m_HpBgObj->SetVisible(visible);
    m_HpFillObj->SetVisible(visible);
    m_HpTextObj->SetVisible(visible);
    m_GoldTextObj->SetVisible(visible);
    m_WaveTextObj->SetVisible(visible);
    m_TimerTextObj->SetVisible(visible);

    if (!visible)
        HideBossBar();
}

void HUD::Draw() {
    m_HpBgObj->Draw();
    m_HpFillObj->Draw();
    m_HpTextObj->Draw();
    m_GoldTextObj->Draw();
    m_WaveTextObj->Draw();
    m_TimerTextObj->Draw();
}

void HUD::ShowBossBar(const std::string &bossName) {
    m_BossNameText->SetText(bossName);
    m_BossBarBg->SetVisible(true);
    m_BossBarFill->SetVisible(true);
    m_BossNameObj->SetVisible(true);
    m_BossHpObj->SetVisible(true);
}

void HUD::HideBossBar() {
    m_BossBarBg->SetVisible(false);
    m_BossBarFill->SetVisible(false);
    m_BossNameObj->SetVisible(false);
    m_BossHpObj->SetVisible(false);
}

void HUD::UpdateBossBar(int hp, int maxHp) {
    float frac = maxHp > 0 ? static_cast<float>(hp) / maxHp : 0.f;
    m_BossBarFill->m_Transform.scale.x = 1.5f * frac;
    m_BossHpText->SetText(std::to_string(hp) + " / " + std::to_string(maxHp));
}

std::vector<std::shared_ptr<Util::GameObject>> HUD::GetObjects() const {
    return {m_HpBgObj,    m_HpFillObj,   m_HpTextObj,
            m_GoldTextObj, m_WaveTextObj, m_TimerTextObj,
            m_BossBarBg,  m_BossBarFill, m_BossNameObj, m_BossHpObj};
}
