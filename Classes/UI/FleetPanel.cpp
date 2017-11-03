#include "FleetPanel.h"

#include "Util.h"
#include "Constants.h"
#include "PurchaseButton.h"

USING_NS_CC;

bool FleetPanel::init()
{
    if (!Node::init()) return false;
    scheduleUpdate();
    setContentSize(UI_SIZE_PANEL);
    setCascadeOpacityEnabled(true);
    
    createBackground();
    createSquadronSlots();
    createCenterPanel();
    createButtons();

    return true;
}

void FleetPanel::createBackground()
{
    auto background = Util::createRoundedRect(UI_ROUNDED_RECT, UI_SIZE_PANEL, UI_COLOR_1);
    background->setSwallowTouches(true);
    addChild(background);
}

void FleetPanel::createSquadronSlots()
{
    for (int i = 0; i < 7; i++) {
        auto slot = Util::createRoundedButton(UI_ROUNDED_RECT, UI_SIZE_FLEET_SLOT, UI_COLOR_2);
        slot->setPosition(Vec2(19 + (UI_SIZE_FLEET_SLOT.width + 19) * i, 470));
        addChild(slot);
    }
}

void FleetPanel::createCenterPanel()
{
    auto panel = Util::createRoundedRect(UI_ROUNDED_RECT, UI_SIZE_FLEET_PANEL, UI_COLOR_2);
    panel->setAnchorPoint(ANCHOR_CENTER_BOTTOM);
    panel->setPosition(Vec2(getContentSize().width / 2.f, 158));
    addChild(panel);
}

void FleetPanel::createButtons()
{
    auto left = PurchaseButton::create(UI_ROUNDED_RECT, UI_SIZE_FLEET_BUTTON, PurchaseButton::IconType::Bits);
    left->setHeaderColor(UI_COLOR_RED);
    left->setHeader("Purchase Squadron");
    left->setButtonColor(UI_COLOR_2);
    left->setAnchorPoint(Vec2(0, 0));
    left->setPosition(Vec2(19, 16));
    addChild(left);

    auto right = PurchaseButton::create(UI_ROUNDED_RECT, UI_SIZE_FLEET_BUTTON, PurchaseButton::IconType::Diamonds);
    right->setHeaderColor(UI_COLOR_RED);
    right->setHeader("Purchase Squadron");
    right->setButtonColor(UI_COLOR_2);
    right->setAnchorPoint(Vec2(1, 0));
    right->setPosition(Vec2(UI_SIZE_PANEL.width - 19, 16));
    addChild(right);
}

