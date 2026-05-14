#include "UI/ShopScene.hpp"
#include "Core/GameConfig.hpp"
#include "Entity/Player.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Color.hpp"
#include <algorithm>
#include <filesystem>
#include <iomanip>
#include <random>
#include <sstream>

static const std::string FONT   = RESOURCE_DIR "/fonts/Inter.ttf";
static constexpr float   WIN_W  = 1280.f;
static constexpr float   WIN_H  = 720.f;
static constexpr float   STATS_X = +455.f;

static int ScaleShopCost(int originalCost) {
    return std::max(3, (originalCost * 3 + 5) / 10);
}

static bool IsWeaponEffect(const std::string &effectType) {
    return effectType == "add_pistol" || effectType == "add_shotgun" ||
           effectType == "add_smg" || effectType == "add_knife";
}

static Util::Color WeaponTierColor(int tier) {
    switch (tier) {
    case 1: return Util::Color::FromRGB(225, 225, 225);
    case 2: return Util::Color::FromRGB(95, 220, 125);
    default:return Util::Color::FromRGB(95, 170, 255);
    }
}

const float ShopScene::CARD_X[4]   = {-435.f, -200.f, +35.f, +270.f};
const float ShopScene::WEAPON_X[2] = {-310.f,  -60.f};

// ── Full item catalog (Brotato-faithful, no weapons – weapons come from stat-select) ──
// Columns: name, description, cost(=value), effectType, effectValue, iconPath
// effectValue units:
//   hp/armor/luck/engineering/speed: direct integer
//   %-based stats (crit, dodge, lifesteal, ranged_dmg, melee_dmg, pct_damage,
//                  attack_speed, harvesting, range): integer that will be /100
//                  before applying as float fraction
//   hp_regen: integer * 0.1 = HP/s (20 → 2.0 HP/s, 5 → 0.5 HP/s)
//
// tier 0 = common ($8-30), tier 1 = uncommon ($25-60),
// tier 2 = rare ($60-85), tier 3 = epic ($90-120)
static const std::vector<ShopItem> ALL_ITEMS = {
    // ── Tier 0: Common ───────────────────────────────────────────────────────
    // name                description            cost  effectType     val  icon
    {"Alien Worm",    "+5 Max HP\n+2 HP/s",        15, "hp_up",        5,  RESOURCE_DIR "/images/items/alien_worm.png"},
    {"Bat",           "+2% Lifesteal",             20,  "lifesteal",    2,  RESOURCE_DIR "/images/items/bat.png"},
    {"Beanie",        "+20 Speed",                 20,  "speed_up",    20,  RESOURCE_DIR "/images/items/beanie.png"},
    {"Book",          "+1 Engineering",             8,  "engineering",  1,  RESOURCE_DIR "/images/items/book.png"},
    {"Boiling Water", "+10 Elem. Dmg",             30,  "elem_dmg",    10,  RESOURCE_DIR "/images/items/boiling_water.png"},
    {"Boxing Glove",  "+10% Melee Dmg",            18,  "melee_dmg",   10,  RESOURCE_DIR "/images/items/boxing_glove.png"},
    {"Coffee",        "+6% Atk Speed",             20,  "attack_speed", 6,  RESOURCE_DIR "/images/items/coffee.png"},
    {"Defect. Ster.", "+10 Max HP\n+10% Melee",    20,  "hp_up",       10,  RESOURCE_DIR "/images/items/defective_steroids.png"},
    {"Fertilizer",    "+10% Harvest",              15,  "harvesting",  10,  RESOURCE_DIR "/images/items/fertilizer.png"},
    {"Fresh Meat",    "+3% Lifesteal",             25,  "lifesteal",    3,  RESOURCE_DIR "/images/items/fresh_meat.png"},
    {"Glasses",       "+20% Range",                20,  "range",       20,  RESOURCE_DIR "/images/items/glasses.png"},
    {"Helmet",        "+1 Armor",                  15,  "armor_up",     1,  RESOURCE_DIR "/images/items/helmet.png"},
    {"Head Injury",   "+5% All Damage",            25,  "pct_damage",   5,  RESOURCE_DIR "/images/items/icon_pctdmg.png"},
    {"Injection",     "+5% All Damage",            20,  "pct_damage",   5,  RESOURCE_DIR "/images/items/injection.png"},
    {"Insanity",      "+5% Crit Chance",           20,  "crit_up",      5,  RESOURCE_DIR "/images/items/insanity.png"},
    {"Lens",          "+5% Ranged Dmg",            20,  "ranged_dmg",   5,  RESOURCE_DIR "/images/items/lens.png"},
    {"Lost Duck",     "+3 Luck",                   25,  "luck",         3,  RESOURCE_DIR "/images/items/icon_luck.png"},
    {"Lumberjack Sh.","Exploration",               15,  "harvesting",   5,  RESOURCE_DIR "/images/items/lumberjack_shirt.png"},
    {"Mushroom",      "+0.3 HP/s",                 25,  "hp_regen",     3,  RESOURCE_DIR "/images/items/mushroom.png"},
    {"Pencil",        "+1 Engineering",            15,  "engineering",  1,  RESOURCE_DIR "/images/items/pencil.png"},
    {"Terr. Onion",   "+10 Speed",                 15,  "speed_up",    10,  RESOURCE_DIR "/images/items/terrified_onion.png"},
    // ── Tier 1: Uncommon ─────────────────────────────────────────────────────
    {"Adrenaline",    "+10% Dodge",                60,  "dodge",       10,  RESOURCE_DIR "/images/items/adrenaline.png"},
    {"Bandana",       "+20% Ranged Dmg",           75,  "ranged_dmg",  20,  RESOURCE_DIR "/images/items/bandana.png"},
    {"Blood Leech",   "+3% Lifesteal\n+0.2 HP/s",  45,  "lifesteal",    3,  RESOURCE_DIR "/images/items/blood_leech.png"},
    {"Blood Donat.",  "+20% Harvest",              50,  "harvesting",  20,  RESOURCE_DIR "/images/items/blood_donation.png"},
    {"Insanity+",     "+8% Crit Chance",           50,  "crit_up",      8,  RESOURCE_DIR "/images/items/insanity.png"},
    {"Leather Vest",  "+2 Armor\n+8% Dodge",       45,  "armor_up",     2,  RESOURCE_DIR "/images/items/leather_vest.png"},
    {"Medal",         "+20 Max HP\n+3 Armor",      55,  "hp_up",       20,  RESOURCE_DIR "/images/items/medal.png"},
    {"Metal Plate",   "+2 Armor",                  40,  "armor_up",     2,  RESOURCE_DIR "/images/items/metal_plate.png"},
    {"Pile of Books", "+5% Crit\n+2 Eng.",         70,  "crit_up",      5,  RESOURCE_DIR "/images/items/pile_of_books.png"},
    {"Riposte",       "+8% Dodge\n+8% Melee",      40,  "dodge",        8,  RESOURCE_DIR "/images/items/riposte.png"},
    {"Scope",         "+2% Ranged\n+25% Range",    48,  "ranged_dmg",   2,  RESOURCE_DIR "/images/items/scope.png"},
    {"Script",        "+3% Atk Speed",             60,  "attack_speed", 3,  RESOURCE_DIR "/images/items/icon_atkspd.png"},
    {"Sunglasses",    "+8% Crit Chance",           50,  "crit_up",      8,  RESOURCE_DIR "/images/items/sunglasses.png"},
    {"Turret",        "+1 Engineering",            15,  "engineering",  2,  RESOURCE_DIR "/images/items/turret.png"},
    // ── Tier 2: Rare ─────────────────────────────────────────────────────────
    {"Barricade",     "+5 Armor",                  75,  "armor_up",     5,  RESOURCE_DIR "/images/items/barricade.png"},
    {"Clover",        "+20 Luck\n+6% Dodge",       65,  "luck",        20,  RESOURCE_DIR "/images/items/clover.png"},
    {"Crown",         "+20% Harvest",              70,  "harvesting",  20,  RESOURCE_DIR "/images/items/crown.png"},
    {"Fairy",         "+1.0 HP/s",                 85,  "hp_regen",    10,  RESOURCE_DIR "/images/items/fairy.png"},
    {"Ghost Outfit",  "+20% Dodge",                80,  "dodge",       20,  RESOURCE_DIR "/images/items/ghost_outfit.png"},
    {"Giant Belt",    "+15% Crit Chance",          110, "crit_up",     15,  RESOURCE_DIR "/images/items/giant_belt.png"},
    {"Lucky Charm",   "+15 Luck",                  75,  "luck",        15,  RESOURCE_DIR "/images/items/lucky_charm.png"},
    {"Medikit+",      "+1.0 HP/s",                 95,  "hp_regen",    10,  RESOURCE_DIR "/images/items/medikit.png"},
    {"Spicy Sauce",   "+10 Max HP\n+8% All Dmg",   40,  "pct_damage",   8,  RESOURCE_DIR "/images/items/spicy_sauce.png"},
    {"Stone Skin",    "+5 Armor\n+30 Max HP",      85,  "armor_up",     5,  RESOURCE_DIR "/images/items/stone_skin.png"},
    {"Warrior Helm.", "+4 Armor\n+30 Max HP",      80,  "armor_up",     4,  RESOURCE_DIR "/images/items/warrior_helmet.png"},
    {"Wings",         "+30 Speed\n+20% Range",     85,  "speed_up",    30,  RESOURCE_DIR "/images/items/wings.png"},
    // ── Tier 3: Epic ─────────────────────────────────────────────────────────
    {"Big Arms",      "+30% Melee\n+20% Ranged",  105,  "melee_dmg",   30,  RESOURCE_DIR "/images/items/big_arms.png"},
    {"Cape",          "+5% Lifesteal\n+20% Dodge",110,  "lifesteal",    5,  RESOURCE_DIR "/images/items/cape.png"},
    {"Lucky Coin",    "+15% Crit\n+15 Luck",      105,  "crit_up",     15,  RESOURCE_DIR "/images/items/lucky_coin.png"},
    {"Medikit",       "+2.0 HP/s",                 95,  "hp_regen",    20,  RESOURCE_DIR "/images/items/medikit.png"},
    {"Ricochet",      "+30% Ranged Dmg",          110,  "ranged_dmg",  30,  RESOURCE_DIR "/images/items/ricochet.png"},
    {"Robot Arm",    "+30% Melee\n+3 Eng.",        100,  "melee_dmg",   30,  RESOURCE_DIR "/images/items/robot_arm.png"},
};

