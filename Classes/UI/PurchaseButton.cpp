#include "PurchaseButton.h"

#include "Util.h"
#include "Constants.h"
#include "UI\UIText.h"
#include "UI\UIHBox.h"

USING_NS_CC;

PurchaseButton* PurchaseButton::create(const std::string& path, Size size, IconType iconType) {
    PurchaseButton *btn = new (std::nothrow) PurchaseButton();
    if (btn && btn->init(path, size, iconType)) {
        btn->autorelease();
        return btn;
    }
    CC_SAFE_DELETE(btn);
    return nullptr;
}

bool PurchaseButton::init(const std::string& path, cocos2d::Size size, IconType iconType)
{
    if (!ui::Button::init(path)) return false;
    setScale9Enabled(true);
    setContentSize(size);
    setCascadeOpacityEnabled(true);
    setZoomScale(0);

    addHeaderBackground();
    addHeader();
    addCost(iconType);

    return true;
}

void PurchaseButton::setButtonColor(const cocos2d::Color4B & color)
{
    setColor(Color3B(color));
}

void PurchaseButton::setHeaderColor(const cocos2d::Color4B & color)
{
    auto node = utils::findChild(this, "header_background");
    node->setColor(Color3B(color));
}

void PurchaseButton::setHeader(const std::string& header)
{
    auto node = utils::findChild<ui::Text*>(this, "header");
    node->setString(header);
}

void PurchaseButton::setCost(const std::string& cost)
{
    auto hbox = utils::findChild(this, "hbox");
    auto label = hbox->getChildByName<ui::Text*>("label");
    label->setString(cost);

    auto iconSize = hbox->getChildByName("icon")->getContentSize();
    auto labelSize = label->getContentSize();
    hbox->setContentSize(Size(iconSize.width + labelSize.width,
        std::max(iconSize.height, labelSize.height)));
}

void PurchaseButton::addHeaderBackground()
{
    auto size = getContentSize();
    size.width -= 16;
    size.height *= 0.4f;

    auto header_background = Util::createRoundedRect(UI_ROUNDED_RECT, size, UI_COLOR_2);
    header_background->setAnchorPoint(Vec2(0, 1));
    header_background->setPosition(Vec2(8, getContentSize().height - 8));
    addChild(header_background, 0, "header_background");
}

void PurchaseButton::addHeader()
{
    auto header_background = getChildByName("header_background");
    auto size = header_background->getContentSize();
    auto header = ui::Text::create("Unnamed", FONT_DEFAULT, size.height * 0.9f);
    header->setPositionNormalized(Vec2(0.5f, 0.5f));
    header_background->addChild(header, 0, "header");
}

void PurchaseButton::addCost(IconType iconType)
{
    // Price container
    auto hbox = ui::HBox::create(getContentSize());
    hbox->setCascadeOpacityEnabled(true);
    hbox->setAnchorPoint(Vec2(0.5f, 0.5f));
    hbox->setPositionNormalized(Vec2(0.5f, 0.26f));
    auto param = ui::LinearLayoutParameter::create();
    param->setGravity(ui::LinearLayoutParameter::LinearGravity::CENTER_HORIZONTAL);
    hbox->setLayoutParameter(param);
    addChild(hbox, 0, "hbox");

    // Price icon
    auto iconPath = (iconType == IconType::Bits) ? SPRITE_BIT : SPRITE_DIAMOND;
    param = ui::LinearLayoutParameter::create();
    param->setGravity(ui::LinearLayoutParameter::LinearGravity::CENTER_VERTICAL);
    param->setMargin(ui::Margin(-8, 0, -4, 0));
    auto icon = ui::ImageView::create(iconPath);
    icon->setScale((getContentSize().height * 0.3f) / 64.0f);
    icon->setLayoutParameter(param);
    hbox->addChild(icon, 0, "icon");

    // Price label
    auto label = ui::Text::create("", FONT_DEFAULT, getContentSize().height * 0.36f);
    label->setLayoutParameter(param);
    hbox->addChild(label, 0, "label");

    hbox->setContentSize(Size(icon->getContentSize().width + label->getContentSize().width,
        std::max(icon->getContentSize().height, label->getContentSize().height)));
}
