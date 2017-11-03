#include "HUD.h"
#include "World.h"
#include "Util.h"
#include "Constants.h"
#include "PlayerData/Player.h"
#include "Scene\GameScene.h"

#include "UI\BitsPanel.h"
#include "UI\SquadronPanel.h"
#include "UI\FleetPanel.h"
#include "UI\UpgradeItem.h"

#include "ui\UIText.h"
#include "ui\UIScrollView.h"
#include "ui\UIButton.h"
#include "ui\UILayoutParameter.h"
#include "ui\UIImageView.h"
#include "ui\UIHBox.h"

#include "UI\PurchaseButton.h"

#include <iostream>

USING_NS_CC;

HUD* HUD::create() {
    HUD* hud = new (std::nothrow) HUD();
    
    if (hud && hud->init())
    {
        hud->autorelease();
        return hud;
    }
    CC_SAFE_DELETE(hud);
    return nullptr;
}

bool HUD::init() {
    if (!Layer::init()) {
        return false;
    }
    scheduleUpdate();

    createCounter();
    createPanels();
    createTabs();
    createEventListener();

    return true;
}

void HUD::setWorld(World* world) {
    this->world = world;
}

void HUD::update(float delta) {
    Layer::update(delta);
    updateCounter();
}

void HUD::addPanel(cocos2d::Node* panel, PanelID id)
{
    auto clip = ClippingRectangleNode::create(Rect(0, 98, GAME_WIDTH, GAME_HEIGHT));
    clip->setContentSize(Size(GAME_WIDTH, GAME_HEIGHT));
    clip->setPosition(Vec2(0, 0));

    panel->setAnchorPoint(Vec2(0.5f, 1));
    panel->setPosition(Vec2(UI_CENTER_X, 98));
    panel->setVisible(false);

    clip->addChild(panel, 0, "panel");
    addChild(clip, 0, id);
}

void HUD::showPanel(PanelID id) {
    auto panel = getChildByTag(id)->getChildByName("panel");
    //if (panel->getNumberOfRunningActions() > 0 || panel->isVisible()) return;

    // Hide other panels
    for (int i = 0; i < 5; i++) {
        if (i == id || i > 1) continue;
        hidePanel(PanelID(i));
    }

    // Show this one
    panel->runAction(Sequence::create(
        Show::create(),
        Spawn::create(
            EaseBackOut::create(MoveTo::create(0.2f, Vec2(UI_CENTER_X, 730))),
            EaseSineIn::create(FadeIn::create(0.1f)),
            nullptr
        ),
        nullptr
    ));

    world->offsetCameraForPanelIsVisible(true);
}

void HUD::hidePanel(PanelID id)
{
    auto panel = getChildByTag(id)->getChildByName("panel");
    //if (panel->getNumberOfRunningActions() > 0 || !panel->isVisible()) return;

    panel->runAction(Sequence::create(
        Spawn::create(
            EaseSineIn::create(MoveTo::create(0.2f, Vec2(UI_CENTER_X, 98))),
            EaseSineIn::create(FadeOut::create(0.1f)),
            nullptr
        ),
        Hide::create(),
        nullptr
    ));

    world->offsetCameraForPanelIsVisible(false);
}

void HUD::togglePanel(PanelID id)
{
    auto panel = getChildByTag(id)->getChildByName("panel");
    if (panel->getNumberOfRunningActions() > 0) return;
    if (panel->isVisible()) {
        hidePanel(id);
    }
    else {
        showPanel(id);
    }
}

void HUD::unlockFleet()
{
    auto clip = getChildByTag(PanelID::Squadron);
    auto panel = clip->getChildByName("panel");
    panel->runAction(Sequence::create(
        Spawn::create(
            EaseSineIn::create(MoveTo::create(0.2f, Vec2(UI_CENTER_X, 98))),
            EaseSineIn::create(FadeOut::create(0.1f)),
            nullptr
        ),
        RemoveSelf::create(false),
        CallFunc::create([=]() {
            clip->removeFromParentAndCleanup(true);
            addPanel(FleetPanel::create(), PanelID::Squadron);
            showPanel(PanelID::Squadron);
        }),
        nullptr
    ));
}

