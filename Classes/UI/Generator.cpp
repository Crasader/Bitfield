#include "Generator.h"
#include "Constants.h"
#include "Util.h"
#include "PurchaseButton.h"
#include "ui\UIImageView.h"
#include "ui\UIText.h"
#include "UI\UIButton.h"
#include "UI\UIHBox.h"
#include "UI\UIScrollView.h"

#include <iomanip>

USING_NS_CC;

Generator::Generator(BitType id) {
    this->id = id;
}

Generator* Generator::create(BitType id) {
    Generator *btn = new (std::nothrow) Generator(id);
    auto info = Player::bit_info[id];
    if (btn && btn->init()) {
        btn->autorelease();
        return btn;
    }
    CC_SAFE_DELETE(btn);
    return nullptr;
}

bool Generator::init() {
    if (!Node::init()) return false;
    scheduleUpdate();
    setCascadeOpacityEnabled(true);
    setContentSize(Size(984, 134));

    addBackground();
    addIcon();
    addLevelBar();
    addName();
    addSpawnBar();
    addSpawnCapacity();
    addBuyButton();

    return true;
}

void Generator::update(float delta) {
    Node::update(delta);

    updateLevelBar();
    updateSpawnBar();
    updateInfo();
    updateBuyButton();
}

void Generator::addBackground() {
    auto background = Util::createRoundedRect(UI_ROUNDED_RECT, Size(984, 134), UI_COLOR_2);// UI_COLOR_2);
    addChild(background);
}

void Generator::addLevelBar() {
    auto level_bar = Util::createRoundedRect(UI_ROUNDED_RECT, Size(102, 28), UI_COLOR_3);
    level_bar->setPosition(Vec2(32, -8));
    addChild(level_bar, 0, "level_bar");

    auto level_filled = Util::createRoundedRect(UI_ROUNDED_RECT, Size(96, 24), UI_COLOR_BLUE);
    level_filled->setPosition(Vec2(34, -6));
    addChild(level_filled, 0, "level_filled");

    auto level = ui::Text::create("0", FONT_DEFAULT, 30);
    level->setPosition(Vec2(32 + 102 / 2, -8 + 28 / 2));
    addChild(level, 0, "level");
}

void Generator::addIcon() {
    auto border = Util::createRoundedRect(UI_ROUNDED_RECT, Size(134, 114), UI_COLOR_3);
    border->setPosition(Vec2(16, 10));
    addChild(border);

    auto background = Util::createRoundedRect(UI_ROUNDED_RECT, Size(126, 106), Player::bit_info[id].color);
    background->setPosition(Vec2(20, 14));
    addChild(background);

    auto icon = ui::ImageView::create(Player::bit_info[id].icon_filepath);
    icon->setPositionNormalized(Vec2(0.5f, 0.52f));
    background->addChild(icon);
}

void Generator::addName() {
    auto name = ui::Text::create(Player::bit_info[id].name, FONT_DEFAULT, FONT_SIZE_MEDIUM);
    name->setAnchorPoint(Vec2(0, 0));
    name->setPosition(Vec2(166, 93 - 10));
    addChild(name);

    // Add line
    auto line = DrawNode::create();
    line->drawLine(Vec2(166, 81), Vec2(166 + 522, 81), Color4F::WHITE);
    addChild(line);
}

void Generator::addSpawnBar() {
    // Spawn Timer Empty
    auto spawn_bar_empty = Util::createRoundedRect(UI_ROUNDED_RECT, Size(522, 32), UI_COLOR_1);
    spawn_bar_empty->setPosition(Vec2(166, 41));
    addChild(spawn_bar_empty, 0, "spawn_bar_empty");

    // Spawn Timer Filled
    auto spawn_bar_filled = Util::createRoundedRect(UI_ROUNDED_RECT, Size(522, 32), Player::bit_info[id].color);
    spawn_bar_filled->setPosition(Vec2(166, 41));
    addChild(spawn_bar_filled, 0, "spawn_bar_filled");

    // Bit Icon
    auto icon = ui::ImageView::create(SPRITE_BIT);
    icon->setAnchorPoint(Vec2(0, 0.5f));
    icon->setScale(0.4f);
    icon->setPosition(Vec2(166 + 8, 56));
    addChild(icon);

    // Value
    auto value = ui::Text::create("", FONT_DEFAULT, 38);
    value->setAnchorPoint(Vec2(0, 0.5f));
    value->setPosition(Vec2(166 + 38, 56));
    addChild(value, 0, "value");

    // Spawn Timer
    auto spawn_timer = ui::Text::create("", FONT_DEFAULT, 38);
    spawn_timer->setAnchorPoint(Vec2(1, 0.5f));
    spawn_timer->setPosition(Vec2(166 + 522 - 8, 56));
    addChild(spawn_timer, 0, "spawn_timer");
}

