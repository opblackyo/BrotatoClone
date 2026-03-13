#pragma once
#include "pch.hpp"
#include "Util/Text.hpp"
#include "Util/Image.hpp"
#include "Util/GameObject.hpp"

class Player; // forward-declare to avoid heavy include chain

struct ShopItem {
    std::string name;
    std::string description;
    int         cost;
    std::string effectType;
    int         effectValue;
    std::string iconPath;
};

class ShopScene {
public:
    ShopScene();

    // Register all pre-allocated GameObjects (call once at scene init)
    std::vector<std::shared_ptr<Util::GameObject>> GetObjects() const;

    // Pick 4 random shop cards for the current wave
    void Refresh(int wave, int luck = 0);

    // Open shop UI populated with player stats
    void Show(const Player &player);

    // Process input each frame.
    // Returns: -1=nothing, 0-3=buy card, 4=close/go,
    //          10=reroll, 20-25=sell owned weapon slot 0-5
    int Update(const Player &player);

    void Hide();

    // Remove slot i from the current item list (after purchase)
    void RemoveItem(int idx);
    // Remove slot i from the weapon list (after purchase)
    void RemoveWeaponItem(int idx);

    const std::vector<ShopItem> &GetItems()       const { return m_Items; }
    const std::vector<ShopItem> &GetWeaponItems() const { return m_WeaponItems; }
    bool IsVisible()   const { return m_Visible; }
    int  GetRerollCost() const { return m_RerollCost; }

private:
    // --- helpers ---
    bool IsInCard(glm::vec2 pt, int i) const;
    bool IsInWeaponCard(glm::vec2 pt, int i) const;
    bool IsInOwnedWeaponSlot(glm::vec2 pt, int i) const;
    bool IsInReroll(glm::vec2 pt) const;
    bool IsInGo(glm::vec2 pt) const;
    void RefreshCardDisplay();
    void RefreshWeaponCardDisplay();
    void UpdateStatsPanel(const Player &player);
    void UpdateWeaponSlots(const Player &player);
    void UpdateTopBar(const Player &player);

    // --- layout constants ---
    static constexpr int   NUM_WEAPON_CARDS = 2;   // weapons for sale
    static constexpr int   NUM_CARDS        = 4;   // items for sale
    static constexpr float CARD_W    = 225.f;
    static constexpr float CARD_H    = 265.f;
    static constexpr float CARD_Y    = -30.f;      // single centered row
    static constexpr float WEAPON_Y  = +130.f;     // weapons row (upper half)
    static const    float  CARD_X[NUM_CARDS];           // defined in .cpp
    static const    float  WEAPON_X[NUM_WEAPON_CARDS];  // defined in .cpp

    // --- state ---
    bool              m_Visible    = false;
    std::vector<ShopItem> m_Items;
    std::vector<ShopItem> m_WeaponItems;  // weapons currently in the shop
    int               m_RerollCost = 2;
    int               m_Wave       = 1;

    // --- background ---
    std::shared_ptr<Util::GameObject> m_BgObj;

    // --- top bar ---
    std::shared_ptr<Util::Text>       m_TitleText;
    std::shared_ptr<Util::GameObject> m_TitleObj;
    std::shared_ptr<Util::Text>       m_GoldText;
    std::shared_ptr<Util::GameObject> m_GoldObj;
    std::shared_ptr<Util::GameObject> m_RerollBg;
    std::shared_ptr<Util::Text>       m_RerollText;
    std::shared_ptr<Util::GameObject> m_RerollObj;
    std::shared_ptr<Util::GameObject> m_GoBg;
    std::shared_ptr<Util::Text>       m_GoText;
    std::shared_ptr<Util::GameObject> m_GoObj;

    // --- 4 item cards ---
    struct CardUI {
        std::shared_ptr<Util::GameObject> bg;
        std::shared_ptr<Util::GameObject> icon;   // uses SetDrawable() to swap
        std::shared_ptr<Util::Text>       nameText;
        std::shared_ptr<Util::GameObject> nameObj;
        std::shared_ptr<Util::Text>       typeText; // "Weapon", "Healing", etc.
        std::shared_ptr<Util::GameObject> typeObj;
        std::shared_ptr<Util::Text>       descText;
        std::shared_ptr<Util::GameObject> descObj;
        std::shared_ptr<Util::Text>       costText;
        std::shared_ptr<Util::GameObject> costObj;
    };
    std::array<CardUI, NUM_CARDS>        m_Cards;
    std::array<CardUI, NUM_WEAPON_CARDS> m_WeaponCards;  // weapons for sale

    // --- section headers ---
    std::shared_ptr<Util::Text>       m_WeapSectionTitleText;
    std::shared_ptr<Util::GameObject> m_WeapSectionTitleObj;
    std::shared_ptr<Util::Text>       m_ItemSectionTitleText;
    std::shared_ptr<Util::GameObject> m_ItemSectionTitleObj;

    // --- stats panel ---
    static constexpr int NUM_STATS = 17;
    std::shared_ptr<Util::Text>       m_StatsTitleText;
    std::shared_ptr<Util::GameObject> m_StatsTitleObj;
    std::array<std::shared_ptr<Util::Text>,       NUM_STATS> m_StatTexts;
    std::array<std::shared_ptr<Util::GameObject>, NUM_STATS> m_StatObjs;

    // --- owned weapon slots (shown in stats panel, vertical list) ---
    static constexpr int MAX_WSLOTS = 6;
    std::shared_ptr<Util::Text>       m_WeapHeaderText;
    std::shared_ptr<Util::GameObject> m_WeapHeaderObj;
    struct WeapSlot {
        std::shared_ptr<Util::GameObject> bg;
        std::shared_ptr<Util::Text>       txt;
        std::shared_ptr<Util::GameObject> obj;
    };
    std::array<WeapSlot, MAX_WSLOTS> m_WeapSlots;
};