cocos2d::Node* HUD::getTab(PanelID id)
{
    auto tab_layer = getChildByName("tab_layer");
    auto tab_button = tab_layer->getChildByTag(id);
    return tab_button;
}

void HUD::createCounter() {
    auto bit_counter_layer = ui::HBox::create(Size(400, 90));
    bit_counter_layer->setAnchorPoint(Vec2(0.5f, 0.5f));
    bit_counter_layer->setPosition(Vec2(GAME_WIDTH * 0.5f, GAME_HEIGHT * 0.92f));
    addChild(bit_counter_layer, 0, "bit_counter_layer");

    auto icon = ui::ImageView::create(SPRITE_BIT);
    auto param = ui::LinearLayoutParameter::create();
    param->setGravity(ui::LinearLayoutParameter::LinearGravity::CENTER_VERTICAL);
    param->setMargin(ui::Margin(0, 0, 8, 0));
    icon->setLayoutParameter(param);
    bit_counter_layer->addChild(icon, 0, "icon");

    auto counter = ui::Text::create("0", FONT_DEFAULT, BIT_COUNTER_SIZE);
    bit_counter_layer->addChild(counter, 0, "counter");
}

void HUD::createPanels()
{
    addPanel(BitsPanel::create(), PanelID::Bits);
    if (Player::squadrons[0].ints["count"] < 7) // TODO: faulty condition
        addPanel(SquadronPanel::create(), PanelID::Squadron);
    else
        addPanel(FleetPanel::create(), PanelID::Squadron);
}

void HUD::createTabs() {
    auto tab_layer = Layer::create();
    addChild(tab_layer, 0, "tab_layer");

    // Create tab buttons
    for (int i = 0; i < 5; i++) {
        auto tab_button = Util::createRoundedButton(UI_ROUNDED_RECT, Size(200, 82), UI_COLOR_1);
        tab_button->setPosition(Vec2(16 + 212 * i, 8));
        tab_button->setOpacity(OPACITY_UI_TABS);
        tab_button->addTouchEventListener([=](Ref* ref, ui::Widget::TouchEventType type) {
            if (type == ui::Widget::TouchEventType::ENDED) {
                if (i == 0 || i == 1) togglePanel(PanelID(i));
                if (i == 2) Player::bits = 0;
                if (i == 3) {
                    auto sum = 0;
                    for (int i = 0; i < BitType::All; i++) {
                        auto type = BitType(i);
                        if (Player::bit_info[type].level > 0)
                            sum += Player::calculateValue(BitType(type));
                    }
                    Player::addBits(sum);
                }
                if (i == 4) Player::addBits(BIT_MAX);
            }
        });
        
        // Add Icon to Button
        auto tab_icon = ui::ImageView::create(TAB_ICONS[i]);
        tab_icon->setPositionNormalized(Vec2(0.5f, 0.5f));
        tab_button->addChild(tab_icon);

        // Add Button to Layer
        tab_layer->addChild(tab_button, 0, i);
    }
}

void HUD::createEventListener()
{
    // Popup Squadron Panel when we have enough money
    auto l_first_ship = EventListenerCustom::create(EVENT_FIRST_SHIP, [=](EventCustom* event) {
        showPanel(PanelID::Squadron);
        getEventDispatcher()->removeCustomEventListeners(EVENT_FIRST_SHIP);
    });
    getEventDispatcher()->addEventListenerWithSceneGraphPriority(l_first_ship, this);

    // Hide Squadron and create Fleet
    auto l_fleet_unlock = EventListenerCustom::create(EVENT_FLEET_UNLOCK, [=](EventCustom* event) {
        unlockFleet();
        getEventDispatcher()->removeCustomEventListeners(EVENT_FLEET_UNLOCK);
    });
    getEventDispatcher()->addEventListenerWithSceneGraphPriority(l_fleet_unlock, this);
}

void HUD::updateCounter()
{
    // Update Bit Counter
    auto bit_counter_layer = getChildByName("bit_counter_layer");
    auto icon = bit_counter_layer->getChildByName("icon");
    auto counter = bit_counter_layer->getChildByName<ui::Text*>("counter");
    counter->setString(Util::getFormattedDouble(Player::bits));
    bit_counter_layer->setContentSize(Size(icon->getContentSize().width + counter->getContentSize().width,
        counter->getContentSize().height));
}