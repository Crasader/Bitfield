#include "SquadronSlot.h"

#include "Util.h"
#include "Constants.h"
#include "UI\UIText.h"
#include "UI\UIHBox.h"

USING_NS_CC;

SquadronSlot::SquadronSlot(int slot)
{
    this->slot = slot;
}

SquadronSlot* SquadronSlot::create(int slot) {
    SquadronSlot *btn = new (std::nothrow) SquadronSlot(slot);
    if (btn && btn->init(UI_ROUNDED_RECT)) {
        btn->autorelease();
        return btn;
    }
    CC_SAFE_DELETE(btn);
    return nullptr;
}

bool SquadronSlot::init(const std::string& path)
{
    if (!ui::Button::init(path)) return false;
    setScale9Enabled(true);
    setContentSize(UI_SIZE_FLEET_SLOT_BACK);
    setAnchorPoint(VEC_CENTER);
    setCascadeOpacityEnabled(true);
    setColor(Color3B(UI_COLOR_2));
    setZoomScale(0);
    onClick = [this]() {
        Player::unlockSlot(slot);
    };

    createIconBackground();
    createIcon();
    createLabel();
    return true;
}

bool SquadronSlot::onTouchBegan(cocos2d::Touch* touch, cocos2d::Event * event)
{
    if (!Button::onTouchBegan(touch, event)) return false;
    setScale(1.05f);
    return true;
}

void SquadronSlot::onTouchEnded(cocos2d::Touch * touch, cocos2d::Event * event)
{
    Button::onTouchEnded(touch, event);
    setScale(1.0f);
    onClick();
}

void SquadronSlot::onTouchCancelled(cocos2d::Touch * touch, cocos2d::Event * event)
{
    setScale(1.0f);
}

void SquadronSlot::createIconBackground()
{
    auto icon_background = Util::createRoundedRect(UI_ROUNDED_RECT, UI_SIZE_FLEET_SLOT_FRONT, UI_COLOR_1);
    icon_background->setAnchorPoint(Vec2(VEC_TOP));
    icon_background->setPosition(Vec2(getContentSize().width / 2, getContentSize().height - 2));
    addChild(icon_background, 0, "icon_background");
}

void SquadronSlot::createIcon()
{
    std::string path;
    if (Player::isSlotUnlocked(slot))
        path = UI_ICON_SHIP;
    else
        path = UI_ICON_LOCK;

    auto icon = ui::ImageView::create(path);
    icon->setPositionNormalized(VEC_CENTER);
    getChildByName("icon_background")->addChild(icon, 0, "icon");
}

void SquadronSlot::createLabel()
{
    std::string str;
    float opacity = OPACITY_FULL;
    if (Player::isSlotUnlocked(slot)) {
        str = "Basic";
    }
    else {
        str = "Locked";
        opacity = OPACITY_HALF;
    }

    auto label = ui::Text::create(str, FONT_DEFAULT, FONT_SIZE_SMALL);
    label->setPosition(Vec2(getContentSize().width / 2, 15));
    label->setOpacity(opacity);
    addChild(label, 0, "label");
}

void SquadronSlot::createEventListener()
{
}
