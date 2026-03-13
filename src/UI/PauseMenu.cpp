#include "UI/PauseMenu.hpp"
#include "Entity/Player.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Color.hpp"
#include <sstream>
#include <iomanip>

static const std::string FONT_PM = RESOURCE_DIR "/fonts/Inter.ttf";

static std::pair<std::shared_ptr<Util::Text>, std::shared_ptr<Util::GameObject>>
MakePMTxt(const std::string &font, int size, const std::string &init,
          Util::Color col, float z, glm::vec2 pos) {
    auto txt = std::make_shared<Util::Text>(font, size, init, col);
    auto obj = std::make_shared<Util::GameObject>(txt, z);
    obj->m_Transform.translation = pos;
    obj->SetVisible(false);
    return {txt, obj};
}

PauseMenu::PauseMenu() {
    // Full-screen overlay (reuse shop background)
    m_BgObj = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Image>(RESOURCE_DIR "/images/backgrounds/shop_bg.png"), 9.f);
    m_BgObj->m_Transform.scale = {1280.f / 512.f, 720.f / 512.f};
    m_BgObj->SetVisible(false);

    auto [tt, to] = MakePMTxt(FONT_PM, 34, "  PAUSED  ",
        Util::Color::FromRGB(255, 120, 80), 10.f, {0.f, 310.f});
    m_TitleText = tt; m_TitleObj = to;

    // Stats title (left column at x=-420)
    auto [stt, sto] = MakePMTxt(FONT_PM, 18, "── STATS ──",
        Util::Color::FromRGB(180, 180, 255), 10.f, {-420.f, 265.f});
    m_StatsTitleText = stt; m_StatsTitleObj = sto;

    const std::array<std::string, NUM_STATS> SINIT = {
        "Wave: 1",        "Max HP: 100",   "HP Regen: 0/s",
        "Lifesteal: 0%",  "%Damage: +0%",  "Melee: x1.00",
        "Ranged: x1.00",  "Elemental: 0",  "AtkSpd: x1.00",
        "Crit: 5%",       "Engineering: 0","Range: x1.00",
        "Armor: 0",       "Dodge: 0%",     "Speed: 220",
        "Luck: 0",        "Harvest: x1.00"
    };
    for (int i = 0; i < NUM_STATS; i++) {
        auto [st, so] = MakePMTxt(FONT_PM, 14, SINIT[i],
            Util::Color::FromRGB(200, 225, 200), 10.f,
            {-420.f, 238.f - i * 26.f});
        m_StatTexts[i] = st; m_StatObjs[i] = so;
    }

    // Items title (right column at x=+60)
    auto [itt, ito] = MakePMTxt(FONT_PM, 18, "── ITEMS ──",
        Util::Color::FromRGB(255, 200, 80), 10.f, {+60.f, 265.f});
    m_ItemsTitleText = itt; m_ItemsTitleObj = ito;

    for (int i = 0; i < MAX_ITEM_LINES; i++) {
        auto [it, io] = MakePMTxt(FONT_PM, 14, " ",
            Util::Color::FromRGB(220, 200, 160), 10.f,
            {+60.f, 238.f - i * 26.f});
        m_ItemTexts[i] = it; m_ItemObjs[i] = io;
    }

    // RESUME button
    m_ResumeBg = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Image>(RESOURCE_DIR "/images/ui/hp_bar_bg.png"), 9.5f);
    m_ResumeBg->m_Transform.translation = {-130.f, -295.f};
    m_ResumeBg->m_Transform.scale       = {1.4f, 1.5f};
    m_ResumeBg->SetVisible(false);
    auto [rt, ro] = MakePMTxt(FONT_PM, 22, "RESUME  (ESC)",
        Util::Color::FromRGB(100, 220, 100), 10.f, {-130.f, -295.f});
    m_ResumeText = rt; m_ResumeObj = ro;

    // QUIT button
    m_QuitBg = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Image>(RESOURCE_DIR "/images/ui/hp_bar_bg.png"), 9.5f);
    m_QuitBg->m_Transform.translation = {+180.f, -295.f};
    m_QuitBg->m_Transform.scale       = {0.9f, 1.5f};
    m_QuitBg->SetVisible(false);
    auto [qt, qo] = MakePMTxt(FONT_PM, 22, "QUIT",
        Util::Color::FromRGB(240, 70, 70), 10.f, {+180.f, -295.f});
    m_QuitText = qt; m_QuitObj = qo;
}

std::vector<std::shared_ptr<Util::GameObject>> PauseMenu::GetObjects() const {
    std::vector<std::shared_ptr<Util::GameObject>> out;
    out.push_back(m_BgObj);
    out.push_back(m_TitleObj);
    out.push_back(m_StatsTitleObj);
    for (const auto &o : m_StatObjs) out.push_back(o);
    out.push_back(m_ItemsTitleObj);
    for (const auto &o : m_ItemObjs) out.push_back(o);
    out.push_back(m_ResumeBg);
    out.push_back(m_ResumeObj);
    out.push_back(m_QuitBg);
    out.push_back(m_QuitObj);
    return out;
}

