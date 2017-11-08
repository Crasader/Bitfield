#include "SquadronCard.h"

#include "Util.h"
#include "Constants.h"
#include "UI\UIText.h"
#include "UI\UIHBox.h"
#include "PlayerData\Player.h"

USING_NS_CC;

SquadronCard* SquadronCard::create(SquadronInfo* info) {
    SquadronCard *btn = new (std::nothrow) SquadronCard();
    if (btn && btn->init(info)) {
        btn->autorelease();
        return btn;
    }
    CC_SAFE_DELETE(btn);
    return nullptr;
}

bool SquadronCard::init(SquadronInfo* info)
{
    if (!ui::Button::init(UI_ROUNDED_RECT)) return false;
    this->info = info;
    setScale9Enabled(true);
    setContentSize(UI_SIZE_FLEET_SLOT_BACK);
    setAnchorPoint(VEC_CENTER);
    setCascadeOpacityEnabled(true);
    setColor(Color3B(UI_COLOR_2));
    setZoomScale(0);

    createIconBackground();
    createIcon();
    createLabel();
    createEventListener();
    return true;
}

bool SquadronCard::onTouchBegan(cocos2d::Touch* touch, cocos2d::Event * event)
{
    if (!Button::onTouchBegan(touch, event)) return false;
    stopAllActions();
    setScale(1.1f);
    return true;
}

void SquadronCard::onTouchEnded(cocos2d::Touch * touch, cocos2d::Event * event)
{
    if (getNumberOfRunningActions() == 0)
        runAction(EaseElasticOut::create(ScaleTo::create(0.4f, 1)));
    if (isHighlighted()) {
        // Tapped
        if (info->ints["owned"] > 0) {
            Player::equipSquadron(Player::slot_selected, info->strings["type"]);
            Player::dispatchEvent(EVENT_SLOT_CHANGED, (void*)Player::slot_selected);
        }
    }
    Button::onTouchEnded(touch, event);
}

void SquadronCard::onTouchCancelled(cocos2d::Touch * touch, cocos2d::Event * event)
{
    if (getNumberOfRunningActions() == 0)
        runAction(EaseElasticOut::create(ScaleTo::create(0.4f, 1)));
    Button::onTouchCancelled(touch, event);
}

void SquadronCard::createIconBackground()
{
    auto icon_background = Util::createRoundedRect(UI_ROUNDED_RECT, UI_SIZE_FLEET_SLOT_FRONT, UI_COLOR_1);
    icon_background->setAnchorPoint(Vec2(VEC_TOP));
    icon_background->setPosition(Vec2(getContentSize().width / 2, getContentSize().height - 2));
    addChild(icon_background, 0, "icon_background");
}

void SquadronCard::createIcon()
{
    auto icon = ui::ImageView::create(info->strings["sprite"]);
    icon->setPositionNormalized(VEC_CENTER);
    icon->setOpacity(info->ints["owned"] > 0 ? OPACITY_FULL : OPACITY_HALF);
    getChildByName("icon_background")->addChild(icon, 0, "icon");
}

void SquadronCard::createLabel()
{
    auto type = info->ints["owned"] > 0 ? info->strings["type"] : "???";
    auto label = ui::Text::create(type, FONT_DEFAULT, FONT_SIZE_SMALL);
    label->setPosition(Vec2(getContentSize().width / 2, 15));
    label->setOpacity(info->ints["owned"] > 0 ? OPACITY_FULL : OPACITY_HALF);
    addChild(label, 0, "label");
}

void SquadronCard::createEventListener()
{
    auto l_squadron_purchased = EventListenerCustom::create(EVENT_SQUADRON_PURCHASED, [=](EventCustom* event) {
        auto type = (const char*)event->getUserData();
        if (strcmp(info->strings["type"].c_str(), type) == 0) {
            auto owned = info->ints["owned"];
            cocos2d::log("%d %s owned", owned, type);

            // Update Icon
            auto icon_background = getChildByName("icon_background");
            auto icon = icon_background->getChildByName("icon");
            if (icon->getOpacity() < OPACITY_FULL && owned > 0) {
                cocos2d::log("%.2f %.2f", (float)icon->getOpacity(), OPACITY_HALF);
                icon->setOpacity(OPACITY_FULL);
            }


            // Update Label
            auto label = getChildByName<ui::Text*>("label");
            if (label->getOpacity() < OPACITY_FULL && owned > 0) {
                label->setOpacity(OPACITY_FULL);
                label->setString(info->strings["type"]);
            }
        }
    });
    getEventDispatcher()->addEventListenerWithSceneGraphPriority(l_squadron_purchased, this);
}
