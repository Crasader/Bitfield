#include "SquadronPanel.h"

#include "Constants.h"
#include "Util.h"
#include "UI\PurchaseButton.h"

USING_NS_CC;

static std::list<Vec2> positions;

SquadronPanel* SquadronPanel::create() {
    SquadronPanel* panel = new (std::nothrow) SquadronPanel();

    if (panel && panel->init())
    {
        panel->autorelease();
        return panel;
    }
    CC_SAFE_DELETE(panel);
    return nullptr;
}

bool SquadronPanel::init()
{
    if (!Node::init()) return false;
    setContentSize(UI_SIZE_PANEL);
    setCascadeOpacityEnabled(true);

    createBackground();
    addPurchaseButton();
    addSilhouettes();

    scheduleUpdate();
    return true;
}

void SquadronPanel::update(float delta)
{
    Node::update(delta);
    updatePurchaseButton();
}

void SquadronPanel::createBackground()
{
    auto background = Util::createRoundedRect(UI_ROUNDED_RECT, getContentSize(), UI_COLOR_1);
    background->setSwallowTouches(true);
    background->setOpacity(OPACITY_UI);
    addChild(background, 0, "background");
}

void SquadronPanel::addPurchaseButton()
{
    auto background = getChildByName("background");
    auto parentSize = background->getContentSize();

    auto purchase_button = PurchaseButton::create(UI_ROUNDED_RECT, Size(484, 140), PurchaseButton::IconType::Bits);
    purchase_button->setButtonColor(UI_COLOR_3);
    purchase_button->setHeaderColor(UI_COLOR_BLUE);
    purchase_button->setHeader("Upgrade Squadron");
    purchase_button->setAnchorPoint(VEC_CENTER);
    purchase_button->setPosition(Vec2(parentSize.width / 2, 16 + 140 / 2));
    purchase_button->onPurchase = [=]() {
        auto ship_count = Player::squadrons["Basic"].ints["count"];
        auto cost = Player::ship_costs.front();
        if (ship_count < 7) {
            if (Player::purchaseShip()) {
                addFilledShip();
            }
        }
        else {
            if (Player::bits >= cost) {
                Player::subBits(cost);
                Player::dispatchEvent(EVENT_FLEET_UNLOCKED, nullptr, true);
            }
        }
    };

    addChild(purchase_button, 3, "purchase_button");
}

void SquadronPanel::addSilhouettes()
{
    auto background = getChildByName("background");
    auto purchase_button = background->getChildByName("purchase_button");

    auto addSilhouette = [=](Vec2 pos) {
        auto silhouette = Sprite::create(UI_ICON_SILHOUETTE);
        silhouette->setScale(1.5f);
        silhouette->setPosition(pos);
        positions.emplace_back(pos);
        background->addChild(silhouette, 1);
    };

    // Build Pyramid
    auto pos = Vec2(background->getContentSize().width / 2, background->getContentSize().height - 80);
    addSilhouette(pos);
    
    const int NUM_ROWS = 3;
    for (int i = 1; i <= NUM_ROWS; i++) {
        addSilhouette(Vec2(pos.x - 144 * i, pos.y - 96 * i));
        addSilhouette(Vec2(pos.x + 144 * i, pos.y - 96 * i));
    }

    // Add in any filled ships from save
    for (int i = 0; i < Player::squadrons["Basic"].ints["count"]; i++) {
        addFilledShip();
    }
}

void SquadronPanel::addFilledShip()
{
    auto background = getChildByName("background");

    auto filledShip = Sprite::create(SPRITE_SHIP);
    filledShip->setScale(1.5f);
    filledShip->setPosition(positions.front());

    auto line = DrawNode::create(4);
    line->setPosition(positions.front());
    line->drawLine(Vec2(0, -18),
        Vec2(0, -(filledShip->getPositionY())),
        Color4F(Player::generators[BitType(7 - positions.size())].color));
    line->setScaleY(0);
    line->runAction(EaseCircleActionOut::create(ScaleTo::create(0.8f, 1, 1)));
    background->addChild(line, 1);

    positions.pop_front();
    background->addChild(filledShip, 2);
}

void SquadronPanel::updatePurchaseButton()
{
    auto purchase_button = utils::findChild<PurchaseButton*>(this, "purchase_button");
    auto ship_count = Player::squadrons["Basic"].ints["count"];
    auto cost = Player::ship_costs.front();
    purchase_button->setCost(Util::getFormattedDouble(cost));

    if (Player::bits < cost) {
        purchase_button->getChildByName("hbox")->setOpacity(255 * 0.3f);
        purchase_button->getChildByName("header_background")->setOpacity(255 * 0.3f);
    }
    else {
        purchase_button->getChildByName("hbox")->setOpacity(255);
        purchase_button->getChildByName("header_background")->setOpacity(255);
    }

    if (ship_count == 7) {
        purchase_button->setHeaderColor(Player::generators[BitType::Red].color);
        purchase_button->setHeader("Unlock Fleet");
    }
}
