#include "HUD.h"
#include "World.h"
#include "..\Util.h"
#include "..\Constants.h"
#include "..\Player.h"
#include "..\Scene\GameScene.h"

#include "..\UI\BitsPanel.h"
#include "..\UI\SquadronPanel.h"
#include "..\UI\Upgrade.h"

#include "UI\UIText.h"
#include "UI\UIScrollView.h"
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

    return true;
}

void HUD::setWorld(World* world) {
    this->world = world;
}

void HUD::onEnter() {
    Layer::onEnter();
    scheduleUpdate();

    addBitCounter();
    addPanels();
    addTabs();
    
    currentPanel = PanelID::Bits;
    setPanel(PanelID::Bits);
}

void HUD::onExit() {
    Layer::onExit();
    unscheduleUpdate();
}

void HUD::update(float delta) {
    Layer::update(delta);

    // Update Bit Counter
    auto bit_counter_layer = getChildByName("bit_counter_layer");
    auto icon = bit_counter_layer->getChildByName("icon");
    auto counter = bit_counter_layer->getChildByName<ui::Text*>("counter");
    counter->setString(Player::getFormattedBits(Player::bits));
    bit_counter_layer->setContentSize(Size(icon->getContentSize().width + counter->getContentSize().width,
        counter->getContentSize().height));
}

void HUD::addBitCounter() {
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

void HUD::addPanels()
{
    auto addCentered = [&](cocos2d::Node* node, PanelID id) {
        node->setAnchorPoint(ANCHOR_CENTER_BOTTOM);
        node->setPosition(Vec2(UI_CENTER_X, PANEL_Y));
        node->setVisible(false);
        addChild(node, 0, id);
    };

    addCentered(BitsPanel::create(), PanelID::Bits);
    addCentered(SquadronPanel::create(), PanelID::Squadron);
}

void HUD::addTabs() {
    auto tab_layer = Layer::create();
    addChild(tab_layer, 0, "tab_layer");

    // Create tab buttons
    for (int i = 0; i < 5; i++) {
        auto tab_button = ui::Button::create(UI_ROUNDED_RECT);
        tab_button->setScale9Enabled(true);
        tab_button->setContentSize(Size(200, 82));
        tab_button->setColor(Color3B(UI_COLOR_1));
        tab_button->setCascadeOpacityEnabled(true);
        tab_button->setAnchorPoint(Vec2(0, 0));
        tab_button->setPosition(Vec2(16 + 212 * i, 8));
        tab_button->addTouchEventListener([=](Ref* ref, ui::Widget::TouchEventType type) {
            if (type == ui::Widget::TouchEventType::ENDED) {
                if (i == 0 || i == 1) setPanel(PanelID(i));
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

void HUD::setPanel(PanelID id) {
    if (currentPanel == id) {
        auto panel = getChildByTag(id);
        panel->setVisible(!panel->isVisible());
    }
    else {
        auto oldPanel = getChildByTag(currentPanel);
        auto newPanel = getChildByTag(id);
        currentPanel = id;
        oldPanel->setVisible(false);
        newPanel->setVisible(true);
    }

    // Change appearance of selected and unselected tabs
    auto tab_layer = getChildByName("tab_layer");
    for (int i = 0; i < 5; i++) {
        auto panel = getChildByTag(i);
        auto tab_button = tab_layer->getChildByTag<ui::Button*>(i);
        if (panel != nullptr && panel->isVisible()) {
            tab_button->setOpacity(255);
        }
        else {
            tab_button->setOpacity(255 * 0.75f);
        }
    }
}