// ── Weapon catalog shown in shop (4 available weapons, 2 picked each wave) ──
static const std::vector<ShopItem> ALL_WEAPONS = {
    //  name        description                       cost  effectType     val  iconPath
    {"Pistol",   "Tier I\nRapid: 5 dmg  0.24s",        10, "add_pistol",   0,  RESOURCE_DIR "/images/weapons/pistol.png"},
    {"Shotgun",  "Tier I\nSpread: 9x5  1.2s",          20, "add_shotgun",  0,  RESOURCE_DIR "/images/weapons/shotgun.png"},
    {"SMG",      "Tier I\nBurst: 5 dmg  0.22s",        40, "add_smg",      0,  RESOURCE_DIR "/images/weapons/smg.png"},
    {"Knife",    "Tier I\nSlash: 22 dmg  0.5s",        15, "add_knife",    0,  RESOURCE_DIR "/images/weapons/knife.png"},
};

// ── Helper to make (Text, GameObject) pairs ───────────────────────────────────
static std::pair<std::shared_ptr<Util::Text>, std::shared_ptr<Util::GameObject>>
MakeTxtObj(const std::string &font, int size, const std::string &init,
           Util::Color col, float z, glm::vec2 pos) {
    auto txt = std::make_shared<Util::Text>(font, size, init, col);
    auto obj = std::make_shared<Util::GameObject>(txt, z);
    obj->m_Transform.translation = pos;
    obj->SetVisible(false);
    return {txt, obj};
}

