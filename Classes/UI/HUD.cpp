#include "HUD.h"
#include "World.h"
#include "Util.h"
#include "Constants.h"
#include "PlayerData/Player.h"
#include "Scene\GameScene.h"

#include "UI\Bits\BitsPanel.h"
#include "UI\Fleet\SquadronPanel.h"
#include "UI\Fleet\FleetPanel.h"

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
    createEventListeners();

    return true;
}

void HUD::update(float delta)
{
    Layer::update(delta);
}

void HUD::setWorld(World* world) {
    this->world = world;
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
            EaseBackOut::create(MoveTo::create(0.3f, Vec2(UI_CENTER_X, 730))),
            EaseSineIn::create(FadeIn::create(0.1f)),
            nullptr
        ),
        nullptr
    ));

    getTab(id)->setOpacity(OPACITY_UI_TABS);
    world->offsetCamera(true);
}

void HUD::hidePanel(PanelID id)
{
    auto panel = getChildByTag(id)->getChildByName("panel");
    //if (panel->getNumberOfRunningActions() > 0 || !panel->isVisible()) return;

    panel->runAction(Sequence::create(
        Spawn::create(
            EaseSineIn::create(MoveTo::create(0.2f, Vec2(UI_CENTER_X, -298))),
            EaseSineIn::create(FadeOut::create(0.1f)),
            nullptr
        ),
        Hide::create(),
        nullptr
    ));

    getTab(id)->setOpacity(OPACITY_HALF);

    world->offsetCamera(false);
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
    auto fleetPanel = getChildByTag(PanelID::Temp);
    auto clip = getChildByTag(PanelID::Fleet);
    auto panel = clip->getChildByName("panel");
    panel->runAction(Sequence::create(
        Spawn::create(
            EaseSineIn::create(MoveTo::create(0.2f, Vec2(UI_CENTER_X, 98))),
            EaseSineIn::create(FadeOut::create(0.1f)),
            nullptr
        ),
        RemoveSelf::create(true),
        CallFunc::create([=]() {
            clip->removeFromParentAndCleanup(true);
            fleetPanel->setTag(PanelID::Fleet);
            showPanel(PanelID::Fleet);
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
    bit_counter_layer->setCascadeOpacityEnabled(true);
    bit_counter_layer->setAnchorPoint(Vec2(0.5f, 0.5f));
    bit_counter_layer->setPosition(Vec2(GAME_WIDTH * 0.5f, GAME_HEIGHT));
    bit_counter_layer->setOpacity(0);
    addChild(bit_counter_layer, 0, "bit_counter_layer");

    auto icon = ui::ImageView::create(SPRITE_BIT);
    auto param = ui::LinearLayoutParameter::create();
    param->setGravity(ui::LinearLayoutParameter::LinearGravity::CENTER_VERTICAL);
    param->setMargin(ui::Margin(0, 0, 8, 0));
    icon->setLayoutParameter(param);
    bit_counter_layer->addChild(icon, 0, "icon");

    auto counter = ui::Text::create(Player::bitString, FONT_DEFAULT, FONT_SIZE_HUGE);
    bit_counter_layer->addChild(counter, 0, "counter");

    // Resize
    bit_counter_layer->setContentSize(Size(icon->getContentSize().width + counter->getContentSize().width,
        counter->getContentSize().height));

    // Animate
    bit_counter_layer->runAction(
        Sequence::create(
            DelayTime::create(1.25f),
            Spawn::create(
                EaseSineOut::create(MoveTo::create(0.9f, Vec2(GAME_WIDTH * 0.5f, GAME_HEIGHT * 0.92f))),
                EaseSineOut::create(FadeTo::create(0.9f, OPACITY_FULL)),
                nullptr
            ),
            nullptr
        )
    );

    // Listen for changes
    auto l_bits_added = EventListenerCustom::create(EVENT_BITS_CHANGED, [=](EventCustom* event) {
        counter->setString(Player::bitString);
        bit_counter_layer->setContentSize(Size(icon->getContentSize().width + counter->getContentSize().width,
            counter->getContentSize().height));
        bit_counter_layer->runAction(runAction(Sequence::create(
            ScaleTo::create(0.1f, 1.25f),
            ScaleTo::create(0.1f, 1),
            nullptr
        )));
    });
    getEventDispatcher()->addEventListenerWithSceneGraphPriority(l_bits_added, this);
}

void HUD::createPanels()
{
    addPanel(BitsPanel::create(), PanelID::Bits);

    auto fleetID = PanelID::Fleet;
    if (!Player::eventFinished(EVENT_FLEET_UNLOCKED)) {
        addPanel(SquadronPanel::create(), PanelID::Fleet);
        fleetID = PanelID::Temp;
    }
    addPanel(FleetPanel::create(), fleetID);
}

void HUD::createTabs() {
    auto tab_layer = Layer::create();
    tab_layer->setPosition(Vec2(UI_CENTER_X - 540, 8));
    addChild(tab_layer, 0, "tab_layer");

    // Create tab buttons
    for (int i = 0; i < 5; i++) {
        auto tab_button = Util::createRoundedButton(UI_ROUNDED_RECT, Size(200, 82), UI_COLOR_1);
        tab_button->setPosition(Vec2(116 + 212 * i, -100));
        tab_button->setOpacity(0);
        tab_button->addTouchEventListener([=](Ref* ref, ui::Widget::TouchEventType type) {
            if (type == ui::Widget::TouchEventType::BEGAN) {
                tab_button->stopAllActions();
                tab_button->setScale(1.1f);
            }
            else if (type == ui::Widget::TouchEventType::CANCELED) {
                tab_button->runAction(EaseElasticOut::create(ScaleTo::create(0.4f, 1)));
            }
            else if (type == ui::Widget::TouchEventType::ENDED) {
                tab_button->runAction(EaseElasticOut::create(ScaleTo::create(0.4f, 1)));
                if (i == 0 || i == 1) togglePanel(PanelID(i));
                if (i == 2) Player::subBits(Player::bits);
                if (i == 3) {
                    double sum = 0;
                    for (int j = 0; j < BitType::All; j++) {
                        auto type = BitType(j);
                        if (Player::generators[type].level > 0)
                            sum += Player::calculateValue(BitType(type));
                    }
                    Player::addBits(sum);
                }
                if (i == 4) Player::addBits(BIT_MAX);
            }
        });

        // Animate
        tab_button->runAction(
            Sequence::create(
                DelayTime::create(1.0f + 0.12f * i),
                Spawn::create(
                    EaseSineOut::create(MoveTo::create(0.4f, Vec2(116 + 212 * i, 41))),
                    EaseSineOut::create(FadeTo::create(0.4f, OPACITY_HALF)),
                    nullptr
                ),
                nullptr
            )
        );
        
        // Add Icon to Button
        auto tab_icon = ui::ImageView::create(TAB_ICONS[i]);
        tab_icon->setPositionNormalized(Vec2(0.5f, 0.5f));
        tab_button->addChild(tab_icon);

        // Add Button to Layer
        tab_layer->addChild(tab_button, 0, i);
    }
}

void HUD::createEventListeners()
{
    //// Popup Squadron Panel when we have enough money
    //if (!Player::eventFinished(EVENT_SQUADRON_UNLOCKED)) {
    //    auto l_first_ship = EventListenerCustom::create(EVENT_SQUADRON_UNLOCKED, [=](EventCustom* event) {
    //        showPanel(PanelID::Squadron);
    //    });
    //    getEventDispatcher()->addEventListenerWithSceneGraphPriority(l_first_ship, this);
    //}

    // Hide Squadron and create Fleet
    if (!Player::eventFinished(EVENT_FLEET_UNLOCKED)) {
        auto l_fleet_unlock = EventListenerCustom::create(EVENT_FLEET_UNLOCKED, [=](EventCustom* event) {
            unlockFleet();
        });
        getEventDispatcher()->addEventListenerWithSceneGraphPriority(l_fleet_unlock, this);
    }

    // Popups on squadron purchased
    auto l_squadron_purchased = EventListenerCustom::create(EVENT_SQUADRON_PURCHASED, [=](EventCustom* event) {
        auto type = (const char*)event->getUserData();

        // Clean up previous popup
        auto previousPopup = getChildByName("squadron_popup");
        if (previousPopup) {
            previousPopup->removeFromParentAndCleanup(true);
        }

        // Determine the string to display
        std::stringstream ss;
        ss << "Rolled a " << type << "!";
        if (Player::squadrons[type].ints["owned"] > 1) {
            ss << " (Duplicate)";
        }

        // Attach the label to the camera
        auto label = Label::create(ss.str(), FONT_DEFAULT, FONT_SIZE_LARGE);
        label->setPositionNormalized(Vec2(0.5f, 0.5f));
        label->setOpacity(0);
        label->runAction(Sequence::create(
            FadeIn::create(0.2f),
            DelayTime::create(1.75f),
            FadeOut::create(0.2f),
            RemoveSelf::create(),
            nullptr
        ));
        addChild(label, 99, "squadron_popup");

    });
    getEventDispatcher()->addEventListenerWithSceneGraphPriority(l_squadron_purchased, this);
}