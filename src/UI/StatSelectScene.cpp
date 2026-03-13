#include "UI/StatSelectScene.hpp"
#include "Entity/Player.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Color.hpp"
#include <random>
#include <algorithm>

static const std::string FONT_SS = RESOURCE_DIR "/fonts/Inter.ttf";

const float StatSelectScene::CARD_X[3] = {-380.f, 0.f, +380.f};

// Full pool of free stat upgrades offered between waves
static const std::vector<StatUpgrade> UPGRADE_POOL = {
    {"Vitality",     "+10 Max HP",         "hp_up",         10, RESOURCE_DIR "/images/items/icon_hp_up.png"},
    {"Brace",        "+5 Max HP",          "hp_up",          5, RESOURCE_DIR "/images/items/icon_hp_up.png"},
    {"Toughness",    "+2 Armor",           "armor_up",       2, RESOURCE_DIR "/images/items/icon_armor.png"},
    {"Shell",        "+1 Armor",           "armor_up",       1, RESOURCE_DIR "/images/items/icon_armor.png"},
    {"Athleticism",  "+20 Speed",          "speed_up",      20, RESOURCE_DIR "/images/items/icon_speed.png"},
    {"Sprint",       "+10 Speed",          "speed_up",      10, RESOURCE_DIR "/images/items/icon_speed.png"},
    {"Fast Reload",  "+6% Atk Speed",      "attack_speed",   6, RESOURCE_DIR "/images/items/icon_atkspd.png"},
    {"Hair Trigger", "+3% Atk Speed",      "attack_speed",   3, RESOURCE_DIR "/images/items/icon_atkspd.png"},
    {"Focus",        "+5% Crit",           "crit_up",        5, RESOURCE_DIR "/images/items/icon_crit.png"},
    {"Precision",    "+3% Crit",           "crit_up",        3, RESOURCE_DIR "/images/items/icon_crit.png"},
    {"Muscles",      "+10% Melee Dmg",     "melee_dmg",     10, RESOURCE_DIR "/images/items/icon_melee.png"},
    {"Marksmanship", "+10% Ranged Dmg",    "ranged_dmg",    10, RESOURCE_DIR "/images/items/icon_ranged.png"},
    {"Fury",         "+8% All Dmg",        "pct_damage",     8, RESOURCE_DIR "/images/items/icon_pctdmg.png"},
    {"Recovery",     "+0.3 HP/s Regen",    "hp_regen",       3, RESOURCE_DIR "/images/items/icon_hpregen.png"},
    {"Bloodsucker",  "+3% Lifesteal",      "lifesteal",      3, RESOURCE_DIR "/images/items/icon_lifesteal.png"},
    {"Agility",      "+8% Dodge",          "dodge",          8, RESOURCE_DIR "/images/items/icon_dodge.png"},
    {"Long Barrel",  "+15% Range",         "range",         15, RESOURCE_DIR "/images/items/icon_range.png"},
    {"Fortune",      "+2 Luck",            "luck",           2, RESOURCE_DIR "/images/items/icon_luck.png"},
    {"Gathering",    "+10% Harvest",       "harvesting",    10, RESOURCE_DIR "/images/items/icon_harvest.png"},
};

// Helper: make (Text, GameObject) pair, initially invisible
static std::pair<std::shared_ptr<Util::Text>, std::shared_ptr<Util::GameObject>>
MakeSSTxt(const std::string &font, int size, const std::string &init,
          Util::Color col, float z, glm::vec2 pos) {
    auto txt = std::make_shared<Util::Text>(font, size, init, col);
    auto obj = std::make_shared<Util::GameObject>(txt, z);
    obj->m_Transform.translation = pos;
    obj->SetVisible(false);
    return {txt, obj};
}

StatSelectScene::StatSelectScene() {
    // Background
    m_BgObj = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Image>(RESOURCE_DIR "/images/backgrounds/shop_bg.png"), 8.f);
    m_BgObj->m_Transform.scale = {1280.f / 512.f, 720.f / 512.f};
    m_BgObj->SetVisible(false);

    auto [tt, to] = MakeSSTxt(FONT_SS, 30, "CHOOSE AN UPGRADE",
        Util::Color::FromRGB(255, 230, 80), 9.5f, {0.f, 290.f});
    m_TitleText = tt; m_TitleObj = to;

    auto [st, so] = MakeSSTxt(FONT_SS, 20, "Pick 1",
        Util::Color::FromRGB(180, 230, 180), 9.5f, {0.f, 248.f});
    m_SubText = st; m_SubObj = so;

    for (int i = 0; i < NUM_CHOICES; i++) {
        CardUI &c = m_Cards[i];
        float   cx = CARD_X[i];

        c.bg = std::make_shared<Util::GameObject>(
            std::make_shared<Util::Image>(RESOURCE_DIR "/images/ui/hp_bar_bg.png"), 8.5f);
        c.bg->m_Transform.translation = {cx, CARD_Y};
        c.bg->m_Transform.scale       = {1.4f, 6.5f};
        c.bg->SetVisible(false);

        c.icon = std::make_shared<Util::GameObject>(
            std::make_shared<Util::Image>(RESOURCE_DIR "/images/ui/hp_bar_fill.png"), 9.0f);
        c.icon->m_Transform.translation = {cx, CARD_Y + 80.f};
        c.icon->m_Transform.scale       = {0.5f, 0.5f};
        c.icon->SetVisible(false);

        auto [nt, no] = MakeSSTxt(FONT_SS, 20, "Stat",
            Util::Color::FromRGB(255, 230, 130), 9.2f, {cx, CARD_Y + 28.f});
        c.nameText = nt; c.nameObj = no;

        auto [dt, dobj] = MakeSSTxt(FONT_SS, 16, "desc",
            Util::Color::FromRGB(190, 210, 190), 9.2f, {cx, CARD_Y - 12.f});
        c.descText = dt; c.descObj = dobj;

        auto [ht, ho] = MakeSSTxt(FONT_SS, 16, "[1]",
            Util::Color::FromRGB(140, 140, 210), 9.2f, {cx, CARD_Y - 85.f});
        c.hintText = ht; c.hintObj = ho;
    }
}

