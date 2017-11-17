#include "Util.h"

#include "Constants.h"
#include "rapidjson\prettywriter.h"
#include "rapidjson\stringbuffer.h"
#include "rapidjson\document.h"

#include <fstream>
#include <iostream>
#include <iomanip>
#include <algorithm>

#include "UI/UIHBox.h"
#include "UI/UIText.h"

USING_NS_CC;
using namespace rapidjson;

static const cocos2d::Rect CAP_INSETS(48, 48, 48, 48);


ui::ImageView* Util::createRoundedRect(const std::string& path, cocos2d::Size size, cocos2d::Color4B color) {
    auto rect = ui::ImageView::create(path);
    rect->setScale9Enabled(true);
    //rect->setCapInsets(CAP_INSETS);
    rect->setContentSize(size);
    rect->setColor(Color3B(color));
    rect->setCascadeOpacityEnabled(true);
    rect->setAnchorPoint(Vec2(0, 0));
    return rect;
}

ui::Button* Util::createRoundedButton(const std::string& path, cocos2d::Size size, cocos2d::Color4B color) {
    auto button = ui::Button::create(path);
    button->setScale9Enabled(true);
    //button->setCapInsets(CAP_INSETS);
    button->setContentSize(size);
    button->setColor(Color3B(color));
    button->setCascadeOpacityEnabled(true);
    //button->setAnchorPoint(Vec2(0, 0));
    button->setZoomScale(0);
    return button;
}

std::string Util::jsonToString(rapidjson::Document &jsonObject) {
    StringBuffer buffer;
    PrettyWriter<StringBuffer> jsonWriter(buffer);
    jsonObject.Accept(jsonWriter);
    return buffer.GetString();
}

rapidjson::Document Util::loadDocument(const std::string& path) {
    auto fileData = FileUtils::getInstance()->getStringFromFile(path);
    Document document;
    document.Parse(fileData.c_str());
    CCASSERT(!document.IsNull(), "Document failed to load.");
    return document;
}

std::string Util::getFormattedDouble(double bits) {
    if (bits > BIT_MAX) return "n/a";

    // Store the double in scientific notation: Looks like 1.23e+09 or 1.23e+308 or inf
    std::stringstream ss;
    if (bits < 1000 && (bits == floor(bits) || ceil(bits) == 3)) {
        ss << std::fixed << std::setprecision(0) << bits;
        return ss.str();
    }
    ss << std::fixed << std::scientific << std::setprecision(4) << bits;
    auto string = ss.str();
    auto len = string.length();

    // Extract the number part
    double number = 0;
    number += string[0] - '0';
    for (int i = 2; i <= 5; i++) {
        number += (string[i] - '0') / pow(10, i - 1);
    }

    // Extract the exponent part
    int exponent = 0;
    if (len == 11) {
        exponent += (string[8] - '0') * 100;
        exponent += (string[9] - '0') * 10;
        exponent += (string[10] - '0');
    }
    else {
        exponent += (string[8] - '0') * 10;
        exponent += (string[9] - '0');
    }
    if (string[7] == '-') exponent *= -1;

    std::stringstream outstream;
    outstream << std::setprecision(3) << std::fixed << std::setprecision(2);
    if (exponent < 3) {
        outstream << (number * pow(10, exponent));
    }
    else {
        outstream << (number * pow(10, exponent % 3)) << getSuffix(exponent);
    }
    return outstream.str();
}

std::string Util::getSuffix(int exponent) {
    if (exponent < 3) return "";
    else if (exponent <= 35) return SUFFIX[(exponent - 3) / 3]; // Standard suffixes up to Decillion
    else if (exponent > 279) return "n/a";
    else if (exponent >= 276) return "sss";
    else if (exponent >= 273) return "ss";
    else if (exponent >= 270) return "s";
    else {
        char first, second;
        first = ALPHABET[int(((exponent - 36) / 3) / 26) % 26];
        second = ALPHABET[((exponent - 36) / 3) % 26];
        return std::string() + first + second;
    }
}

void Util::capVector(cocos2d::Vec2& v, double minX, double maxX, double minY,  double maxY)
{
    if (v.x < minX) v.x = minX;
    if (v.x > maxX) v.x = maxX;
    if (v.y < minY) v.y = minY;
    if (v.y > maxY) v.y = maxY;
}

void Util::capVector(cocos2d::Vec2& v, cocos2d::Rect rect)
{
    capVector(v, rect.getMinX(), rect.getMaxX(), rect.getMinY(), rect.getMaxY());
}

cocos2d::Node* Util::createIconLabel(int iconType, double amount, double size) {
    // Price container
    auto container = ui::HBox::create(Size(GAME_WIDTH, GAME_HEIGHT));
    container->setCascadeOpacityEnabled(true);
    container->setCascadeColorEnabled(true);
    container->setAnchorPoint(Vec2(0.5f, 0.5f));
    //auto param = ui::LinearLayoutParameter::create();
    //param->setGravity(ui::LinearLayoutParameter::LinearGravity::CENTER_HORIZONTAL);
    //container->setLayoutParameter(param);

    // Price icon
    auto iconPath = (iconType == 0) ? SPRITE_BIT : SPRITE_DIAMOND;
    auto param = ui::LinearLayoutParameter::create();
    param->setGravity(ui::LinearLayoutParameter::LinearGravity::CENTER_VERTICAL);
    param->setMargin(ui::Margin(0, 0, -8, 0));
    auto icon = ui::ImageView::create(iconPath);
    icon->setScale(size / 64.0f);
    icon->setLayoutParameter(param);
    container->addChild(icon, 0, "icon");

    // Price label
    auto label = ui::Text::create(getFormattedDouble(amount), FONT_DEFAULT, size);
    label->setLayoutParameter(param);
    container->addChild(label, 0, "label");

    container->setContentSize(Size(icon->getContentSize().width + label->getContentSize().width, size));
    return container;
}

