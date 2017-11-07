#include "SquadronSlot.h"

#include "Util.h"
#include "Constants.h"
#include "UI\UIText.h"
#include "UI\UIHBox.h"
#include "PlayerData\Player.h"

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

    createIconBackground();
    createIcon();
    createLabel();
    createEventListener();
    return true;
}

bool SquadronSlot::onTouchBegan(cocos2d::Touch* touch, cocos2d::Event * event)
{
    if (!Button::onTouchBegan(touch, event)) return false;
    stopAllActions();
    setScale(1.1f);
    return true;
}

void SquadronSlot::onTouchEnded(cocos2d::Touch * touch, cocos2d::Event * event)
{
    if (getNumberOfRunningActions() == 0)
        runAction(EaseElasticOut::create(ScaleTo::create(0.4f, 1)));
    if (isHighlighted()) {
        if (!Player::isSlotUnlocked(slot)) {
            Player::unlockSlot(slot);
        }
        else {
            if (Player::getEquippedType(slot) == "Empty") {
                Player::squadrons_equipped[slot] = rand_0_1() > 0.5f ? "Basic" : "Wanderer";
                Player::dispatchEvent(EVENT_SLOT_CHANGED, (void*)slot);
            }
            else if (Player::slot_selected == slot) {
                Player::squadrons_equipped[slot] = "Empty";
                Player::dispatchEvent(EVENT_SLOT_CHANGED, (void*)slot);
            }

            Player::slot_selected = slot;
            Player::dispatchEvent(EVENT_SLOT_SELECTED, (void*)slot);
        }
    }
    Button::onTouchEnded(touch, event);
}

void SquadronSlot::onTouchCancelled(cocos2d::Touch * touch, cocos2d::Event * event)
{
    if (getNumberOfRunningActions() == 0)
        runAction(EaseElasticOut::create(ScaleTo::create(0.4f, 1)));
    Button::onTouchCancelled(touch, event);
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
    std::string type = Player::getEquippedType(slot);
    std::string path;
    float opacity = OPACITY_HALF;
    if (type == "Empty") {
        path = UI_ICON_PLUS;
    }
    else if (type == "Locked") {
        path = UI_ICON_LOCK;
        opacity = OPACITY_FULL;
    }
    else {
        path = Player::squadrons[type].strings["sprite"];
        opacity = OPACITY_FULL;
    }

    auto icon = ui::ImageView::create(path);
    icon->setPositionNormalized(VEC_CENTER);
    icon->setOpacity(opacity);
    getChildByName("icon_background")->addChild(icon, 0, "icon");
}

void SquadronSlot::createLabel()
{
    std::string type = Player::getEquippedType(slot);
    float opacity = OPACITY_FULL;
    if (type == "Locked" || type == "Empty")
        opacity = OPACITY_HALF;

    auto label = ui::Text::create(type, FONT_DEFAULT, FONT_SIZE_SMALL);
    label->setPosition(Vec2(getContentSize().width / 2, 15));
    label->setOpacity(opacity);
    addChild(label, 0, "label");
}

void SquadronSlot::createEventListener()
{
    auto l_slot_changed = EventListenerCustom::create(EVENT_SLOT_CHANGED, [=](EventCustom* event) {
        auto changed_slot = (int)event->getUserData();
        if (changed_slot == this->slot) {
            // Update Icon
            auto icon_background = getChildByName("icon_background");
            icon_background->removeChildByName("icon");
            createIcon();

            // Update Label
            auto label = getChildByName<ui::Text*>("label");
            auto type = Player::getEquippedType(slot);
            label->setString(type);
            if (type != "Locked" && type != "Empty")
                label->setOpacity(OPACITY_FULL);
            else
                label->setOpacity(OPACITY_HALF);
        }
    });
    getEventDispatcher()->addEventListenerWithSceneGraphPriority(l_slot_changed, this);
}
