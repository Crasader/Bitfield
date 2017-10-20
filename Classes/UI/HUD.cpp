#include "HUD.h"
#include "World.h"
#include "..\Util.h"
#include "..\Constants.h"
#include "..\Player.h"
#include "..\Scene\GameScene.h"

#include "..\UI\BitsPanel.h"
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

    currentPanel = PanelID::Bits;
    setPanel(PanelID::Bits);

    addTabs();
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
    bit_counter_layer->setPosition(Vec2(WIDTH * 0.5f, HEIGHT * 0.92f));
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
    auto bits_panel = BitsPanel::create();
    bits_panel->setPosition(Vec2(16, 98));
    addChild(bits_panel, 0, (int)PanelID::Bits);
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
                if (i == 0) setPanel(PanelID(i));
                if (i == 1 || i == 2) Player::bits = 0;
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
    auto oldPanel = getChildByTag(currentPanel);
    auto newPanel = getChildByTag(id);
    currentPanel = id;
    oldPanel->setVisible(false);
    newPanel->setVisible(true);
}