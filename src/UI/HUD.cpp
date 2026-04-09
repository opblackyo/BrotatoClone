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

    // m_XpBarBg = std::make_shared<Util::Image>(RESOURCE_DIR "/images/ui/hp_bar_bg.png");
    // m_XpBgObj = std::make_shared<Util::GameObject>(m_XpBarBg, 9.f);
    // m_XpBgObj->m_Transform.translation = {0.f, WIN_H / 2.f - 10.f}; // 靠上
    // m_XpBgObj->m_Transform.scale = {2.0f, 0.15f}; // 拉長變細
    //
    // m_XpBarFill = std::make_shared<Util::Image>(RESOURCE_DIR "/images/ui/hp_bar_fill.png");
    // m_XpFillObj = std::make_shared<Util::GameObject>(m_XpBarFill, 9.1f);
    // m_XpFillObj->m_Transform.translation = {0.f, WIN_H / 2.f - 10.f};
    // m_XpFillObj->m_Transform.scale = {2.0f, 0.15f};
    //
    // m_XpText = std::make_shared<Util::Text>(RESOURCE_DIR "/fonts/Inter.ttf", 16, "LVL 1", Util::Color::FromRGB(100, 255, 100));
    // m_XpTextObj = std::make_shared<Util::GameObject>(m_XpText, 9.5f);
    // m_XpTextObj->m_Transform.translation = {0.f, WIN_H / 2.f - 10.f};
    // --- XP Bar (對齊左上角的 HP 血條下方) ---
    m_XpBarBg = std::make_shared<Util::Image>(RESOURCE_DIR "/images/ui/hp_bar_bg.png");
    m_XpBgObj = std::make_shared<Util::GameObject>(m_XpBarBg, 9.f);
    m_XpBgObj->m_Transform.translation = {-WIN_W / 2.f + 160.f, WIN_H / 2.f - 55.f};
    m_XpBgObj->m_Transform.scale = {0.5f, 0.15f};

    m_XpBarFill = std::make_shared<Util::Image>(RESOURCE_DIR "/images/ui/hp_bar_fill.png");
    m_XpFillObj = std::make_shared<Util::GameObject>(m_XpBarFill, 9.1f);
    m_XpFillObj->m_Transform.translation = {-WIN_W / 2.f + 160.f, WIN_H / 2.f - 55.f};
    m_XpFillObj->m_Transform.scale = {0.5f, 0.15f};

    m_XpText = std::make_shared<Util::Text>(RESOURCE_DIR "/fonts/Inter.ttf", 16, "LVL 1", Util::Color::FromRGB(100, 255, 100));
    m_XpTextObj = std::make_shared<Util::GameObject>(m_XpText, 9.5f);
    m_XpTextObj->m_Transform.translation = {-WIN_W / 2.f + 55.f, WIN_H / 2.f - 55.f};
}

void HUD::Update(int hp, int maxHp, int gold, int wave, int totalWaves,
                 float waveTimer,   int xp, int maxXp, int level) {
    // --- 更新 XP 條 ---
    float xpFrac = (maxXp > 0) ? static_cast<float>(xp) / maxXp : 0.f;

    // 1. 設定基礎參數
    float baseScaleX = 0.5f;
    float imageWidth = 300.f; // ⚠️請把這裡換成 hp_bar_fill.png 的實際像素寬度！

    // 2. 計算最大寬度與當前寬度
    float maxBarWidth = imageWidth * baseScaleX;
    float currentBarWidth = maxBarWidth * xpFrac;

    // 3. 更新縮放
    m_XpFillObj->m_Transform.scale.x = baseScaleX * xpFrac;

    // 4. 更新座標：把中心點往左移，抵銷縮放造成的左邊界位移
    float baseX = -WIN_W / 2.f + 160.f; // 這是你設定的初始 X 座標
    m_XpFillObj->m_Transform.translation.x = baseX - (maxBarWidth - currentBarWidth) / 2.0f;

    m_XpText->SetText("LVL " + std::to_string(level));
    // float xpFrac = (maxXp > 0) ? static_cast<float>(xp) / maxXp : 0.f;
    // m_XpFillObj->m_Transform.scale.x = 2.0f * xpFrac; // 注意這裡的 2.0f 要跟上面的 scale.x 一致
    // m_XpText->SetText("LVL " + std::to_string(level));

    // 👉 把這裡的 2.0f 改成 0.5f，這樣填滿時的寬度才會剛好對齊底框
    m_XpFillObj->m_Transform.scale.x = 0.5f * xpFrac;

    m_XpText->SetText("LVL " + std::to_string(level));

    // Scale HP bar fill proportionally
    // --- 更新 HP 條 ---
    float hpFrac = (maxHp > 0) ? static_cast<float>(hp) / maxHp : 0.f;
    float currentHpWidth = maxBarWidth * hpFrac; // 共用上面的 maxBarWidth 變數

    m_HpFillObj->m_Transform.scale.x = baseScaleX * hpFrac;
    m_HpFillObj->m_Transform.translation.x = baseX - (maxBarWidth - currentHpWidth) / 2.0f;
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

    m_XpBgObj->SetVisible(visible);
    m_XpFillObj->SetVisible(visible);
    m_XpTextObj->SetVisible(visible);

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

    m_XpBgObj -> Draw();
    m_XpFillObj -> Draw();
    m_XpTextObj -> Draw();
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
            m_BossBarBg,  m_BossBarFill, m_BossNameObj, m_BossHpObj,
            m_XpBgObj, m_XpFillObj, m_XpTextObj};
}
