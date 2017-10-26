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
bool Util::touch_down = false;
cocos2d::Vec2 Util::touch_location = cocos2d::Vec2(0, 0);
cocos2d::Vec2 Util::touch_location_original = cocos2d::Vec2(0, 0);


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
    button->setAnchorPoint(Vec2(0, 0));
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
    cocos2d::log("%s", jsonToString(document).c_str());
    CCASSERT(!document.IsNull(), "Document failed to load.");
    return document;
}

std::string Util::getFormattedDouble(double bits) {
    // Store the double in scientific notation: Looks like 1.23e+09 or 1.23e+308 or inf
    std::stringstream ss;
    ss << std::fixed << std::scientific << std::setprecision(4) << bits;
    auto string = ss.str();
    auto len = string.length();
    if (len <= 3) return "INF";

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
    if (exponent > 276) return "INF";

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
    else if (exponent > 276) return "INF";
    else if (exponent >= 276) return "SSS";
    else if (exponent >= 273) return "SS";
    else if (exponent >= 270) return "S";
    else {
        char first, second;
        first = ALPHABET[int(((exponent - 36) / 3) / 26) % 26];
        second = ALPHABET[((exponent - 36) / 3) % 26];
        return std::string() + first + second;
    }
}

cocos2d::Vec2 Util::capVector(cocos2d::Vec2 v, double xMin, double yMin, double xMax, double yMax)
{
    auto ret = v;
    if (ret.x < xMin) ret.x = xMin;
    if (ret.x > xMax) ret.x = xMax;
    if (ret.y < yMin) ret.y = yMin;
    if (ret.y > yMax) ret.y = yMax;
    return ret;
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