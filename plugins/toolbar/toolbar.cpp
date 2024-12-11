#include <cassert>
#include <iostream>

#include "../plugins/toolbar/toolbar.hpp"

#include "style/design.hpp"

#include "implementation/bar/bar_base.hpp"

static psapi::sfm::ITexture* back    = nullptr;
static psapi::sfm::ITexture* release = nullptr;
static psapi::sfm::ITexture* hover   = nullptr;
static psapi::sfm::ITexture* press   = nullptr;
static psapi::sfm::ITexture* normal  = nullptr;

static const psapi::sfm::IntRect BACKGROUND_RECT = {{0, 0}, {128, 700}};
static const psapi::sfm::IntRect BUTTON_RECT = {{0, 0}, {90, 90}};

static const char* BACKGROUND_TEXTURE = "assets/textures/background_gray.jpg";
static const char* HOVER_TEXTURE      = "assets/textures/hovered_icon.png";
static const char* RELEASE_TEXTURE    = "assets/textures/active_icon.png";
static const char* PRESS_TEXTURE      = "assets/textures/pressed_icon.png";
static const char* NORMAL_TEXTURE     = "assets/textures/normal_icon.png";

bool onLoadPlugin()
{
    std::cout << "toolbar loaded\n";

    back = psapi::sfm::ITexture::create().release();
    back->loadFromFile(BACKGROUND_TEXTURE);

    hover = psapi::sfm::ITexture::create().release();
    hover->loadFromFile(HOVER_TEXTURE);

    press = psapi::sfm::ITexture::create().release();
    press->loadFromFile(PRESS_TEXTURE);

    normal = psapi::sfm::ITexture::create().release();
    normal->loadFromFile(NORMAL_TEXTURE);

    release = psapi::sfm::ITexture::create().release();
    release->loadFromFile(RELEASE_TEXTURE);

    std::unique_ptr<psapi::sfm::ISprite> back_sprite = psapi::sfm::ISprite::create();
    make_styled_sprite(back_sprite.get(), back, BACKGROUND_RECT);

    std::unique_ptr<psapi::sfm::ISprite> release_sprite = psapi::sfm::ISprite::create();
    make_styled_sprite(release_sprite.get(), release, BUTTON_RECT, 1);

    std::unique_ptr<psapi::sfm::ISprite> hover_sprite = psapi::sfm::ISprite::create();
    make_styled_sprite(hover_sprite.get(), hover, BUTTON_RECT, 1);

    std::unique_ptr<psapi::sfm::ISprite> press_sprite = psapi::sfm::ISprite::create();
    make_styled_sprite(press_sprite.get(), press, BUTTON_RECT, 1);

    std::unique_ptr<psapi::sfm::ISprite> normal_sprite = psapi::sfm::ISprite::create();
    make_styled_sprite(normal_sprite.get(), normal, BUTTON_RECT, 1);

    auto bar = std::make_unique<ToolBar>(psapi::kToolBarWindowId,
                                         psapi::vec2i(20, 20),
                                         psapi::vec2u(BACKGROUND_RECT.size.x, BACKGROUND_RECT.size.y),
                                         std::move(back_sprite),
                                         std::move(normal_sprite),
                                         std::move(hover_sprite),
                                         std::move(press_sprite),
                                         std::move(release_sprite));

    auto root = psapi::getRootWindow();

    root->addWindow(std::move(bar));
}

void onUnloadPlugin()
{
    delete back;
    delete release;
    delete hover;
    delete press;
    delete normal;
}


// ********************* TOOLBAR *********************/

ToolBar::ToolBar(const psapi::wid_t id, const psapi::vec2i& pos, const psapi::vec2u& size,
            std::unique_ptr<psapi::sfm::ISprite> background,
            std::unique_ptr<psapi::sfm::ISprite> normal,
            std::unique_ptr<psapi::sfm::ISprite> hovered,
            std::unique_ptr<psapi::sfm::ISprite> pressed,
            std::unique_ptr<psapi::sfm::ISprite> released) :
    ABar(id, pos, size, std::move(background),
                    std::move(normal),
                    std::move(hovered),
                    std::move(pressed),
                    std::move(released))
{
}


void ToolBar::draw(psapi::sfm::IRenderWindow* renderWindow)
{
    background_.get()->setPosition(pos_.x, pos_.y);
    background_.get()->draw(renderWindow);

    for (auto& button : buttons_)
    {
        finishButtonDraw(renderWindow, button.get());
        button.get()->draw(renderWindow);
    }

}
std::unique_ptr<psapi::IAction> ToolBar::createAction(const psapi::IRenderWindow* renderWindow, const psapi::Event& event)
{
    return std::make_unique<ToolBarAction>(renderWindow, event, this);
}

// ******************** TOOLBAR ACTION ***********************

ToolBarAction::ToolBarAction(const psapi::IRenderWindow* render_window, const psapi::Event& event, ToolBar* tool_bar) :
AAction(render_window, event), tool_bar_(tool_bar)
{
}

bool ToolBarAction::execute(const Key& key)
{
    if (!tool_bar_->is_active_)
        return false;

    bool flag = false;

    auto controller = psapi::getActionController();

    for (size_t i = 0; i < tool_bar_->buttons_.size(); ++i)
    {
        auto& button = tool_bar_->buttons_[i];

        psapi::IBarButton::State prev_state = button.get()->getState();

        flag |= controller->execute(button.get()->createAction(render_window_, event_));

        psapi::IBarButton::State cur_state = button.get()->getState();

        if (cur_state == psapi::IBarButton::State::Released &&
            prev_state != psapi::IBarButton::State::Released)
        {
            for (size_t j = 0; j < tool_bar_->buttons_.size(); ++j)
                if (j != i) tool_bar_->buttons_[j].get()->setState(psapi::IBarButton::State::Normal);
        }
    }

    return flag;
}

bool ToolBarAction::isUndoable(const Key& key)
{
    return false;
}
