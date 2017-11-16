#include "UpgradeItem.h"
#include "Constants.h"
#include "Util.h"
#include "UI\PurchaseButton.h"
#include "ui\UIImageView.h"
#include "ui\UIText.h"
#include "UI\UIButton.h"
#include "UI\UIHBox.h"

#include <iomanip>

USING_NS_CC;

UpgradeItem::UpgradeItem(int id) {
    this->id = id;
}

UpgradeItem* UpgradeItem::create(int id) {
    UpgradeItem *btn = new (std::nothrow) UpgradeItem(id);
    auto info = Player::upgrades[id];
    if (btn && btn->init()) {
        btn->autorelease();
        return btn;
    }
    CC_SAFE_DELETE(btn);
    return nullptr;
}

bool UpgradeItem::init() {
    if (!Node::init()) return false;
    scheduleUpdate();
    setCascadeOpacityEnabled(true);

    setContentSize(Size(984, 134));
    createBackground();
    addIcon();
    addName();
    addDescription();
    addBuyButton();

    return true;
}

void UpgradeItem::update(float delta) {
    Node::update(delta);
    std::stringstream ss;

    // Buy Button
    auto buy_button = getChildByName<PurchaseButton*>("buy_button");
    auto cost = Player::upgrades[id].cost;

    // Item Opacity
    if (Player::bits < cost) {
        buy_button->setOpacity(OPACITY_HALF);
    }
    else {
        buy_button->setOpacity(OPACITY_FULL);
    }
}

int UpgradeItem::getID() {
    return id;
}

void UpgradeItem::createBackground() {
    auto background = Util::createRoundedRect(UI_ROUNDED_RECT, Size(984, 134), UI_COLOR_2);
    addChild(background);
}

void UpgradeItem::addIcon() {
    auto border = Util::createRoundedRect(UI_ROUNDED_RECT, Size(134, 114), UI_COLOR_3);
    border->setPosition(Vec2(16, 10));
    addChild(border);

    auto background = Util::createRoundedRect(UI_ROUNDED_RECT, Size(126, 106), Player::upgrades[id].color);
    background->setPosition(Vec2(20, 14));
    addChild(background);

    auto icon = ui::ImageView::create(Player::upgrades[id].icon_filepath);
    icon->setPositionNormalized(Vec2(0.5f, 0.52f));
    background->addChild(icon);
}

void UpgradeItem::addName() {
    auto name = ui::Text::create(Player::upgrades[id].name, FONT_DEFAULT, FONT_SIZE_MEDIUM);
    name->setAnchorPoint(Vec2(0, 0));
    name->setPosition(Vec2(166, 93 - 10));
    addChild(name);

    // Add line
    auto line = DrawNode::create();
    line->drawLine(Vec2(166, 81), Vec2(166 + 522, 81), Color4F::WHITE);
    addChild(line);
}

void UpgradeItem::addDescription() {
    auto desc = ui::Text::create(Player::upgrades[id].desc, FONT_DEFAULT, 34);
    desc->setContentSize(Size(520, 72));
    desc->ignoreContentAdaptWithSize(false);
    desc->setAnchorPoint(Vec2(0, 1));
    desc->setPosition(Vec2(165, 80));
    addChild(desc);
}

void UpgradeItem::addBuyButton() {
    auto buy_button = PurchaseButton::create(UI_ROUNDED_RECT, Size(264, 114), PurchaseButton::IconType::Bits);
    buy_button->setColor(Color3B(UI_COLOR_3));
    buy_button->setHeaderColor(Player::upgrades[id].color);
    buy_button->setPosition(Vec2(704 + 264 / 2, 10 + 114 / 2));
    buy_button->setHeader("Buy");
    buy_button->setCost(Util::getFormattedDouble(Player::upgrades[id].cost));
    buy_button->onPurchase = [=]() {
        if (Player::purchaseUpgrade(id)) {
            removeFromParent();
        }
    };

    addChild(buy_button, 0, "buy_button");
}