static std::shared_ptr<Util::Image> MakeSafeIconDrawable(const std::string &path) {
    std::error_code ec;
    if (!path.empty() && std::filesystem::exists(path, ec))
        return std::make_shared<Util::Image>(path);
    return std::make_shared<Util::Image>(RESOURCE_DIR "/images/ui/hp_bar_fill.png");
}

// ── Constructor ───────────────────────────────────────────────────────────────
ShopScene::ShopScene() {
    // Background
    m_BgObj = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Image>(RESOURCE_DIR "/images/backgrounds/shop_bg.png"), 8.f);
    m_BgObj->m_Transform.scale = {WIN_W / 1920.f, WIN_H / 1080.f};
    m_BgObj->SetVisible(false);

    // Title
    auto [tt, to] = MakeTxtObj(FONT, 26, "Shop (Wave 1)",
        Util::Color::FromRGB(255, 230, 80), 9.5f, {-430.f, 295.f});
    m_TitleText = tt; m_TitleObj = to;

    // Gold label
    auto [gt, go] = MakeTxtObj(FONT, 24, "Gold: 0",
        Util::Color::FromRGB(255, 215, 0), 9.5f, {-120.f, 295.f});
    m_GoldText = gt; m_GoldObj = go;

    // REROLL button
    m_RerollBg = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Image>(RESOURCE_DIR "/images/ui/hp_bar_bg.png"), 9.0f);
    m_RerollBg->m_Transform.translation = {+90.f, 295.f};
    m_RerollBg->m_Transform.scale       = {1.1f, 1.3f};
    m_RerollBg->SetVisible(false);
    auto [rt, ro] = MakeTxtObj(FONT, 18, "REROLL -5g",
        Util::Color::FromRGB(160, 240, 160), 9.6f, {+90.f, 295.f});
    m_RerollText = rt; m_RerollObj = ro;

    // GO button
    m_GoBg = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Image>(RESOURCE_DIR "/images/ui/hp_bar_bg.png"), 9.0f);
    m_GoBg->m_Transform.translation = {+250.f, 295.f};
    m_GoBg->m_Transform.scale       = {0.85f, 1.3f};
    m_GoBg->SetVisible(false);
    auto [xt, xo] = MakeTxtObj(FONT, 20, "GO >",
        Util::Color::FromRGB(255, 255, 80), 9.6f, {+250.f, 295.f});
    m_GoText = xt; m_GoObj = xo;

    // ── 4 Item cards ──────────────────────────────────────────────────────────
    for (int i = 0; i < NUM_CARDS; i++) {
        CardUI &c  = m_Cards[i];
        float  cx  = CARD_X[i];

        c.bg = std::make_shared<Util::GameObject>(
            std::make_shared<Util::Image>(RESOURCE_DIR "/images/ui/hp_bar_bg.png"), 8.5f);
        c.bg->m_Transform.translation = {cx, CARD_Y};
        c.bg->m_Transform.scale       = {1.4f, 7.2f};
        c.bg->SetVisible(false);

        c.icon = std::make_shared<Util::GameObject>(
            std::make_shared<Util::Image>(RESOURCE_DIR "/images/ui/hp_bar_fill.png"), 9.0f);
        c.icon->m_Transform.translation = {cx, CARD_Y + 95.f};
        c.icon->m_Transform.scale       = {0.5f, 0.5f};
        c.icon->SetVisible(false);

        auto [nt, no] = MakeTxtObj(FONT, 20, "Item",
            Util::Color::FromRGB(255, 230, 130), 9.2f, {cx, CARD_Y + 45.f});
        c.nameText = nt; c.nameObj = no;

        auto [pt, po] = MakeTxtObj(FONT, 14, "Misc",
            Util::Color::FromRGB(140, 200, 140), 9.2f, {cx, CARD_Y + 15.f});
        c.typeText = pt; c.typeObj = po;

        auto [dt, dobj] = MakeTxtObj(FONT, 15, "desc",
            Util::Color::FromRGB(190, 190, 210), 9.2f, {cx, CARD_Y - 20.f});
        c.descText = dt; c.descObj = dobj;

        auto [ct, co] = MakeTxtObj(FONT, 20, "$0",
            Util::Color::FromRGB(255, 215, 0), 9.2f, {cx, CARD_Y - 92.f});
        c.costText = ct; c.costObj = co;
    }

    // ── 2 Weapon cards for sale ────────────────────────────────────────────────
    for (int i = 0; i < NUM_WEAPON_CARDS; i++) {
        CardUI &c  = m_WeaponCards[i];
        float  cx  = WEAPON_X[i];

        c.bg = std::make_shared<Util::GameObject>(
            std::make_shared<Util::Image>(RESOURCE_DIR "/images/ui/hp_bar_bg.png"), 8.5f);
        c.bg->m_Transform.translation = {cx, WEAPON_Y};
        c.bg->m_Transform.scale       = {1.4f, 7.2f};
        c.bg->SetVisible(false);

        c.icon = std::make_shared<Util::GameObject>(
            std::make_shared<Util::Image>(RESOURCE_DIR "/images/ui/hp_bar_fill.png"), 9.0f);
        c.icon->m_Transform.translation = {cx, WEAPON_Y + 95.f};
        c.icon->m_Transform.scale       = {0.55f, 0.55f};
        c.icon->SetVisible(false);

        auto [nt, no] = MakeTxtObj(FONT, 20, "Weapon",
            Util::Color::FromRGB(255, 200, 100), 9.2f, {cx, WEAPON_Y + 45.f});
        c.nameText = nt; c.nameObj = no;

        auto [pt, po] = MakeTxtObj(FONT, 14, "Weapon",
            Util::Color::FromRGB(255, 160, 80), 9.2f, {cx, WEAPON_Y + 15.f});
        c.typeText = pt; c.typeObj = po;

        auto [dt, dobj] = MakeTxtObj(FONT, 15, "desc",
            Util::Color::FromRGB(190, 190, 210), 9.2f, {cx, WEAPON_Y - 20.f});
        c.descText = dt; c.descObj = dobj;

        auto [ct, co] = MakeTxtObj(FONT, 20, "$0",
            Util::Color::FromRGB(255, 215, 0), 9.2f, {cx, WEAPON_Y - 92.f});
        c.costText = ct; c.costObj = co;
    }

    // ── Section headers ────────────────────────────────────────────────────────
    auto [wst, wso] = MakeTxtObj(FONT, 18, "── WEAPONS ──",
        Util::Color::FromRGB(255, 190, 80), 9.2f, {-185.f, +255.f});
    m_WeapSectionTitleText = wst; m_WeapSectionTitleObj = wso;

    auto [ist, iso] = MakeTxtObj(FONT, 18, "── ITEMS ──",
        Util::Color::FromRGB(160, 240, 160), 9.2f, {-80.f, +20.f});
    m_ItemSectionTitleText = ist; m_ItemSectionTitleObj = iso;

    // ── Stats panel ───────────────────────────────────────────────────────────
    auto [sht, sho] = MakeTxtObj(FONT, 20, "STATS",
        Util::Color::FromRGB(180, 180, 255), 9.2f, {STATS_X, +265.f});
    m_StatsTitleText = sht; m_StatsTitleObj = sho;

    const std::array<std::string, NUM_STATS> SINIT = {
        "Wave: 1",        "Max HP: " + std::to_string(GameConfig::PLAYER_MAX_HP),   "HP Regen: 0/s",
        "Lifesteal: 0%",  "%Damage: +0%",  "Melee: x1.00",
        "Ranged: x1.00",  "Elemental: 0",  "AtkSpd: x1.00",
        "Crit: 5%",       "Engineering: 0","Range: x1.00",
        "Armor: 0",       "Dodge: 0%",     "Speed: 220",
        "Luck: 0",        "Harvest: x1.00"
    };
    for (int i = 0; i < NUM_STATS; i++) {
        auto [st, so] = MakeTxtObj(FONT, 15, SINIT[i],
            Util::Color::FromRGB(200, 225, 200), 9.2f,
            {STATS_X, +228.f - i * 26.f});
        m_StatTexts[i] = st; m_StatObjs[i] = so;
    }

    // ── Owned weapon slots (vertical list in stats panel area) ────────────────
    auto [wht, who] = MakeTxtObj(FONT, 16, "Weapons (0/6):",
        Util::Color::FromRGB(180, 180, 255), 9.2f, {STATS_X, -222.f});
    m_WeapHeaderText = wht; m_WeapHeaderObj = who;

    for (int i = 0; i < MAX_WSLOTS; i++) {
        WeapSlot &s = m_WeapSlots[i];
        s.bg = std::make_shared<Util::GameObject>(
            std::make_shared<Util::Image>(RESOURCE_DIR "/images/ui/hp_bar_bg.png"), 9.0f);
        s.bg->m_Transform.translation = {STATS_X + 5.f, -248.f - i * 17.f};
        s.bg->m_Transform.scale       = {0.90f, 0.52f};
        s.bg->SetVisible(false);

        auto [st, so] = MakeTxtObj(FONT, 12, "Empty",
            Util::Color::FromRGB(100, 100, 100), 9.4f, {STATS_X + 5.f, -248.f - i * 17.f});
        s.txt = st; s.obj = so;
    }
}

