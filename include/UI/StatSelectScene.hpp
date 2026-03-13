#pragma once
#include "pch.hpp"
#include "Util/Text.hpp"
#include "Util/Image.hpp"
#include "Util/GameObject.hpp"

class Player; // forward declaration

// A free stat upgrade offered after each wave
struct StatUpgrade {
    std::string name;
    std::string description;
    std::string effectType;
    int         effectValue;
    std::string iconPath;
};

class StatSelectScene {
public:
    StatSelectScene();

    // Register all pre-allocated GameObjects (call once at scene init)
    std::vector<std::shared_ptr<Util::GameObject>> GetObjects() const;

    // Open with numPicks=1 normally, =2 on level-up wave
    void Open(int numPicks);
    void Close();

    bool IsVisible()  const { return m_Visible; }
    bool IsDone()     const { return m_PicksRemaining == 0; }

    // Returns 0-2 when a card is selected, -1 if nothing yet
    int Update();

    // Apply selected upgrade to player; returns the upgrade name
    std::string ApplyUpgrade(int idx, Player &player);

private:
    void Refresh();  // Pick 3 new random upgrades and update card display
    bool IsInCard(glm::vec2 pt, int i) const;

    static constexpr int   NUM_CHOICES = 3;
    static constexpr float CARD_W      = 230.f;
    static constexpr float CARD_H      = 240.f;
    static constexpr float CARD_Y      = 0.f;
    static const    float  CARD_X[NUM_CHOICES];

    bool m_Visible         = false;
    int  m_PicksRemaining  = 0;
    std::vector<StatUpgrade> m_Choices; // current 3 choices

    std::shared_ptr<Util::GameObject> m_BgObj;

    std::shared_ptr<Util::Text>       m_TitleText;
    std::shared_ptr<Util::GameObject> m_TitleObj;
    std::shared_ptr<Util::Text>       m_SubText;
    std::shared_ptr<Util::GameObject> m_SubObj;

    struct CardUI {
        std::shared_ptr<Util::GameObject> bg;
        std::shared_ptr<Util::GameObject> icon;
        std::shared_ptr<Util::Text>       nameText;
        std::shared_ptr<Util::GameObject> nameObj;
        std::shared_ptr<Util::Text>       descText;
        std::shared_ptr<Util::GameObject> descObj;
        std::shared_ptr<Util::Text>       hintText;
        std::shared_ptr<Util::GameObject> hintObj;
    };
    std::array<CardUI, NUM_CHOICES> m_Cards;
};
