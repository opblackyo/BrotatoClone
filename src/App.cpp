#include "App.hpp"

#include "Util/Color.hpp"
#include "Util/Image.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"
#include "Util/Time.hpp"

namespace {
const std::string TITLE_FONT = RESOURCE_DIR "/fonts/NotoSansTC-Medium.otf";

std::shared_ptr<Util::GameObject> MakeTitleArt(const std::string &path,
                                               float zIndex,
                                               glm::vec2 translation,
                                               glm::vec2 scale = {2.f / 3.f, 2.f / 3.f}) {
    auto obj = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Image>(path), zIndex);
    obj->m_Transform.translation = translation;
    obj->m_Transform.scale = scale;
    return obj;
}
} // namespace

void App::Start() {
    LOG_TRACE("Start");

    m_TitleBgObj = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Image>(RESOURCE_DIR "/images/title/title_bg.png"), 0.f);
    m_TitleBgObj->m_Transform.scale = {1280.f / 1920.f, 720.f / 1080.f};

    m_TitleArtObjs = {
        MakeTitleArt(RESOURCE_DIR "/images/title/title_mobs_back.png", 1.f, {0.f, -95.f}),
        MakeTitleArt(RESOURCE_DIR "/images/title/title_pets_back.png", 1.5f, {466.f, 154.f}),
        MakeTitleArt(RESOURCE_DIR "/images/title/title_ground.png", 2.f, {181.f, -197.f}),
        MakeTitleArt(RESOURCE_DIR "/images/title/title_catling.png", 2.5f, {-9.f, -96.f}),
        MakeTitleArt(RESOURCE_DIR "/images/title/title_fx.png", 2.6f, {-468.f, -32.f}),
        MakeTitleArt(RESOURCE_DIR "/images/title/title_brotato.png", 3.f, {239.f, -103.f}),
        MakeTitleArt(RESOURCE_DIR "/images/title/title_hand.png", 3.1f, {341.f, -50.f}),
        MakeTitleArt(RESOURCE_DIR "/images/title/title_bonkdog.png", 3.2f, {497.f, -154.f}),
    };

    m_TitleShadowText = std::make_shared<Util::Text>(
        RESOURCE_DIR "/fonts/Inter.ttf", 76, "BROTATO CLONE",
        Util::Color::FromRGB(20, 18, 24));
    m_TitleShadowObj = std::make_shared<Util::GameObject>(m_TitleShadowText, 3.f);
    m_TitleShadowObj->m_Transform.translation = {-42.f, 184.f};

    m_TitleText = std::make_shared<Util::Text>(
        RESOURCE_DIR "/fonts/Inter.ttf", 76, "BROTATO CLONE",
        Util::Color::FromRGB(255, 232, 78));
    m_TitleObj = std::make_shared<Util::GameObject>(m_TitleText, 4.f);
    m_TitleObj->m_Transform.translation = {-50.f, 192.f};

    auto makeTitleButton = [](const std::string &label, glm::vec2 center) {
        TitleButton button;
        button.center = center;
        button.halfSize = {142.f, 42.f};

        button.bg = std::make_shared<Util::GameObject>(
            std::make_shared<Util::Image>(RESOURCE_DIR "/images/ui/hp_bar_bg.png"), 5.f);
        button.bg->m_Transform.translation = center;
        button.bg->m_Transform.scale = {0.9f, 1.75f};

        button.text = std::make_shared<Util::Text>(
            TITLE_FONT, 34, label, Util::Color::FromRGB(245, 245, 245));
        button.textObj = std::make_shared<Util::GameObject>(button.text, 6.f);
        button.textObj->m_Transform.translation = {center.x - 72.f, center.y - 4.f};
        return button;
    };

    m_TitleButtons[0] = makeTitleButton("開始", {-354.f, -118.f});
    m_TitleButtons[1] = makeTitleButton("選項", {-354.f, -210.f});
    m_TitleButtons[2] = makeTitleButton("離開", {-354.f, -302.f});

    m_TitleRenderer.AddChild(m_TitleBgObj);
    m_TitleRenderer.AddChildren(m_TitleArtObjs);
    m_TitleRenderer.AddChild(m_TitleShadowObj);
    m_TitleRenderer.AddChild(m_TitleObj);
    for (const auto &button : m_TitleButtons) {
        m_TitleRenderer.AddChild(button.bg);
        m_TitleRenderer.AddChild(button.textObj);
    }

    m_CurrentState = State::TITLE;
}

void App::Update() {
    float dt = static_cast<float>(Util::Time::GetDeltaTimeMs()) / 1000.f;
    dt = std::min(dt, 0.05f);

    if (m_CurrentState == State::TITLE) {
        UpdateTitle();
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

void App::StartGame() {
    m_GameScene = std::make_unique<GameScene>();
    m_CurrentState = State::UPDATE;
}

void App::UpdateTitle() {
    UpdateTitleButtonVisuals();
    m_TitleRenderer.Update();

    if (Util::Input::IsKeyUp(Util::Keycode::RETURN) ||
        Util::Input::IsKeyUp(Util::Keycode::SPACE)) {
        StartGame();
        return;
    }

    if (Util::Input::IsKeyUp(Util::Keycode::ESCAPE) || Util::Input::IfExit()) {
        m_CurrentState = State::END;
        return;
    }

    if (!Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB)) return;

    const glm::vec2 mouse = Util::Input::GetCursorPosition();
    if (IsInTitleButton(m_TitleButtons[0], mouse)) {
        StartGame();
    } else if (IsInTitleButton(m_TitleButtons[2], mouse)) {
        m_CurrentState = State::END;
    }
}

void App::UpdateTitleButtonVisuals() {
    const glm::vec2 mouse = Util::Input::GetCursorPosition();
    for (auto &button : m_TitleButtons) {
        const bool hovered = IsInTitleButton(button, mouse);
        button.bg->m_Transform.scale = hovered ? glm::vec2{0.94f, 1.9f}
                                                : glm::vec2{0.9f, 1.75f};
        if (hovered == button.hovered) continue;
        button.hovered = hovered;
        button.text->SetColor(hovered ? Util::Color::FromRGB(30, 30, 30)
                                      : Util::Color::FromRGB(245, 245, 245));
    }
}

bool App::IsInTitleButton(const TitleButton &button, glm::vec2 point) const {
    return std::abs(point.x - button.center.x) <= button.halfSize.x &&
           std::abs(point.y - button.center.y) <= button.halfSize.y;
}

void App::End() { // NOLINT(this method will mutate members in the future)
    LOG_TRACE("End");
}

