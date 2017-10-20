#include "Util.h"

#include "Constants.h"

USING_NS_CC;

static const cocos2d::Rect CAP_INSETS(48, 48, 48, 48);

Util::Util(){}

ui::ImageView* Util::createRoundedRect(const std::string& path, cocos2d::Size size, cocos2d::Color4B color) {
    auto rect = ui::ImageView::create(path);
    rect->setScale9Enabled(true);
    //rect->setCapInsets(CAP_INSETS);
    rect->setContentSize(size);
    rect->setColor(Color3B(color));
    rect->setCascadeOpacityEnabled(true);
    rect->setAnchorPoint(Vec2(0, 0));
    return rect;
}

ui::Button* Util::createRoundedButton(const std::string& path, cocos2d::Size size, cocos2d::Color4B color) {
    auto button = ui::Button::create(path);
    button->setScale9Enabled(true);
    //button->setCapInsets(CAP_INSETS);
    button->setContentSize(size);
    button->setColor(Color3B(color));
    button->setCascadeOpacityEnabled(true);
    button->setAnchorPoint(Vec2(0, 0));
    return button;
}