void PauseMenu::UpdateContent(const Player &player,
                              const std::vector<std::string> &ownedItems,
                              int wave) {
    auto f2 = [](float v) {
        std::ostringstream ss;
        ss << std::fixed << std::setprecision(2) << v;
        return ss.str();
    };
    auto ip = [](float v) { return std::to_string(static_cast<int>(v * 100)); };

    m_StatTexts[0]->SetText("Wave: "         + std::to_string(wave));
    m_StatTexts[1]->SetText("Max HP: "        + std::to_string(player.GetMaxHp()));
    m_StatTexts[2]->SetText("HP Regen: "      + f2(player.GetHpRegen()) + "/s");
    m_StatTexts[3]->SetText("Lifesteal: "     + ip(player.GetLifesteal()) + "%");
    m_StatTexts[4]->SetText("%Damage: +"      + ip(player.GetPctDamage()) + "%");
    m_StatTexts[5]->SetText("Melee: x"        + f2(player.GetMeleeDamageMult()));
    m_StatTexts[6]->SetText("Ranged: x"       + f2(player.GetRangedDamageMult()));
    m_StatTexts[7]->SetText("Elemental: 0");
    m_StatTexts[8]->SetText("AtkSpd: x"       + f2(player.GetAttackSpeedMult()));
    m_StatTexts[9]->SetText("Crit: "          + ip(player.GetCritChance()) + "%");
    m_StatTexts[10]->SetText("Engineering: "  + std::to_string(player.GetEngineering()));
    m_StatTexts[11]->SetText("Range: x"       + f2(player.GetRangeMult()));
    m_StatTexts[12]->SetText("Armor: "        + std::to_string(player.GetArmor()));
    m_StatTexts[13]->SetText("Dodge: "        + ip(player.GetDodge()) + "%");
    m_StatTexts[14]->SetText("Speed: "        + std::to_string(static_cast<int>(player.GetSpeed())));
    m_StatTexts[15]->SetText("Luck: "         + std::to_string(player.GetLuck()));
    m_StatTexts[16]->SetText("Harvest: x"     + f2(player.GetHarvesting()));

    // Items list (show up to MAX_ITEM_LINES)
    for (int i = 0; i < MAX_ITEM_LINES; i++) {
        if (i < static_cast<int>(ownedItems.size())) {
            m_ItemTexts[i]->SetText("・" + ownedItems[i]);
            m_ItemObjs[i]->SetVisible(true);
        } else {
            m_ItemTexts[i]->SetText(" ");
            m_ItemObjs[i]->SetVisible(false);
        }
    }
}

void PauseMenu::Show(const Player &player,
                     const std::vector<std::string> &ownedItems,
                     int wave) {
    m_Visible = true;
    m_BgObj->SetVisible(true);
    m_TitleObj->SetVisible(true);
    m_StatsTitleObj->SetVisible(true);
    for (auto &o : m_StatObjs) o->SetVisible(true);
    m_ItemsTitleObj->SetVisible(true);
    m_ResumeBg->SetVisible(true);
    m_ResumeObj->SetVisible(true);
    m_QuitBg->SetVisible(true);
    m_QuitObj->SetVisible(true);
    UpdateContent(player, ownedItems, wave);
}

void PauseMenu::Hide() {
    m_Visible = false;
    m_BgObj->SetVisible(false);
    m_TitleObj->SetVisible(false);
    m_StatsTitleObj->SetVisible(false);
    for (auto &o : m_StatObjs) o->SetVisible(false);
    m_ItemsTitleObj->SetVisible(false);
    for (auto &o : m_ItemObjs) o->SetVisible(false);
    m_ResumeBg->SetVisible(false);
    m_ResumeObj->SetVisible(false);
    m_QuitBg->SetVisible(false);
    m_QuitObj->SetVisible(false);
}

bool PauseMenu::IsInResume(glm::vec2 pt) const {
    return std::abs(pt.x - (-130.f)) < 110.f && std::abs(pt.y - (-295.f)) < 26.f;
}
bool PauseMenu::IsInQuit(glm::vec2 pt) const {
    return std::abs(pt.x - 180.f) < 65.f && std::abs(pt.y - (-295.f)) < 26.f;
}

int PauseMenu::Update() {
    if (!m_Visible) return 0;

    if (Util::Input::IsKeyDown(Util::Keycode::ESCAPE)) return 1; // resume

    glm::vec2 mouse = Util::Input::GetCursorPosition();
    bool lmb        = Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB);
    if (lmb) {
        if (IsInResume(mouse)) return 1;
        if (IsInQuit(mouse))   return 2;
    }
    return 0;
}