void Generator::addSpawnCapacity() {
    // Bit Icon
    auto icon = ui::ImageView::create(SPRITE_BIT);
    icon->setAnchorPoint(Vec2(0, 0.5f));
    icon->setScale(0.4f);
    icon->setPosition(Vec2(166 + 8, 20));
    addChild(icon);

    // Spawned
    auto spawn_capacity = ui::Text::create("", FONT_DEFAULT, 38);
    spawn_capacity->setAnchorPoint(Vec2(0, 0.5f));
    spawn_capacity->setPosition(Vec2(166 + 38, 20));
    addChild(spawn_capacity, 0, "spawn_capacity");
}

void Generator::addBuyButton() {
    auto buy_button = PurchaseButton::create(UI_ROUNDED_RECT, Size(264, 114), PurchaseButton::IconType::Bits);
    buy_button->setCost(Player::bit_info[id].costString);
    buy_button->setColor(Color3B(UI_COLOR_1));
    buy_button->setHeaderColor(Player::bit_info[id].color);
    buy_button->setPosition(Vec2(704 + 264/2, 10 + 114/2));

    buy_button->addTouchEventListener([=](Ref* ref, ui::Widget::TouchEventType type) {
        if (type == ui::Widget::TouchEventType::BEGAN) {
            buy_button->setScale(1.05f);
        }
        if (type == ui::Widget::TouchEventType::CANCELED) {
            buy_button->setScale(1.0f);
        }
        if (type == ui::Widget::TouchEventType::ENDED) {
            buy_button->setScale(1.0f);
            auto oldLevel = Player::bit_info[id].level;
            auto nextTier = Player::getNextTier(id);
            auto didPurchase = Player::purchaseBitUpgrade(id);
            if (didPurchase) {
                if (oldLevel == 0 && id >= BitType::Yellow && id < BitType::Indigo) {
                    // Purchased new resource, expand scrollview
                    auto scrollView = (ui::ScrollView*)getParent()->getParent();
                    auto oldSize = scrollView->getInnerContainerSize();
                    auto newSize = Size(oldSize.width, oldSize.height + 150);
                    scrollView->setInnerContainerSize(newSize);
                    scrollView->jumpToBottom();
                }
                if (Player::bit_info[id].level >= nextTier) {
                    addLevelUp();
                }
            }
        }
    });

    addChild(buy_button, 0, "buy_button");
}

void Generator::addLevelUp() {
    if (getParent()->getParent()->getParent()->getParent()->getChildByName("popup") != nullptr) return;

    // Popup Layer
    auto popup = Util::createRoundedRect(UI_ROUNDED_RECT, Size(552 + 40, 72 + 20), UI_COLOR_1);
    popup->setIgnoreAnchorPointForPosition(false);
    popup->setAnchorPoint(Vec2(0.5f, 0.5f));
    popup->setPositionNormalized(Vec2(0.5f, 0.45f));
    popup->setCascadeOpacityEnabled(true);
    popup->setOpacity(0);
    getParent()->getParent()->getParent()->getParent()->addChild(popup, 1, "popup"); // TODO

    // Container
    auto hbox = ui::HBox::create(Size(552, 72));
    hbox->setCascadeOpacityEnabled(true);
    hbox->setAnchorPoint(Vec2(0.5f, 0.5f));
    hbox->setPositionNormalized(Vec2(0.5f, 0.5f));
    popup->addChild(hbox, 0, "hbox");

    // Layout Parameter
    auto param = ui::LinearLayoutParameter::create();
    param->setGravity(ui::LinearLayoutParameter::LinearGravity::CENTER_VERTICAL);
    param->setMargin(ui::Margin(8, 0, 8, 0));

    // Label Strings
    std::string labelString;
    std::string iconString;
    if (Player::bit_info[id].level > Player::LEVEL_TIER[2] + Player::LEVEL_TIER[3]) {
        labelString = BIT_STRINGS[id] + " value tripled!" ;
        iconString = "x3";
    }
    else {
        labelString = BIT_STRINGS[id] + " spawn rate doubled!";
        iconString = "x2";
    }

    // Icon
    auto icon = ui::ImageView::create(SPRITE_BIT);
    icon->setLayoutParameter(param);
    hbox->addChild(icon);

    // Icon Text
    auto iconText = ui::Text::create(iconString, FONT_DEFAULT, 38);
    iconText->setColor(Color3B(WORLD_COLOR));
    iconText->setPositionNormalized(Vec2(0.5f, 0.5f));
    icon->addChild(iconText);

    // Label
    auto label = ui::Text::create(labelString, FONT_DEFAULT, FONT_SIZE_MEDIUM);
    label->setLayoutParameter(param);
    hbox->addChild(label);

    // Re-adjust container sizes
    hbox->setContentSize(Size(icon->getContentSize().width + label->getContentSize().width + 32,
        std::max(icon->getContentSize().height, label->getContentSize().height) + 8));
    popup->setContentSize(hbox->getContentSize());

    // Animate
    popup->runAction(Sequence::create(
        FadeTo::create(0.15f, 255),
        DelayTime::create(2.5f),
        FadeOut::create(0.2f),
        RemoveSelf::create(),
        nullptr));
}

