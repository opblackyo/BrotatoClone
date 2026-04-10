#pragma once
#include "pch.hpp"
#include "Util/Text.hpp"
#include "Util/Image.hpp"
#include "Util/GameObject.hpp"

class Player; // forward declaration

class PauseMenu {
public:
    PauseMenu();

    // Register all pre-allocated GameObjects (call once at scene init)
    std::vector<std::shared_ptr<Util::GameObject>> GetObjects() const;

    // Show the pause menu populated with current game state
    void Show(const Player &player, const std::vector<std::string> &ownedItems, int wave);

    void Hide();
    bool IsVisible() const { return m_Visible; }

    // Returns: 0=nothing, 1=resume, 2=quit
    int Update();

    void SetVolumeText(float volumeMultiplier);

private:
    void UpdateContent(const Player &player,
                       const std::vector<std::string> &ownedItems,
                       int wave);
    bool IsInResume(glm::vec2 pt) const;
    bool IsInQuit(glm::vec2 pt)   const;

    bool m_Visible = false;
    std::shared_ptr<Util::Text> m_VolText, m_VolMinusText, m_VolPlusText;
    std::shared_ptr<Util::GameObject> m_VolObj, m_VolMinusObj, m_VolPlusObj;

    // 點擊判定
    bool IsInVolMinus(glm::vec2 pt) const;
    bool IsInVolPlus(glm::vec2 pt) const;

    std::shared_ptr<Util::GameObject> m_BgObj;
    std::shared_ptr<Util::Text>       m_TitleText;
    std::shared_ptr<Util::GameObject> m_TitleObj;

    // Stats panel (left side, 17 lines)
    static constexpr int NUM_STATS = 17;
    std::shared_ptr<Util::Text>       m_StatsTitleText;
    std::shared_ptr<Util::GameObject> m_StatsTitleObj;
    std::array<std::shared_ptr<Util::Text>,       NUM_STATS> m_StatTexts;
    std::array<std::shared_ptr<Util::GameObject>, NUM_STATS> m_StatObjs;

    // Items panel (right side, up to 14 lines)
    static constexpr int MAX_ITEM_LINES = 14;
    std::shared_ptr<Util::Text>       m_ItemsTitleText;
    std::shared_ptr<Util::GameObject> m_ItemsTitleObj;
    std::array<std::shared_ptr<Util::Text>,       MAX_ITEM_LINES> m_ItemTexts;
    std::array<std::shared_ptr<Util::GameObject>, MAX_ITEM_LINES> m_ItemObjs;

    // Buttons
    std::shared_ptr<Util::GameObject> m_ResumeBg;
    std::shared_ptr<Util::Text>       m_ResumeText;
    std::shared_ptr<Util::GameObject> m_ResumeObj;
    std::shared_ptr<Util::GameObject> m_QuitBg;
    std::shared_ptr<Util::Text>       m_QuitText;
    std::shared_ptr<Util::GameObject> m_QuitObj;
};