// ── GetObjects ────────────────────────────────────────────────────────────────
std::vector<std::shared_ptr<Util::GameObject>> ShopScene::GetObjects() const {
    std::vector<std::shared_ptr<Util::GameObject>> out;
    out.push_back(m_BgObj);
    out.push_back(m_TitleObj);
    out.push_back(m_GoldObj);
    out.push_back(m_RerollBg);
    out.push_back(m_RerollObj);
    out.push_back(m_GoBg);
    out.push_back(m_GoObj);
    out.push_back(m_WeapSectionTitleObj);
    out.push_back(m_ItemSectionTitleObj);
    for (const auto &c : m_WeaponCards) {
        out.push_back(c.bg);
        out.push_back(c.icon);
        out.push_back(c.nameObj);
        out.push_back(c.typeObj);
        out.push_back(c.descObj);
        out.push_back(c.costObj);
    }
    for (const auto &c : m_Cards) {
        out.push_back(c.bg);
        out.push_back(c.icon);
        out.push_back(c.nameObj);
        out.push_back(c.typeObj);
        out.push_back(c.descObj);
        out.push_back(c.costObj);
    }
    out.push_back(m_StatsTitleObj);
    for (const auto &o : m_StatObjs) out.push_back(o);
    out.push_back(m_WeapHeaderObj);
    for (const auto &s : m_WeapSlots) {
        out.push_back(s.bg);
        out.push_back(s.obj);
    }
    return out;
}

