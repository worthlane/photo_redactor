#ifndef SCROLLBAR_PLUGIN_HPP
#define SCROLLBAR_PLUGIN_HPP

#include "standard/api_sfm.hpp"
#include "standard/api_photoshop.hpp"

class Scrollable
{
public:
    virtual bool updateScale(const psapi::IRenderWindow* renderWindow, const psapi::Event& event) = 0;
    virtual psapi::sfm::vec2u getObjectSize() const = 0;

    psapi::sfm::vec2i getCoordStart() const { return coord_start_; }
    psapi::sfm::vec2f getScale() const      { return scale_; }

    void setCoordStart(psapi::sfm::vec2i coord_start) { coord_start_ = coord_start; }
    void setScale(psapi::sfm::vec2f scale)            { scale_ = scale; }

protected:
    psapi::sfm::vec2i coord_start_ = {0, 0};
    psapi::sfm::vec2f scale_       = {1, 1};
};

class AScrollBar : public psapi::IWindow
{
public:
    enum class State
    {
        Normal,
        Active,
    };
    
    AScrollBar(const psapi::wid_t id, const psapi::vec2i& pos, const psapi::vec2u& size,
               std::unique_ptr<psapi::sfm::ITexture> background,
               std::unique_ptr<psapi::sfm::ITexture> normal,
               std::unique_ptr<psapi::sfm::ITexture> active,
               Scrollable* object);

    virtual psapi::wid_t getId() const override;

    virtual psapi::IWindow* getWindowById(psapi::wid_t id) override;
    virtual const psapi::IWindow* getWindowById(psapi::wid_t id) const override;
    virtual psapi::vec2i getPos() const override;
    virtual psapi::vec2u getSize() const override;
    virtual void setParent(const psapi::IWindow* parent) override;

    virtual void forceActivate() override;
    virtual void forceDeactivate() override;

    virtual bool isActive() const override;
    virtual bool isWindowContainer() const override;

protected:
    Scrollable* object_;

    psapi::vec2i pos_;
    psapi::vec2i size_;

    psapi::vec2i scroller_pos_;
    psapi::vec2i scroller_size_;

    psapi::wid_t id_ = psapi::kInvalidWindowId;

    bool is_active_ = true;

    std::unique_ptr<psapi::sfm::ITexture> background_;

    std::unique_ptr<psapi::sfm::ITexture> normal_;
    std::unique_ptr<psapi::sfm::ITexture> active_;

    State state_ = State::Normal;

    void loadStateTexture(psapi::sfm::ISprite* sprite) const;
};

class HorizontalScrollBar : public AScrollBar
{
public:
    HorizontalScrollBar(const psapi::wid_t id, const psapi::vec2i& pos, const psapi::vec2u& size,
                        std::unique_ptr<psapi::sfm::ITexture> background,
                        std::unique_ptr<psapi::sfm::ITexture> normal,
                        std::unique_ptr<psapi::sfm::ITexture> active,
                        Scrollable* object);

    virtual void draw(psapi::IRenderWindow* renderWindow) override;
    virtual bool update(const psapi::IRenderWindow* renderWindow, const psapi::Event& event) override;

private:

};

static psapi::sfm::vec2i operator/(const psapi::sfm::vec2i& self, const psapi::sfm::vec2f& other)
{
    return {static_cast<int>(static_cast<double>(self.x) / other.x),
            static_cast<int>(static_cast<double>(self.y) / other.y)};
}

static psapi::sfm::vec2i operator*(const psapi::sfm::vec2i& self, const psapi::sfm::vec2f& other)
{
    return {static_cast<int>(static_cast<double>(self.x) * other.x),
            static_cast<int>(static_cast<double>(self.y) * other.y)};
}


/*class VerticalScrollBar : public AScrollBar
{
public:
    VerticalScrollBar();

    virtual void draw(psapi::IRenderWindow* renderWindow) override;
    virtual bool update(const psapi::IRenderWindow* renderWindow, const psapi::Event& event) override;

private:

};*/



#endif // SCROLLBAR_PLUGIN_HPP