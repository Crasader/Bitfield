#include "FleetPanel.h"

#include "Util.h"
#include "Constants.h"
#include "UI\PurchaseButton.h"
#include "UI\Fleet\SquadronSlot.h"
#include "UI\Fleet\SquadronCard.h"

#include "UI\UIScrollView.h"
#include "UI\UIText.h"

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
    createEventListeners();

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

static Vec2 getSlotPosition(int slot) {
    return Vec2(UI_SIZE_FLEET_SLOT_BACK.width / 2 + (UI_SIZE_FLEET_SLOT_BACK.width + 19) * slot,
        UI_SIZE_FLEET_SLOT_BACK.height / 2);
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
        slot->setPosition(getSlotPosition(i));
        slot_layer->addChild(slot);
    }
    addChild(slot_layer, 0, "slot_layer");

    // Add indicator
    auto slot_indicator = Util::createRoundedRect(UI_ROUNDED_RECT,
        Size(UI_SIZE_FLEET_SLOT_BACK.width + 4, UI_SIZE_FLEET_SLOT_BACK.height + 4),
        UI_COLOR_WHITE);
    slot_indicator->setAnchorPoint(VEC_CENTER);
    slot_indicator->setPosition(Vec2(UI_SIZE_FLEET_SLOT_BACK.width / 2,
        UI_SIZE_FLEET_SLOT_BACK.height / 2));
    slot_layer->addChild(slot_indicator, -1, "slot_indicator");
}

void FleetPanel::createCenterPanel()
{
    auto panel = Util::createRoundedRect(UI_ROUNDED_RECT, UI_SIZE_FLEET_PANEL, UI_COLOR_2);
    panel->setCascadeOpacityEnabled(true);
    panel->setAnchorPoint(VEC_BOTTOM);
    panel->setPosition(Vec2(getContentSize().width / 2.f, 16));
    addChild(panel, 0, "panel");

    auto scrollview = ui::ScrollView::create();
    scrollview->setContentSize(UI_SIZE_FLEET_SCROLLVIEW);
    scrollview->setAnchorPoint(VEC_CENTER);
    scrollview->setPosition(panel->getContentSize() / 2);
    scrollview->setLayoutType(cocos2d::ui::Layout::Type::HORIZONTAL);
    scrollview->setDirection(ui::ScrollView::Direction::HORIZONTAL);
    scrollview->setScrollBarPositionFromCorner(Vec2(0, 0));
    scrollview->setScrollBarOpacity(OPACITY_UI);
    scrollview->setScrollBarAutoHideTime(0.7f);
    scrollview->setScrollBarAutoHideEnabled(false);
    panel->addChild(scrollview, 0, "scrollview");

    auto rankC = createSquadronRank(0);
    auto rankB = createSquadronRank(1);
    auto rankA = createSquadronRank(2);

    auto innerWidth = 32 * 2 + 28 * 2
        + rankC->getContentSize().width
        + rankB->getContentSize().width
        + rankA->getContentSize().width;
    scrollview->setInnerContainerSize(Size(innerWidth, UI_SIZE_FLEET_SCROLLVIEW.height));
}

cocos2d::Node* FleetPanel::createSquadronRank(int rank)
{
    auto scrollview = getChildByName("panel")->getChildByName<ui::ScrollView*>("scrollview");
    char rankChar = 'C' - rank;

    auto background = Util::createRoundedRect(UI_ROUNDED_RECT, Size(0, 232), UI_COLOR_1);
    auto param = ui::LinearLayoutParameter::create();
    param->setGravity(ui::LinearLayoutParameter::LinearGravity::CENTER_VERTICAL);
    param->setMargin(ui::Margin(32, 0, 0, 0));
    background->setLayoutParameter(param);
    scrollview->addChild(background);

    auto rankIcon = Sprite::create(SPRITE_BIT);
    rankIcon->setColor(Color3B(Player::generators[BitType(rank)].color));
    rankIcon->setPosition(Vec2(background->getBoundingBox().getMinX() + 8, background->getBoundingBox().getMaxY() - 16));
    auto rankLabel = ui::Text::create(std::string(1, rankChar), FONT_DEFAULT, 40);
    rankLabel->setPosition(rankIcon->getPosition());
    background->addChild(rankIcon);
    background->addChild(rankLabel);

    // Add squadron cards
    int count = 0;
    for (auto& pair : Player::squadrons) {
        auto& info = pair.second;
        if (info.ints["rank"] == rank) {
            auto squadronCard = SquadronCard::create(&info);
            squadronCard->setPosition(Vec2(28 + UI_SIZE_FLEET_SLOT_BACK.width / 2 + (28 + UI_SIZE_FLEET_SLOT_BACK.width) * count, 116));
            background->addChild(squadronCard);
            count++;
        }
    }
    background->setContentSize(Size(24 + (UI_SIZE_FLEET_SLOT_BACK.width + 28) * count, 232));
    return background;
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
    // Update slot indicator
    auto l_slot_selected = EventListenerCustom::create(EVENT_SLOT_SELECTED, [=](EventCustom* event) {
        auto changed_slot = (int)event->getUserData();
        auto slot_indicator = getChildByName("slot_layer")->getChildByName("slot_indicator");
        slot_indicator->stopAllActions();
        slot_indicator->runAction(
            Spawn::createWithTwoActions(
                Sequence::create(
                    FadeOut::create(0.1f),
                    DelayTime::create(0.05f),
                    FadeIn::create(0.05f),
                    nullptr
                ),
                EaseBackOut::create(MoveTo::create(0.2f, getSlotPosition(changed_slot)))
            )
        );
    });
    getEventDispatcher()->addEventListenerWithSceneGraphPriority(l_slot_selected, this);
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