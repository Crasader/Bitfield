#include "BitsPanel.h"

#include "Util.h"
#include "Constants.h"
#include "Generator.h";
#include "Upgrade.h";
#include "UI/UIImageView.h"
#include "UI/UIScrollView.h"
#include "UI/UIText.h"

USING_NS_CC;

bool BitsPanel::init()
{
    if (!Node::init()) return false;
    scheduleUpdate();
    setContentSize(Size(PANEL_WIDTH, PANEL_HEIGHT));
    
    addBackground();
    addTabs();
    addGeneratorLayer();
    addGenerators();
    addUpgradeLayer();
    addUpgrades();
    addBuyAmountButton();
    setView(View::Generators);

    return true;
}

void BitsPanel::update(float delta)
{
    updateIndicator();
}

void BitsPanel::setView(View view)
{
    getChildByTag(currentView)->setVisible(false);
    getChildByTag(view)->setVisible(true);

    auto tab_layer = getChildByName("tab_layer");
    tab_layer->getChildByTag(currentView)->setOpacity(255 * 0.5f);
    tab_layer->getChildByTag(view)->setOpacity(255);
    currentView = view;

    auto buy_amount = getChildByName("buy_amount");
    if (currentView != View::Generators) {
        buy_amount->setVisible(false);
    }
    else {
        buy_amount->setVisible(true);
    }
}

void BitsPanel::addBackground()
{
    auto background = ui::ImageView::create(UI_ROUNDED_RECT);
    background->setScale9Enabled(true);
    background->setContentSize(Size(PANEL_WIDTH, PANEL_HEIGHT + 16));
    background->setColor(Color3B(UI_COLOR_1));
    background->setAnchorPoint(Vec2(0, 0));
    background->setPosition(Vec2(0, 0));
    addChild(background);
}

static ui::Button* createTab(const std::string& label, float posX)
{
    auto tab = Util::createRoundedButton(UI_ROUNDED_RECT, Size(228, 64), UI_COLOR_1);
    tab->setPosition(Vec2(posX, 618));
    tab->setZoomScale(0);
    auto tab_label = ui::Text::create(label, FONT_DEFAULT, FONT_SIZE_MEDIUM);
    tab_label->setPositionNormalized(Vec2(0.5f, 0.5f));
    tab->addChild(tab_label, 0, "tab_label");
    return tab;
}

void BitsPanel::addTabs()
{
    auto tab_layer = Node::create();
    addChild(tab_layer, 0, "tab_layer");

    auto generators_tab = createTab("Generators", 0);
    generators_tab->setTag(View::Generators);
    generators_tab->setOpacity(255 * 0.5f);
    generators_tab->addTouchEventListener([&](Ref* ref, ui::Widget::TouchEventType type) {
        setView(View::Generators);
    });
    tab_layer->addChild(generators_tab, 0, "generators_tab");

    auto upgrades_tab = createTab("Upgrades", 228);
    upgrades_tab->setTag(View::Upgrades);
    upgrades_tab->setOpacity(255 * 0.5f);
    upgrades_tab->addTouchEventListener([&](Ref* ref, ui::Widget::TouchEventType type) {
        setView(View::Upgrades);
    });
    tab_layer->addChild(upgrades_tab, 0, "upgrades_tab");
}

void BitsPanel::addGeneratorLayer()
{
    auto generator_layer = ui::ScrollView::create();
    generator_layer->setContentSize(Size(984, 591));
    generator_layer->setAnchorPoint(Vec2(0, 0));
    generator_layer->setPosition(Vec2(32, 16));
    generator_layer->setLayoutType(cocos2d::ui::Layout::Type::VERTICAL);
    generator_layer->setDirection(ui::ScrollView::Direction::VERTICAL);
    generator_layer->setScrollBarPositionFromCorner(Vec2(0, 0));
    generator_layer->setScrollBarOpacity(255);
    generator_layer->setScrollBarAutoHideTime(0.7f);
    generator_layer->setScrollBarColor(Color3B::WHITE);
    generator_layer->setTag(View::Generators);
    generator_layer->setVisible(false);
    addChild(generator_layer, 0, "generator_layer");
}

