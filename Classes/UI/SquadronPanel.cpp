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
    auto background = Util::createRoundedRect(UI_ROUNDED_RECT, Size(520, 588), UI_COLOR_1);
    addChild(background, 0, "background");
}

void SquadronPanel::addPurchaseButton()
{
    auto parent = getChildByName("background");
    auto parentSize = parent->getContentSize();

    auto purchase_button = PurchaseButton::create(Size(484, 140));
    purchase_button->setButtonColor(UI_COLOR_2);
    purchase_button->setHeaderColor(UI_COLOR_BLUE);
    purchase_button->setHeader("Upgrade Squadron");
    purchase_button->setCost(25);
    purchase_button->setAnchorPoint(ANCHOR_CENTER_BOTTOM);
    purchase_button->setPosition(Vec2(parentSize.width / 2, 16));
    parent->addChild(purchase_button);
}

void SquadronPanel::addSilhouettes()
{
}