// ── Refresh ───────────────────────────────────────────────────────────────────
void ShopScene::Refresh(int wave, int luck) {
    m_Wave = wave;
    std::mt19937 rng(static_cast<unsigned>(wave * 13337 + std::rand())); // NOLINT

    // Items: pick 3 and guarantee 1 weapon in the same 4-card row.
    std::vector<ShopItem> pool = ALL_ITEMS;
    for (auto &it : pool)
        it.cost = ScaleShopCost(it.cost);
    if (luck >= 3) {
        int minCost = std::max(0, (luck - 2) * 2);
        std::vector<ShopItem> good, rest;
        for (const auto &it : pool)
            (it.cost >= minCost ? good : rest).push_back(it);
        std::shuffle(good.begin(), good.end(), rng);
        std::shuffle(rest.begin(), rest.end(), rng);
        pool = good;
        for (const auto &b : rest) pool.push_back(b);
    } else {
        std::shuffle(pool.begin(), pool.end(), rng);
    }

    std::vector<ShopItem> wpPool = ALL_WEAPONS;
    for (auto &it : wpPool)
        it.cost = ScaleShopCost(it.cost);
    std::shuffle(wpPool.begin(), wpPool.end(), rng);

    m_Items.clear();
    for (int i = 0; i < NUM_CARDS - 1 && i < static_cast<int>(pool.size()); i++)
        m_Items.push_back(pool[i]);
    if (!wpPool.empty())
        m_Items.push_back(wpPool.front());
    std::shuffle(m_Items.begin(), m_Items.end(), rng);

    m_WeaponItems.clear();

    if (m_Visible) {
        RefreshCardDisplay();
        RefreshWeaponCardDisplay();
    }
}