void BitsPanel::addGenerators()
{
    auto generator_layer = utils::findChild<ui::ScrollView*>(this, "generator_layer");
    auto param = ui::LinearLayoutParameter::create();
    param->setGravity(ui::LinearLayoutParameter::LinearGravity::CENTER_HORIZONTAL);
    param->setMargin(ui::Margin(0, 0, 0, 16));
    auto activeCount = 1;
    for (auto i = 0; i < Player::bit_info.size(); i++) {
        auto button = Generator::create(BitType(i));
        button->setLayoutParameter(param);
        generator_layer->addChild(button, 0, i);

        if (Player::bit_info[BitType(i)].level > 0) {
            activeCount++;
        }
    }

    // Adjust ScrollView internal size
    Size containerSize;
    if (activeCount > 3) {
        containerSize = Size(984, 591 * 2);
        generator_layer->setInnerContainerSize(containerSize);
    }
}

void BitsPanel::addBuyAmountButton()
{
    auto tab_buy_amount = createTab("Buy x1", 820);
    tab_buy_amount->addTouchEventListener([=](Ref* ref, ui::Widget::TouchEventType type) {
        std::string label_string = "";
        if (type == ui::Widget::TouchEventType::ENDED) {
            Player::toggleBuyMode();
            switch (Player::buy_mode) {
            case BuyMode::One: label_string = "Buy x1"; break;
            case BuyMode::Ten: label_string = "Buy x10"; break;
            case BuyMode::Hundred: label_string = "Buy x100"; break;
            case BuyMode::Max: label_string = "Buy Max"; break;
            }
            tab_buy_amount->getChildByName<ui::Text*>("tab_label")->setString(label_string);
        }
    });
    addChild(tab_buy_amount, 0, "buy_amount");
}

void BitsPanel::addUpgradeLayer()
{
    auto upgrade_layer = ui::ScrollView::create();
    upgrade_layer->setContentSize(Size(984, 591));
    upgrade_layer->setAnchorPoint(Vec2(0, 0));
    upgrade_layer->setPosition(Vec2(32, 16));
    upgrade_layer->setLayoutType(cocos2d::ui::Layout::Type::VERTICAL);
    upgrade_layer->setDirection(ui::ScrollView::Direction::VERTICAL);
    upgrade_layer->setScrollBarPositionFromCorner(Vec2(0, 0));
    upgrade_layer->setScrollBarOpacity(255);
    upgrade_layer->setScrollBarAutoHideTime(0.7f);
    upgrade_layer->setScrollBarColor(Color3B::WHITE);
    upgrade_layer->setTag(View::Upgrades);
    upgrade_layer->setVisible(false);
    addChild(upgrade_layer, 0, "upgrade_layer");
}

void BitsPanel::addUpgrades()
{
    auto upgrade_layer = getChildByName<ui::ScrollView*>("upgrade_layer");
    auto param = ui::LinearLayoutParameter::create();
    param->setGravity(ui::LinearLayoutParameter::LinearGravity::CENTER_HORIZONTAL);
    param->setMargin(ui::Margin(0, 0, 0, 16));

    for (auto i = 0; i < Player::upgrade_info.size(); i++) {
        auto button = Upgrade::create(i);
        button->setLayoutParameter(param);
        upgrade_layer->addChild(button, 0, i);
    }

    const int UPGRADE_SIZE = 150;
    const int UPGRADES_SHOWN = 8;
    upgrade_layer->setInnerContainerSize(Size(upgrade_layer->getInnerContainerSize().width,
        UPGRADE_SIZE * UPGRADES_SHOWN));
}

void BitsPanel::updateIndicator()
{
    auto upgrades_tab = utils::findChild(this, "upgrades_tab");
    auto upgrade_indicator = upgrades_tab->getChildByName("upgrade_indicator");
    if (upgrade_indicator == nullptr && Player::canBuyUpgrade()) {
        upgrade_indicator = Sprite::create(SPRITE_CIRCLE);
        upgrade_indicator->setScale(0.7f);
        upgrade_indicator->setColor(Color3B(UI_COLOR_RED));
        upgrade_indicator->setOpacity(0);
        upgrade_indicator->setPositionNormalized(Vec2(0.90f, 0.72f));
        upgrade_indicator->runAction(RepeatForever::create(
            Sequence::create(FadeIn::create(0.5f), DelayTime::create(0.25f), FadeOut::create(0.55f), nullptr)));
        upgrades_tab->addChild(upgrade_indicator, 0, "upgrade_indicator");
    }
    else if (upgrade_indicator != nullptr && !Player::canBuyUpgrade()) {
        upgrade_indicator->removeFromParentAndCleanup(true);
    }
}