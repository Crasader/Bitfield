#include "Upgrade.h"
#include "Constants.h"
#include "Util.h"
#include "PurchaseButton.h"
#include "ui\UIImageView.h"
#include "ui\UIText.h"
#include "UI\UIButton.h"
#include "UI\UIHBox.h"

#include <iomanip>

USING_NS_CC;

Upgrade::Upgrade(int id) {
    this->id = id;
}

Upgrade* Upgrade::create(int id) {
    Upgrade *btn = new (std::nothrow) Upgrade(id);
    auto info = Player::upgrade_info[id];
    if (btn && btn->init()) {
        btn->autorelease();
        return btn;
    }
    CC_SAFE_DELETE(btn);
    return nullptr;
}

bool Upgrade::init() {
    if (!Node::init()) return false;
    scheduleUpdate();
    setCascadeOpacityEnabled(true);

    setContentSize(Size(984, 134));
    addBackground();
    addIcon();
    addName();
    addDescription();
    addBuyButton();

    return true;
}

void Upgrade::update(float delta) {
    Node::update(delta);
    std::stringstream ss;

    // Buy Button
    auto buy_button = getChildByName<PurchaseButton*>("buy_button");
    auto cost = Player::upgrade_info[id].cost;

    // Item Opacity
    if (Player::bits < cost) {
        buy_button->setOpacity(255 * BUY_BUTTON_FADE_PERCENT);
    }
    else {
        buy_button->setOpacity(255);
    }
}

int Upgrade::getID() {
    return id;
}

void Upgrade::addBackground() {
    auto background = Util::createRoundedRect(UI_ROUNDED_RECT, Size(984, 134), UI_COLOR_2);
    addChild(background);
}

void Upgrade::addIcon() {
    auto border = Util::createRoundedRect(UI_ROUNDED_RECT, Size(134, 114), UI_COLOR_3);
    border->setPosition(Vec2(16, 10));
    addChild(border);

    auto background = Util::createRoundedRect(UI_ROUNDED_RECT, Size(126, 106), Player::upgrade_info[id].color);
    background->setPosition(Vec2(20, 14));
    addChild(background);

    auto icon = ui::ImageView::create(Player::upgrade_info[id].icon_filepath);
    icon->setPositionNormalized(Vec2(0.5f, 0.52f));
    background->addChild(icon);
}

void Upgrade::addName() {
    auto name = ui::Text::create(Player::upgrade_info[id].name, FONT_DEFAULT, FONT_SIZE_MEDIUM);
    name->setAnchorPoint(Vec2(0, 0));
    name->setPosition(Vec2(166, 93 - 10));
    addChild(name);

    // Add line
    auto line = DrawNode::create();
    line->drawLine(Vec2(166, 81), Vec2(166 + 522, 81), Color4F::WHITE);
    addChild(line);
}

void Upgrade::addDescription() {
    auto desc = ui::Text::create(Player::upgrade_info[id].desc, FONT_DEFAULT, 34);
    desc->setContentSize(Size(520, 72));
    desc->ignoreContentAdaptWithSize(false);
    desc->setAnchorPoint(Vec2(0, 1));
    desc->setPosition(Vec2(165, 80));
    addChild(desc);
}

void Upgrade::addBuyButton() {
    auto buy_button = PurchaseButton::create(Size(264, 114));
    buy_button->setAnchorPoint(Vec2(0.5f, 0.5f));
    buy_button->setPosition(Vec2(704 + 264 / 2, 10 + 114 / 2));
    buy_button->setHeader("Buy");
    buy_button->setCost(Player::upgrade_info[id].cost);

    buy_button->getChildByName<ui::Button*>("button")->addTouchEventListener([=](Ref* ref, ui::Widget::TouchEventType type) {
        if (type == ui::Widget::TouchEventType::BEGAN) {
            buy_button->setScale(1.05f);
        }
        if (type == ui::Widget::TouchEventType::ENDED) {
            buy_button->setScale(1.0f);
            if (Player::purchaseUpgrade(id)) {
                removeFromParent();
                Player::canBuyUpgrade();
            }
        }
    });

    addChild(buy_button, 0, "buy_button");
}