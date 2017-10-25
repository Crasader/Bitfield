#include "SquadronPanel.h"

#include "Constants.h"
#include "Util.h"
#include "PurchaseButton.h"

USING_NS_CC;

static std::stack<Vec2> positions;

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
    setContentSize(Size(520, 588));

    addBackground();
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

void SquadronPanel::addBackground()
{
    auto background = Util::createRoundedRect(UI_ROUNDED_RECT, Size(520, 544), UI_COLOR_1);
    addChild(background, 0, "background");
}

void SquadronPanel::addPurchaseButton()
{
    auto background = getChildByName("background");
    auto parentSize = background->getContentSize();

    auto purchase_button = PurchaseButton::create(UI_ROUNDED_RECT, Size(484, 140), PurchaseButton::IconType::Bits);
    purchase_button->setButtonColor(UI_COLOR_2);
    purchase_button->setHeaderColor(UI_COLOR_BLUE);
    purchase_button->setHeader("Upgrade Squadron");
    purchase_button->setCost(25);
    purchase_button->setAnchorPoint(ANCHOR_CENTER);
    purchase_button->setPosition(Vec2(parentSize.width / 2, 16 + 140 / 2));
    purchase_button->addTouchEventListener([=](Ref* ref, ui::Widget::TouchEventType type) {
        if (type == ui::Widget::TouchEventType::BEGAN) {
            purchase_button->setScale(1.05f);
        }
        if (type == ui::Widget::TouchEventType::CANCELED) {
            purchase_button->setScale(1.0f);
        }
        if (type == ui::Widget::TouchEventType::ENDED) {
            purchase_button->setScale(1.0f);

            auto ship_count = Player::squadrons[0].ints["count"];
            auto cost = Player::ship_costs[ship_count - 1];
            if (ship_count < 6) {
                if (Player::buyShip()) {
                    addFilledShip();
                }
            }
            else {
                //Player::unlockFleetPanel();
                if (Player::bits >= cost) {
                    Player::bits -= cost;
                    cocos2d::log("UNLOCKED FLEET");
                }
            }
        }
    });

    background->addChild(purchase_button, 0, "purchase_button");
}

void SquadronPanel::addSilhouettes()
{
    auto background = getChildByName("background");
    auto purchase_button = background->getChildByName("purchase_button");

    auto addSilhouette = [=](int x, int y) {
        auto silhouette = Sprite::create(SPRITE_SILHOUETTE);
        silhouette->setScale(1.5f);
        silhouette->setPosition(x, y);
        positions.push(Vec2(x, y));
        background->addChild(silhouette);
    };

    // Build Pyramid
    const int NUM_ROWS = 3;
    for (int r = 0; r < NUM_ROWS; r++) {
        for (int c = 0; c < NUM_ROWS - r; c++) {
            if ((r % 2 == 0 && c % 2 == 0) || (r % 2 > 0 && c % 2 > 0)) {
                if (c == 0) {
                    addSilhouette(background->getContentSize().width / 2, 220 + 128 * r);
                }
                else {
                    addSilhouette(background->getContentSize().width / 2 + 92 * c, 220 + 128 * r);
                    addSilhouette(background->getContentSize().width / 2 - 92 * c, 220 + 128 * r);
                }
            }
        }
    }

    // Add in any filled ships from save
    for (int i = 0; i < Player::squadrons[0].ints["count"]; i++) {
        addFilledShip();
    }
}

void SquadronPanel::addFilledShip()
{
    auto background = getChildByName("background");

    auto filledShip = Sprite::create(SPRITE_SHIP);
    filledShip->setScale(1.5f);
    filledShip->setPosition(positions.top());
    filledShip->setOpacity(255 * 0.5f);
    filledShip->runAction(RepeatForever::create(
        Sequence::create(
            FadeIn::create(1.0f),
            DelayTime::create(1.0f + rand_minus1_1() * 0.1f),
            FadeTo::create(1.0f, 255 * 0.5f),
            nullptr))
    );
    positions.pop();
    background->addChild(filledShip);
}

void SquadronPanel::updatePurchaseButton()
{
    auto purchase_button = utils::findChild<PurchaseButton*>(this, "purchase_button");
    auto ship_count = Player::squadrons[0].ints["count"];
    auto cost = Player::ship_costs[ship_count - 1];
    purchase_button->setCost(cost);

    if (Player::bits < cost) {
        purchase_button->setOpacity(255 * 0.5f);
    }
    else {
        purchase_button->setOpacity(255);
    }

    if (ship_count == 6) {
        purchase_button->setHeaderColor(Player::bit_info[BitType::Red].color);
        purchase_button->setHeader("Unlock Fleet");
    }
}
