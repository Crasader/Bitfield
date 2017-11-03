#include "FleetPanel.h"

#include "Util.h"
#include "Constants.h"
#include "UI\PurchaseButton.h"
#include "UI\Fleet\SquadronSlot.h"

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

void FleetPanel::update(float delta)
{
    updateButtons();
}

void FleetPanel::createBackground()
{
    auto background = Util::createRoundedRect(UI_ROUNDED_RECT, UI_SIZE_PANEL, UI_COLOR_1);
    background->setOpacity(OPACITY_UI);
    background->setSwallowTouches(true);
    addChild(background);
}

void FleetPanel::createSquadronSlots()
{
    auto slot_layer = Layer::create();
    slot_layer->setCascadeOpacityEnabled(true);
    slot_layer->setAnchorPoint(VEC_BOTTOM);
    slot_layer->setContentSize(Size(UI_SIZE_FLEET_PANEL.width, UI_SIZE_FLEET_SLOT_BACK.height));
    slot_layer->setPosition(Vec2(19, UI_SIZE_FLEET_PANEL.height + 32));
    for (int i = 0; i < 7; i++) {
        auto slot = SquadronSlot::create(i);
        slot->setPosition(Vec2(UI_SIZE_FLEET_SLOT_BACK.width / 2 + (UI_SIZE_FLEET_SLOT_BACK.width + 19) * i,
            UI_SIZE_FLEET_SLOT_BACK.height / 2));
        slot_layer->addChild(slot);
    }
    addChild(slot_layer, 0, "slot_layer");
}

void FleetPanel::createCenterPanel()
{
    auto panel = Util::createRoundedRect(UI_ROUNDED_RECT, UI_SIZE_FLEET_PANEL, UI_COLOR_2);
    panel->setCascadeOpacityEnabled(true);
    panel->setAnchorPoint(VEC_BOTTOM);
    panel->setPosition(Vec2(getContentSize().width / 2.f, 16));
    addChild(panel);
}

void FleetPanel::createButtons()
{
    auto button_layer = Layer::create();
    button_layer->setCascadeOpacityEnabled(true);
    button_layer->setContentSize(Size(UI_SIZE_FLEET_PANEL.width, UI_SIZE_FLEET_BUTTON.height));
    button_layer->setAnchorPoint(VEC_TOP);
    button_layer->setPosition(Vec2(getContentSize().width / 2, getContentSize().height - UI_SIZE_FLEET_BUTTON.height / 2 - 16));

    auto left = PurchaseButton::create(UI_ROUNDED_RECT, UI_SIZE_FLEET_BUTTON, PurchaseButton::IconType::Bits);
    left->setHeaderColor(UI_COLOR_RED);
    left->setButtonColor(UI_COLOR_2);
    left->setHeader("Purchase Squadron");
    auto cost = Player::squadron_costs.front();
    if (cost == 0) {
        left->setCost(" FREE!");
    }
    else {
        left->setCost(Util::getFormattedDouble(Player::squadron_costs.front()));
    }
    left->setPosition(Vec2(-UI_SIZE_FLEET_BUTTON.width / 2 - 10, 0));
    left->onPurchase = [=]() {
        if (Player::purchaseSquadron()) {
            if (Player::squadron_costs.empty())
                left->setCost("N/A");
            else
                left->setCost(Util::getFormattedDouble(Player::squadron_costs.front()));
        }
    };
    button_layer->addChild(left, 0, "left");

    auto right = PurchaseButton::create(UI_ROUNDED_RECT, UI_SIZE_FLEET_BUTTON, PurchaseButton::IconType::Diamonds);
    right->setHeaderColor(UI_COLOR_INDIGO);
    right->setButtonColor(UI_COLOR_2);
    right->setHeader("Purchase Squadron");
    right->setCost(Util::getFormattedDouble(Player::squadron_diamond_cost));
    right->setPosition(Vec2(UI_SIZE_FLEET_BUTTON.width / 2 + 10, 0));
    button_layer->addChild(right, 0, "right");

    addChild(button_layer, 0, "button_layer");
}

void FleetPanel::createEventListeners()
{

}

void FleetPanel::updateButtons()
{
    auto button_layer = getChildByName("button_layer");

    auto left = button_layer->getChildByName("left");
    if (Player::squadron_costs.empty() || (Player::bits < Player::squadron_costs.front() && left->getOpacity() != OPACITY_HALF)) {
        left->setOpacity(OPACITY_HALF);
    }
    else if (left->getOpacity() != OPACITY_FULL) {
        left->setOpacity(OPACITY_FULL);
    }

    auto right = button_layer->getChildByName("right");
}

void FleetPanel::updateSlots()
{
}