void Generator::updateLevelBar()
{
    auto info = Player::bit_info[id];
    std::stringstream ss;

    // Level
    auto level = utils::findChild<ui::Text*>(this, "level");
    ss << info.level;
    level->setString(ss.str());

    // Level Bar
    auto level_filled = utils::findChild(this, "level_filled");
    auto ratio = (info.level - Player::getTier(id)) / (float)(Player::getNextTier(id) - Player::getTier(id));
    level_filled->setScaleX(ratio);
}

void Generator::updateSpawnBar()
{
    auto info = Player::bit_info[id];
    std::stringstream ss;

    // Spawn Bar: Darken when max capacity, solid bar when spawning too fast
    auto spawn_bar_filled = getChildByName("spawn_bar_filled");
    if (info.spawned == info.capacity
        && spawn_bar_filled->getNumberOfRunningActions() == 0
        && spawn_bar_filled->getOpacity() != 80) {
        spawn_bar_filled->runAction(FadeTo::create(0.1f, 80));
    }
    else if (info.spawned < info.capacity
        && spawn_bar_filled->getNumberOfRunningActions() == 0
        && spawn_bar_filled->getOpacity() < 255) {
        spawn_bar_filled->runAction(FadeIn::create(0.1f));// setOpacity(255);
    }
    if (info.spawnTime <= 0.125f) {
        spawn_bar_filled->setScaleX(1);
    }
    else {
        spawn_bar_filled->setScaleX(info.timer / info.spawnTime);
    }

    // Value
    auto value = getChildByName<ui::Text*>("value");
    value->setString(info.valueString);

    // Timer
    auto timer = getChildByName<ui::Text*>("spawn_timer");
    auto time_remaining = info.spawnTime - info.timer;
    if (time_remaining <= 60) {
        //if (time_remaining <= 0.125) time_remaining = 0; // Stop annoying switching between 0.0s and 0.1s
        ss << std::fixed << std::setprecision(1) << time_remaining << "s";
    }
    else {
        // Format as minutes
        int minutes = time_remaining / 60;
        int seconds = (time_remaining / 60.0f - minutes) * 60;
        ss << minutes << ":" << std::setprecision(2);
        if (seconds == 0)
            ss << "00";
        else
            ss << seconds;
        ss << "m";
    }
    timer->setString(ss.str());
    ss.str("");
    ss << std::fixed << std::setprecision(0);
}

void Generator::updateInfo()
{
    auto info = Player::bit_info[id];
    std::stringstream ss;

    // Spawn Capacity
    auto spawn_capacity = getChildByName<ui::Text*>("spawn_capacity");
    ss << (info.level == 0 ? 0 : info.spawned) << " / " << info.capacity;
    spawn_capacity->setString(ss.str());
    ss.str("");
}

void Generator::updateBuyButton()
{
    auto buy_button = getChildByName<PurchaseButton*>("buy_button");
    auto& info = Player::bit_info[id];
    std::stringstream ss;

    // Constantly update cost string if we're on Max
    if (Player::buy_mode == BuyMode::Max) {
        auto cost = Player::calculateCost(id);
        info.cost = cost;
        info.costString = Util::getFormattedDouble(cost);
    }

    if (info.cost >= BIT_MAX) {
        buy_button->setCost("N/A");
    }
    else {
        buy_button->setCost(info.costString);
    }

    // Set Header
    auto amount = Player::getBuyAmount(id);
    if (info.level == 0) {
        ss << "Unlock";
    }
    else {
        ss << "Level Up";
        if (amount > 1) {
            ss << " (" << amount << ")";
        }
    }
    buy_button->setHeader(ss.str());

    // Hide next-next tier
    bool previousTierPurchased = (id == BitType::Green) || Player::bit_info[BitType(id - 1)].level > 0;
    if (!previousTierPurchased) {
        if (getOpacity() > 0) setOpacity(0);
        buy_button->setTouchEnabled(false);
    }
    else {
        buy_button->setTouchEnabled(true);

        // Show the next tier, faded out
        if (info.level == 0 && Player::bits < info.cost) {
            if (getOpacity() != 255 * BUY_BUTTON_FADE_PERCENT)
                setOpacity(255 * BUY_BUTTON_FADE_PERCENT);
        }
        else {
            if (getOpacity() != 255) setOpacity(255);
            if (Player::bits < info.cost) {
                if (buy_button->getOpacity() != 255 * BUY_BUTTON_FADE_PERCENT)
                    buy_button->setOpacity(255 * BUY_BUTTON_FADE_PERCENT);
            }
            else {
                if (buy_button->getOpacity() != 255)
                    buy_button->setOpacity(255);
            }
        }
    }
}
