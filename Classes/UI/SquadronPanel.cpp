#include "SquadronPanel.h"

#include "Constants.h"
#include"Util.h"
#include "PurchaseButton.h"

USING_NS_CC;

bool SquadronPanel::init()
{
    if (!Node::init()) return false;
    setContentSize(Size(520, 588));

    addBackground();
    addPurchaseButton();
    addSilhouettes();

    return true;
}

void SquadronPanel::addBackground()
{
    auto background = Util::createRoundedRect(UI_ROUNDED_RECT, Size(520, 544), UI_COLOR_1);
    addChild(background, 0, "background");
}

void SquadronPanel::addPurchaseButton()
{
    auto parent = getChildByName("background");
    auto parentSize = parent->getContentSize();

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
        }
    });

    parent->addChild(purchase_button, 0, "purchase_button");
}

void SquadronPanel::addSilhouettes()
{
    auto background = getChildByName("background");
    auto purchase_button = background->getChildByName("purchase_button");

    auto addSilhouette = [=](int x, int y) {
        auto silhouette = Sprite::create(SPRITE_SILHOUETTE);
        silhouette->setScale(1.5f);
        silhouette->setPosition(x, y);
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
}