void ShopScene::RemoveItem(int idx) {
    if (idx >= 0 && idx < static_cast<int>(m_Items.size()))
        m_Items.erase(m_Items.begin() + idx);
}

void ShopScene::RemoveWeaponItem(int idx) {
    if (idx >= 0 && idx < static_cast<int>(m_WeaponItems.size()))
        m_WeaponItems.erase(m_WeaponItems.begin() + idx);
}

// ── RefreshCardDisplay ────────────────────────────────────────────────────────
void ShopScene::RefreshCardDisplay() {
    auto typeTag = [](const std::string &t) -> std::string {
        if (IsWeaponEffect(t))                           return "Weapon";
        if (t == "hp_up" || t == "hp_regen")            return "Healing";
        if (t == "armor_up" || t == "dodge")             return "Defense";
        if (t == "ranged_dmg" || t == "melee_dmg" ||
            t == "pct_damage" || t == "elem_dmg" ||
            t == "crit_up")                              return "Damage";
        if (t == "speed_up" || t == "attack_speed" ||
            t == "range")                                return "Attack";
        if (t == "lifesteal")                            return "Lifesteal";
        if (t == "harvesting" || t == "luck")            return "Economy";
        if (t == "engineering")                          return "Tech";
        return "Misc";
    };
    for (int i = 0; i < NUM_CARDS; i++) {
        CardUI &c = m_Cards[i];
        if (i < static_cast<int>(m_Items.size())) {
            const ShopItem &it = m_Items[i];
            const bool isWeapon = IsWeaponEffect(it.effectType);
            c.nameText->SetText(isWeapon ? it.name + " I" : it.name);
            c.nameText->SetColor(isWeapon ? WeaponTierColor(1)
                                           : Util::Color::FromRGB(255, 230, 130));
            c.typeText->SetText(isWeapon ? "Weapon - Common" : typeTag(it.effectType));
            c.typeText->SetColor(isWeapon ? WeaponTierColor(1)
                                           : Util::Color::FromRGB(140, 200, 140));
            c.descText->SetText(it.description);
            c.costText->SetText("$" + std::to_string(it.cost) +
                                "  [" + std::to_string(i + 1) + "]");
            c.icon->SetDrawable(MakeSafeIconDrawable(it.iconPath));
            c.bg->SetVisible(true);
            c.icon->SetVisible(true);
            c.nameObj->SetVisible(true);
            c.typeObj->SetVisible(true);
            c.descObj->SetVisible(true);
            c.costObj->SetVisible(true);
        } else {
            c.bg->SetVisible(false);
            c.icon->SetVisible(false);
            c.nameObj->SetVisible(false);
            c.typeObj->SetVisible(false);
            c.descObj->SetVisible(false);
            c.costObj->SetVisible(false);
        }
    }
}