std::vector<std::shared_ptr<Util::GameObject>> StatSelectScene::GetObjects() const {
    std::vector<std::shared_ptr<Util::GameObject>> out;
    out.push_back(m_BgObj);
    out.push_back(m_TitleObj);
    out.push_back(m_SubObj);
    for (const auto &c : m_Cards) {
        out.push_back(c.bg);
        out.push_back(c.icon);
        out.push_back(c.nameObj);
        out.push_back(c.descObj);
        out.push_back(c.hintObj);
    }
    return out;
}

void StatSelectScene::Refresh() {
    static std::mt19937 rng(std::random_device{}());
    std::vector<StatUpgrade> pool = UPGRADE_POOL;
    std::shuffle(pool.begin(), pool.end(), rng);
    m_Choices.clear();
    for (int i = 0; i < NUM_CHOICES && i < static_cast<int>(pool.size()); i++)
        m_Choices.push_back(pool[i]);

    for (int i = 0; i < NUM_CHOICES; i++) {
        const StatUpgrade &u = m_Choices[i];
        m_Cards[i].nameText->SetText(u.name);
        m_Cards[i].descText->SetText(u.description);
        m_Cards[i].hintText->SetText("[" + std::to_string(i + 1) + "] Click");
        m_Cards[i].icon->SetDrawable(std::make_shared<Util::Image>(u.iconPath));
    }
}

void StatSelectScene::Open(int numPicks) {
    m_PicksRemaining = numPicks;
    m_Visible = true;
    Refresh();

    m_SubText->SetText(numPicks > 1 ? "Pick 2  (Level Up!)" : "Pick 1");
    m_BgObj->SetVisible(true);
    m_TitleObj->SetVisible(true);
    m_SubObj->SetVisible(true);
    for (auto &c : m_Cards) {
        c.bg->SetVisible(true);
        c.icon->SetVisible(true);
        c.nameObj->SetVisible(true);
        c.descObj->SetVisible(true);
        c.hintObj->SetVisible(true);
    }
}

void StatSelectScene::Close() {
    m_Visible = false;
    m_BgObj->SetVisible(false);
    m_TitleObj->SetVisible(false);
    m_SubObj->SetVisible(false);
    for (auto &c : m_Cards) {
        c.bg->SetVisible(false);
        c.icon->SetVisible(false);
        c.nameObj->SetVisible(false);
        c.descObj->SetVisible(false);
        c.hintObj->SetVisible(false);
    }
}

bool StatSelectScene::IsInCard(glm::vec2 pt, int i) const {
    return std::abs(pt.x - CARD_X[i]) < CARD_W * 0.5f &&
           std::abs(pt.y - CARD_Y)    < CARD_H * 0.5f;
}

int StatSelectScene::Update() {
    if (!m_Visible) return -1;

    glm::vec2 mouse = Util::Input::GetCursorPosition();
    bool lmb        = Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB);

    if (lmb) {
        for (int i = 0; i < NUM_CHOICES; i++)
            if (i < static_cast<int>(m_Choices.size()) && IsInCard(mouse, i))
                return i;
    }

    const Util::Keycode KB[3] = {
        Util::Keycode::NUM_1, Util::Keycode::NUM_2, Util::Keycode::NUM_3
    };
    for (int i = 0; i < 3; i++)
        if (Util::Input::IsKeyDown(KB[i]) && i < static_cast<int>(m_Choices.size()))
            return i;

    return -1;
}

std::string StatSelectScene::ApplyUpgrade(int idx, Player &player) {
    if (idx < 0 || idx >= static_cast<int>(m_Choices.size())) return "";
    const StatUpgrade &u = m_Choices[idx];
    std::string name = u.name;

    if (u.effectType == "hp_up") {
        player.SetMaxHp(player.GetMaxHp() + u.effectValue);
        player.Heal(u.effectValue);
    } else if (u.effectType == "armor_up")
        player.AddArmor(u.effectValue);
    else if (u.effectType == "speed_up")
        player.AddSpeed(static_cast<float>(u.effectValue));
    else if (u.effectType == "attack_speed")
        player.AddAttackSpeed(u.effectValue / 100.f);
    else if (u.effectType == "crit_up")
        player.AddCrit(u.effectValue / 100.f);
    else if (u.effectType == "melee_dmg")
        player.AddMeleeDamage(u.effectValue / 100.f);
    else if (u.effectType == "ranged_dmg")
        player.AddRangedDamage(u.effectValue / 100.f);
    else if (u.effectType == "pct_damage")
        player.AddPctDamage(u.effectValue / 100.f);
    else if (u.effectType == "hp_regen")
        player.AddHpRegen(u.effectValue / 10.f);
    else if (u.effectType == "lifesteal")
        player.AddLifesteal(u.effectValue / 100.f);
    else if (u.effectType == "dodge")
        player.AddDodge(u.effectValue / 100.f);
    else if (u.effectType == "range")
        player.AddRangeMult(u.effectValue / 100.f);
    else if (u.effectType == "luck")
        player.AddLuck(u.effectValue);
    else if (u.effectType == "harvesting")
        player.AddHarvesting(u.effectValue / 100.f);

    m_PicksRemaining--;
    if (m_PicksRemaining > 0) {
        Refresh();
        m_SubText->SetText("Pick " + std::to_string(m_PicksRemaining) + " more");
    }
    return name;
}
