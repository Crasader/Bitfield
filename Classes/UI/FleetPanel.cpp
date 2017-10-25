#include "FleetPanel.h"

#include "Util.h"
#include "Constants.h"
#include "Generator.h"
#include "UpgradeItem.h"
#include "UI/UIImageView.h"
#include "UI/UIScrollView.h"
#include "UI/UIText.h"

USING_NS_CC;

bool FleetPanel::init()
{
    if (!Node::init()) return false;
    scheduleUpdate();
    setContentSize(Size(PANEL_WIDTH, PANEL_HEIGHT));
    
    addBackground();
    addTabs();
    addFleetLayer();
    setView(View::Fleet);

    return true;
}

void FleetPanel::setView(View view)
{
    getChildByTag(currentView)->setVisible(false);
    getChildByTag(view)->setVisible(true);

    auto tab_layer = getChildByName("tab_layer");
    tab_layer->getChildByTag(currentView)->setOpacity(255 * 0.5f);
    tab_layer->getChildByTag(view)->setOpacity(255);
    currentView = view;
}

void FleetPanel::addBackground()
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
    tab->setPosition(Vec2(posX, PANEL_HEIGHT));
    tab->setZoomScale(0);
    auto tab_label = ui::Text::create(label, FONT_DEFAULT, FONT_SIZE_MEDIUM);
    tab_label->setPositionNormalized(Vec2(0.5f, 0.6f));
    tab->addChild(tab_label, 0, "tab_label");
    return tab;
}

void FleetPanel::addTabs()
{
    auto tab_layer = Node::create();
    addChild(tab_layer, 0, "tab_layer");

    auto generators_tab = createTab("Fleet", 0);
    generators_tab->setTag(View::Fleet);
    generators_tab->setOpacity(255 * 0.5f);
    generators_tab->addTouchEventListener([&](Ref* ref, ui::Widget::TouchEventType type) {
        setView(View::Fleet);
    });
    tab_layer->addChild(generators_tab, 0, "generators_tab");

    auto upgrades_tab = createTab("Squadrons", 228);
    upgrades_tab->setTag(View::Squadrons);
    upgrades_tab->setOpacity(255 * 0.5f);
    upgrades_tab->addTouchEventListener([&](Ref* ref, ui::Widget::TouchEventType type) {
        setView(View::Squadrons);
    });
    tab_layer->addChild(upgrades_tab, 0, "upgrades_tab");
}

void FleetPanel::addFleetLayer()
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
    generator_layer->setTag(View::Fleet);
    generator_layer->setVisible(false);
    addChild(generator_layer, 0, "generator_layer");
}