// ── RefreshWeaponCardDisplay ──────────────────────────────────────────────────
void ShopScene::RefreshWeaponCardDisplay() {
    for (int i = 0; i < NUM_WEAPON_CARDS; i++) {
        CardUI &c = m_WeaponCards[i];
        if (i < static_cast<int>(m_WeaponItems.size())) {
            const ShopItem &it = m_WeaponItems[i];
            c.nameText->SetText(it.name + " I");
            c.nameText->SetColor(WeaponTierColor(1));
            c.typeText->SetText("Weapon - Common");
            c.typeText->SetColor(WeaponTierColor(1));
            c.descText->SetText(it.description);
            c.costText->SetText("$" + std::to_string(it.cost) +
                                "  [Q" + std::to_string(i + 1) + "]");
            c.icon->SetDrawable(MakeSafeIconDrawable(it.iconPath));
            c.bg->SetVisible(true);
            c.icon->SetVisible(true);
            c.nameObj->SetVisible(true);
            c.typeObj->SetVisible(true);
            c.descObj->SetVisible(true);
            c.costObj->SetVisible(true);
        } else {
            c.bg->SetVisible(false);
            c.icon->SetVisible(false);
            c.nameObj->SetVisible(false);
            c.typeObj->SetVisible(false);
            c.descObj->SetVisible(false);
            c.costObj->SetVisible(false);
        }
    }
}

// ── UpdateStatsPanel ──────────────────────────────────────────────────────────
void ShopScene::UpdateStatsPanel(const Player &player) {
    auto f2 = [](float v) {
        std::ostringstream ss;
        ss << std::fixed << std::setprecision(2) << v;
        return ss.str();
    };
    auto ip = [](float v) { return std::to_string(static_cast<int>(v * 100)); };
    m_StatTexts[0]->SetText("Wave: " + std::to_string(m_Wave));
    m_StatTexts[1]->SetText("Max HP: " + std::to_string(player.GetMaxHp()));
    m_StatTexts[2]->SetText("HP Regen: " + f2(player.GetHpRegen()) + "/s");
    m_StatTexts[3]->SetText("Lifesteal: " + ip(player.GetLifesteal()) + "%");
    m_StatTexts[4]->SetText("%Damage: +" + ip(player.GetPctDamage()) + "%");
    m_StatTexts[5]->SetText("Melee: x" + f2(player.GetMeleeDamageMult()));
    m_StatTexts[6]->SetText("Ranged: x" + f2(player.GetRangedDamageMult()));
    m_StatTexts[7]->SetText("Elemental: 0");
    m_StatTexts[8]->SetText("AtkSpd: x" + f2(player.GetAttackSpeedMult()));
    m_StatTexts[9]->SetText("Crit: " + ip(player.GetCritChance()) + "%");
    m_StatTexts[10]->SetText("Engineering: " + std::to_string(player.GetEngineering()));
    m_StatTexts[11]->SetText("Range: x" + f2(player.GetRangeMult()));
    m_StatTexts[12]->SetText("Armor: " + std::to_string(player.GetArmor()));
    m_StatTexts[13]->SetText("Dodge: " + ip(player.GetDodge()) + "%");
    m_StatTexts[14]->SetText("Speed: " + std::to_string(static_cast<int>(player.GetSpeed())));
    m_StatTexts[15]->SetText("Luck: " + std::to_string(player.GetLuck()));
    m_StatTexts[16]->SetText("Harvest: x" + f2(player.GetHarvesting()));
}

// ── UpdateWeaponSlots ─────────────────────────────────────────────────────────
void ShopScene::UpdateWeaponSlots(const Player &player) {
    const auto &weps = player.GetWeapons();
    m_WeapHeaderText->SetText("Weapons (" + std::to_string(weps.size()) + "/6)  [Click to Sell]");
    const bool canSell = player.CanSellWeapon();
    for (int i = 0; i < MAX_WSLOTS; i++) {
        if (i < static_cast<int>(weps.size())) {
            m_WeapSlots[i].txt->SetColor(WeaponTierColor(weps[i]->GetLevel()));
            if (canSell) {
                const int refund = player.GetWeaponSellValue(i);
                m_WeapSlots[i].txt->SetText(weps[i]->GetName() + "  [Sell $" + std::to_string(refund) + "]");
            } else {
                m_WeapSlots[i].txt->SetText(weps[i]->GetName() + "  [Locked]");
            }
        } else {
            m_WeapSlots[i].txt->SetColor(Util::Color::FromRGB(100, 100, 100));
            m_WeapSlots[i].txt->SetText("Empty");
        }
    }
}

// ── UpdateTopBar ──────────────────────────────────────────────────────────────
void ShopScene::UpdateTopBar(const Player &player) {
    m_TitleText->SetText("Shop (Wave " + std::to_string(m_Wave) + ")");
    m_GoldText->SetText("Gold: " + std::to_string(player.GetGold()));
    m_RerollText->SetText("REROLL -" + std::to_string(m_RerollCost) + "g");
}

