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
    State m_CurrentState = State::START;
    std::unique_ptr<GameScene> m_GameScene;

    // Title screen
    Util::Renderer m_TitleRenderer;
    std::shared_ptr<Util::Text>       m_TitleText;
    std::shared_ptr<Util::GameObject> m_TitleObj;
    std::shared_ptr<Util::Text>       m_TitleSubText;
    std::shared_ptr<Util::GameObject> m_TitleSubObj;

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
