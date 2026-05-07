#ifndef APP_HPP
#define APP_HPP

#include "pch.hpp" // IWYU pragma: export
#include "Scene/GameScene.hpp"
#include "Util/Text.hpp"
#include "Util/GameObject.hpp"
#include "Util/Renderer.hpp"

class App {
public:
    enum class State {
        START,
        TITLE,
        UPDATE,
        END,
    };

    State GetCurrentState() const { return m_CurrentState; }

    void Start();
    void Update();
    void End(); // NOLINT(readability-convert-member-functions-to-static)

private:
    struct TitleButton {
        std::shared_ptr<Util::GameObject> bg;
        std::shared_ptr<Util::Text>       text;
        std::shared_ptr<Util::GameObject> textObj;
        glm::vec2 center = {0.f, 0.f};
        glm::vec2 halfSize = {0.f, 0.f};
        bool hovered = false;
    };

    void StartGame();
    void UpdateTitle();
    void UpdateTitleButtonVisuals();
    bool IsInTitleButton(const TitleButton &button, glm::vec2 point) const;

    State m_CurrentState = State::START;
    std::unique_ptr<GameScene> m_GameScene;

    // Title screen
    Util::Renderer m_TitleRenderer;
    std::shared_ptr<Util::GameObject> m_TitleBgObj;
    std::vector<std::shared_ptr<Util::GameObject>> m_TitleArtObjs;
    std::shared_ptr<Util::Text>       m_TitleText;
    std::shared_ptr<Util::Text>       m_TitleShadowText;
    std::shared_ptr<Util::GameObject> m_TitleObj;
    std::shared_ptr<Util::GameObject> m_TitleShadowObj;
    std::array<TitleButton, 3>        m_TitleButtons;

    // Overlay screens (game over / victory)
    std::shared_ptr<Util::Text>       m_OverlayText;
    std::shared_ptr<Util::GameObject> m_OverlayObj;
    std::shared_ptr<Util::Text>       m_OverlayHintText;
    std::shared_ptr<Util::GameObject> m_OverlayHintObj;
    Util::Renderer m_OverlayRenderer;
    bool  m_OverlayShown  = false;
    float m_OverlayTimer  = 0.f;
};

#endif