// ── Show ──────────────────────────────────────────────────────────────────────
void ShopScene::Show(const Player &player) {
    m_Visible = true;
    m_BgObj->SetVisible(true);
    m_TitleObj->SetVisible(true);
    m_GoldObj->SetVisible(true);
    m_RerollBg->SetVisible(true);
    m_RerollObj->SetVisible(true);
    m_GoBg->SetVisible(true);
    m_GoObj->SetVisible(true);
    m_WeapSectionTitleObj->SetVisible(false);
    m_ItemSectionTitleObj->SetVisible(false);
    m_StatsTitleObj->SetVisible(true);
    for (auto &o : m_StatObjs) o->SetVisible(true);
    m_WeapHeaderObj->SetVisible(true);
    for (auto &s : m_WeapSlots) { s.bg->SetVisible(true); s.obj->SetVisible(true); }

    RefreshCardDisplay();
    RefreshWeaponCardDisplay();
    UpdateStatsPanel(player);
    UpdateWeaponSlots(player);
    UpdateTopBar(player);
}

// ── Hide ──────────────────────────────────────────────────────────────────────
void ShopScene::Hide() {
    m_Visible = false;
    auto hide = [](std::shared_ptr<Util::GameObject> &o){ o->SetVisible(false); };
    hide(m_BgObj); hide(m_TitleObj); hide(m_GoldObj);
    hide(m_RerollBg); hide(m_RerollObj); hide(m_GoBg); hide(m_GoObj);
    hide(m_WeapSectionTitleObj); hide(m_ItemSectionTitleObj);
    hide(m_StatsTitleObj);
    for (auto &o : m_StatObjs) hide(o);
    hide(m_WeapHeaderObj);
    for (auto &s : m_WeapSlots) { hide(s.bg); hide(s.obj); }
    for (auto &c : m_Cards) {
        hide(c.bg); hide(c.icon); hide(c.nameObj);
        hide(c.typeObj); hide(c.descObj); hide(c.costObj);
    }
    for (auto &c : m_WeaponCards) {
        hide(c.bg); hide(c.icon); hide(c.nameObj);
        hide(c.typeObj); hide(c.descObj); hide(c.costObj);
    }
}

// ── Hit-test helpers ──────────────────────────────────────────────────────────
bool ShopScene::IsInCard(glm::vec2 pt, int i) const {
    return std::abs(pt.x - CARD_X[i]) < CARD_W * 0.5f &&
           std::abs(pt.y - CARD_Y)    < CARD_H * 0.5f;
}
bool ShopScene::IsInWeaponCard(glm::vec2 pt, int i) const {
    return std::abs(pt.x - WEAPON_X[i]) < CARD_W * 0.5f &&
           std::abs(pt.y - WEAPON_Y)    < CARD_H * 0.5f;
}
bool ShopScene::IsInReroll(glm::vec2 pt) const {
    return std::abs(pt.x - 90.f) < 75.f && std::abs(pt.y - 295.f) < 24.f;
}
bool ShopScene::IsInGo(glm::vec2 pt) const {
    return std::abs(pt.x - 250.f) < 60.f && std::abs(pt.y - 295.f) < 24.f;
}
bool ShopScene::IsInOwnedWeaponSlot(glm::vec2 pt, int i) const {
    if (i < 0 || i >= MAX_WSLOTS) return false;
    const float cx = STATS_X + 5.f;
    const float cy = -248.f - i * 17.f;
    return std::abs(pt.x - cx) < 95.f && std::abs(pt.y - cy) < 13.f;
}

// ── Update ────────────────────────────────────────────────────────────────────
int ShopScene::Update(const Player &player) {
    if (!m_Visible) return -1;

    m_GoldText->SetText("Gold: " + std::to_string(player.GetGold()));

    glm::vec2 mouse = Util::Input::GetCursorPosition();
    bool lmb        = Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB);

    if (lmb) {
        // Check the four shop cards.
        for (int i = 0; i < NUM_CARDS; i++)
            if (i < static_cast<int>(m_Items.size()) && IsInCard(mouse, i))
                return i;

        // Click owned weapon slot to sell and free space.
        for (int i = 0; i < player.GetWeaponCount() && i < MAX_WSLOTS; ++i) {
            if (IsInOwnedWeaponSlot(mouse, i))
                return 20 + i;
        }

        if (IsInReroll(mouse)) return 10;
        if (IsInGo(mouse))     return 4;
    }

    // Number keys 1-4 = buy item 0-3
    const Util::Keycode KB[4] = {
        Util::Keycode::NUM_1, Util::Keycode::NUM_2,
        Util::Keycode::NUM_3, Util::Keycode::NUM_4};
    for (int i = 0; i < 4; i++)
        if (Util::Input::IsKeyDown(KB[i]) &&
            i < static_cast<int>(m_Items.size()))
            return i;

    if (Util::Input::IsKeyDown(Util::Keycode::NUM_0) ||
        Util::Input::IsKeyDown(Util::Keycode::RETURN))
        return 4;

    return -1;
}
