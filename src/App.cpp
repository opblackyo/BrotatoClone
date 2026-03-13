#include "App.hpp"

#include "Util/Color.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"
#include "Util/Time.hpp"

void App::Start() {
    LOG_TRACE("Start");

    // Build title screen
    m_TitleText = std::make_shared<Util::Text>(
        RESOURCE_DIR "/fonts/Inter.ttf", 72, "BROTATO CLONE",
        Util::Color::FromRGB(255, 200, 30));
    m_TitleObj = std::make_shared<Util::GameObject>(m_TitleText, 10.f);
    m_TitleObj->m_Transform.translation = {0.f, 60.f};

    m_TitleSubText = std::make_shared<Util::Text>(
        RESOURCE_DIR "/fonts/Inter.ttf", 28, "Press ENTER or SPACE to Start",
        Util::Color::FromRGB(220, 220, 220));
    m_TitleSubObj = std::make_shared<Util::GameObject>(m_TitleSubText, 10.f);
    m_TitleSubObj->m_Transform.translation = {0.f, -30.f};

    m_TitleRenderer.AddChild(m_TitleObj);
    m_TitleRenderer.AddChild(m_TitleSubObj);

    m_CurrentState = State::TITLE;
}

void App::Update() {
    float dt = static_cast<float>(Util::Time::GetDeltaTimeMs()) / 1000.f;
    dt = std::min(dt, 0.05f);

    // ── Title screen ─────────────────────────────────────────────────────
    if (m_CurrentState == State::TITLE) {
        m_TitleRenderer.Update();
        bool start = Util::Input::IsKeyUp(Util::Keycode::RETURN) ||
                     Util::Input::IsKeyUp(Util::Keycode::SPACE);
        if (start) {
            m_GameScene = std::make_unique<GameScene>();
            m_CurrentState = State::UPDATE;
        }
        if (Util::Input::IfExit()) m_CurrentState = State::END;
        return;
    }

    // ── Gameplay ──────────────────────────────────────────────────────────
    GameSceneState sceneState = m_GameScene->Update(dt);

    if (sceneState == GameSceneState::GAME_OVER && !m_OverlayShown) {
        m_OverlayText = std::make_shared<Util::Text>(
            RESOURCE_DIR "/fonts/Inter.ttf", 64, "GAME OVER",
            Util::Color::FromRGB(255, 60, 60));
        m_OverlayObj = std::make_shared<Util::GameObject>(m_OverlayText, 20.f);
        m_OverlayObj->m_Transform.translation = {0.f, 60.f};
        m_OverlayRenderer.AddChild(m_OverlayObj);

        m_OverlayHintText = std::make_shared<Util::Text>(
            RESOURCE_DIR "/fonts/Inter.ttf", 26, "R = Restart    ESC = Quit",
            Util::Color::FromRGB(200, 200, 200));
        m_OverlayHintObj = std::make_shared<Util::GameObject>(m_OverlayHintText, 20.f);
        m_OverlayHintObj->m_Transform.translation = {0.f, -20.f};
        m_OverlayRenderer.AddChild(m_OverlayHintObj);
        m_OverlayShown = true;
    }

    if (sceneState == GameSceneState::VICTORY && !m_OverlayShown) {
        m_OverlayText = std::make_shared<Util::Text>(
            RESOURCE_DIR "/fonts/Inter.ttf", 64, "YOU WIN!",
            Util::Color::FromRGB(80, 255, 120));
        m_OverlayObj = std::make_shared<Util::GameObject>(m_OverlayText, 20.f);
        m_OverlayObj->m_Transform.translation = {0.f, 60.f};
        m_OverlayRenderer.AddChild(m_OverlayObj);

        m_OverlayHintText = std::make_shared<Util::Text>(
            RESOURCE_DIR "/fonts/Inter.ttf", 26, "R = Play Again    ESC = Quit",
            Util::Color::FromRGB(200, 200, 200));
        m_OverlayHintObj = std::make_shared<Util::GameObject>(m_OverlayHintText, 20.f);
        m_OverlayHintObj->m_Transform.translation = {0.f, -20.f};
        m_OverlayRenderer.AddChild(m_OverlayHintObj);
        m_OverlayShown = true;
    }

    if (m_OverlayShown) {
        m_OverlayRenderer.Update();

        // R: restart game
        if (Util::Input::IsKeyUp(Util::Keycode::R)) {
            m_GameScene = std::make_unique<GameScene>();
            // Clear overlay
            m_OverlayRenderer.RemoveChild(m_OverlayObj);
            if (m_OverlayHintObj) m_OverlayRenderer.RemoveChild(m_OverlayHintObj);
            m_OverlayObj.reset();
            m_OverlayHintObj.reset();
            m_OverlayShown = false;
            m_OverlayTimer = 0.f;
            return;
        }
    }

    if (Util::Input::IfExit() ||
        (m_OverlayShown && Util::Input::IsKeyUp(Util::Keycode::ESCAPE))) {
        m_CurrentState = State::END;
    }
}

void App::End() { // NOLINT(this method will mutate members in the future)
    LOG_TRACE("End");
}

