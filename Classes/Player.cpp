#include "Player.h"

#include "Constants.h"
#include "GameObject/Bit.h"

#include "rapidjson\prettywriter.h"
#include "rapidjson\stringbuffer.h"

#include <fstream>
#include <iostream>
#include <iomanip>
#include <algorithm>

USING_NS_CC;
using namespace rapidjson;

//---- General
Document Player::document;
double Player::bits = 0;
double Player::time_played = 0;

// ---- Resources
double Player::all_multiplier = 1;
std::map<BitType, BitInfo> Player::bit_info;
BuyMode Player::buy_mode = BuyMode::One;
const int Player::LEVEL_TIER[] = { 10, 25, 50, 50 }; // Last value of x means "Every x levels, level up"

//---- Upgrades
std::map<int, UpgradeInfo> Player::upgrade_info;
std::set<int> Player::upgrades_purchased;

//---- Squadron
float Player::alignment = 0.0f;
float Player::cohesion = 0.0f;
float Player::separation = 1.0f;
float Player::wander = 1.0f;
float Player::seek = 1.6f;
float Player::seekBits = 1.2f;
int Player::ship_count = 1;
float Player::ship_speed = 5.8f;
float Player::ship_force = 0.16f;
float Player::ship_vision = 400;
float Player::ship_separation = 72;

bool Player::touch_down = false;
cocos2d::Vec2 Player::touch_location = cocos2d::Vec2(0, 0);

//---- Utility
static std::string jsonToString(rapidjson::Document &jsonObject) {
    StringBuffer buffer;
    PrettyWriter<StringBuffer> jsonWriter(buffer);
    jsonObject.Accept(jsonWriter);
    return buffer.GetString();
}

//---- PLAYER
void Player::load() {
    loadDocument();

    bits = document["bits"].GetDouble();
    ship_count = document["ship_count"].GetDouble();
    time_played = document["time_played"].GetDouble();
    all_multiplier = document["all_multiplier"].GetDouble();
    loadBits();
    loadUpgrades();
}
void Player::loadDocument() {
    std::string filePath = FileUtils::getInstance()->getWritablePath() + SAVE_FILE;
    if (!USE_SAVE || !FileUtils::getInstance()->isFileExist(filePath)) {
        cocos2d::log("Loading defaults from %s...", filePath.c_str());
        filePath = DEFAULT_FILE;
    }
    else {
        cocos2d::log("Loading save from %s...", filePath.c_str());
    }

    auto fileData = FileUtils::getInstance()->getStringFromFile(filePath);
    document.Parse(fileData.c_str());
    cocos2d::log("%s", jsonToString(document).c_str());
    CCASSERT(!document.IsNull(), "Document failed to load.");
}
void Player::loadBits() {
    const auto& resources = document["resources"].GetArray();
    for (SizeType i = 0; i < resources.Size(); i++) {
        const auto& resource = resources[i];

        BitInfo info;
        info.name = resource["name"].GetString();
        const auto& colors = resource["color"].GetArray();
        info.color = Color4B(colors[0].GetInt(), colors[1].GetInt(), colors[2].GetInt(), colors[3].GetInt());
        info.icon_filepath = resource["icon_filepath"].GetString();
        info.level = resource["level"].GetInt();
        info.baseCost = resource["baseCost"].GetDouble();
        info.baseValue = resource["baseValue"].GetDouble();
        info.costMultiplier = resource["costMultiplier"].GetDouble();
        info.valueMultiplier = resource["valueMultiplier"].GetDouble();
        info.timer = resource["timer"].GetDouble();
        info.spawnTime = resource["spawnTime"].GetDouble();
        info.spawned = resource["spawned"].GetInt();
        info.capacity = resource["capacity"].GetInt();

        bit_info[BitType(i)] = info;
    }
}
void Player::loadUpgrades() {
    // Load purchased upgrades
    const auto& purchased = document["upgrades_purchased"].GetArray();
    for (SizeType i = 0; i < purchased.Size(); i++) {
        auto id = purchased[i].GetInt();
        upgrades_purchased.insert(id);
        cocos2d::log("%d already purchased.", i);
    }

    const auto& upgrades = document["upgrades"].GetArray();
    for (SizeType i = 0; i < upgrades.Size(); i++) {
        // Skip purchased upgrades
        if (isUpgradePurchased(i)) {
            cocos2d::log("Already bought %d, skipping", i);
            continue;
        }

        const auto& values = upgrades[i].GetArray();
        UpgradeInfo info;
        info.cost = values[0].GetDouble();
        info.bitType = BitType(values[1].GetInt());
        info.upgradeType = UpgradeType(values[2].GetInt());
        info.value = values[3].GetDouble();

        // Identifier and Icon Path
        std::stringstream ss;
        std::string identifier;
        switch (info.upgradeType) {
        case UpgradeType::Value:
            identifier = "Value";
            info.icon_filepath = UI_ICON_VALUE;
            break;
        case UpgradeType::Capacity:
            identifier = "Capacity";
            info.icon_filepath = UI_ICON_CAPACITY;
            break;
        default: break;
        }
        info.color = (info.bitType == BitType::All) ? Color4B(WORLD_COLOR) : bit_info[info.bitType].color;

        // Name
        ss << BIT_STRINGS[info.bitType] << " " << identifier;
        info.name = ss.str();
        ss.str("");

        // Description
        std::transform(identifier.begin(), identifier.end(), identifier.begin(), tolower);
        ss << "Increase the " << identifier << " of " << BIT_STRINGS[info.bitType] << " by " <<
            (info.upgradeType == UpgradeType::Value ? "x" : "") << info.value << ".";
        info.desc = ss.str();

        upgrade_info[i] = info;
    }
}

void Player::save() {
    if (!USE_SAVE) return;

    document["bits"] = bits;
    document["ship_count"] = ship_count;
    document["time_played"] = time_played;
    document["all_multiplier"] = all_multiplier;
    saveBits();
    saveUpgrades();

    // Write to file
    std::ofstream outputFile;
    auto filepath = FileUtils::getInstance()->getWritablePath() + "data";
    if (!FileUtils::getInstance()->isDirectoryExist(filepath)) {
        FileUtils::getInstance()->createDirectory(filepath);
        cocos2d::log("Created directory %s", filepath.c_str());
    }
    filepath += "/save.json";

    FILE *fp = fopen(filepath.c_str(), "w");
    if (!fp) {
        cocos2d::log("Could not create file %s", filepath.c_str());
        return;
    }

    std::string jsonObjectData = jsonToString(document);
    fputs(jsonObjectData.c_str(), fp);
    fclose(fp);
}
void Player::saveBits() {
    const auto& resources = document["resources"].GetArray();
    for (SizeType i = 0; i < resources.Size(); i++) {
        auto& resource = resources[i];

        BitInfo info = bit_info[BitType(i)];
        resource["level"] = info.level;
        resource["valueMultiplier"] = info.valueMultiplier;
        resource["timer"] = info.timer;
        resource["spawnTime"] = info.spawnTime;
        resource["spawned"] = info.spawned;
        resource["capacity"] = info.capacity;
    }
}
void Player::saveUpgrades() {
    document["upgrades_purchased"].SetArray();
    const auto& purchased = document["upgrades_purchased"].GetArray();
    
    Document::AllocatorType& allocator = document.GetAllocator();
    for (auto id : upgrades_purchased) {
        purchased.PushBack(id, allocator);
    }
}

void Player::addBits(double bits) {
    Player::bits = std::min<double>(Player::bits + bits, BIT_MAX);
}
void Player::subBits(double bits) {
    Player::bits = std::max<double>(Player::bits - bits, 0);
}

//---- Resources
bool Player::purchaseBitUpgrade(BitType type) {
    auto price = calculatePrice(type);
    if (bits < price) return false;

    auto& info = bit_info[type];
    int amount = getBuyAmount(type);
    subBits(price);
    
    while (amount > 0) {
        info.level++;
        if (info.level == LEVEL_TIER[0]) {
            info.spawnTime /= 2.0f;
        }
        if (info.level == LEVEL_TIER[1]) {
            info.spawnTime /= 2.0f;
        }
        if (info.level == LEVEL_TIER[2]) {
            info.spawnTime /= 2.0f;
        }
        else if (info.level >= LEVEL_TIER[2] + LEVEL_TIER[3] && info.level % LEVEL_TIER[3] == 0) {
            info.valueMultiplier *= 3;
        }
        amount--;
    }
    return true;
}
std::string Player::getFormattedBits(double bits) {
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
std::string Player::getSuffix(int exponent) {
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
int Player::getTier(BitType type) {
    auto level = bit_info[type].level;
    if (level >= LEVEL_TIER[2]) {
        return level - (level % LEVEL_TIER[3]);
    }
    else if (level >= LEVEL_TIER[1]) {
        return LEVEL_TIER[1];
    }
    else if (level >= LEVEL_TIER[0]) {
        return LEVEL_TIER[0];
    }
    else {
        return 0;
    }
}
int Player::getNextTier(BitType type) {
    auto level = bit_info[type].level;
    if (level >= LEVEL_TIER[2]) {
        return getTier(type) + LEVEL_TIER[3];
    }
    else if (level >= LEVEL_TIER[1]) {
        return LEVEL_TIER[2];
    }
    else if (level >= LEVEL_TIER[0]) {
        return LEVEL_TIER[1];
    }
    else {
        return LEVEL_TIER[0];
    }
}
double Player::calculatePrice(BitType type) {
    int amount = getBuyAmount(type);
    auto info = bit_info[type];

    if (info.level == 0) {
        return info.baseCost;
    }

    double ret = 0;
    for (int i = 0; i < amount; i++) {
        ret += info.baseCost * (pow(info.costMultiplier, info.level));
        info.level++;
    }
    return std::min(ret, BIT_INF);
}
double Player::calculateValue(BitType type) {
    auto info = bit_info[type];
    return std::min(std::max(1, info.level) * info.baseValue * info.valueMultiplier * Player::all_multiplier, BIT_MAX);
}
int Player::getBuyAmount(BitType type) {
    auto info = bit_info[type];
    if (info.level == 0) return 1;

    int amount = 0;
    switch (buy_mode) {
    case BuyMode::One: amount = 1; break;
    case BuyMode::Ten: amount = 10; break;
    case BuyMode::Hundred: amount = 100; break;
    case BuyMode::Max: amount = calculateMaxLevels(info.level, info.baseCost, info.costMultiplier); break;
    default: break;
    }
    return amount;
}
int Player::calculateMaxLevels(int level, double baseCost, double multiplier) {
    int max_levels = 0;
    double cost = 0;
    
    do {
        max_levels++;
        cost += baseCost * pow(multiplier, level);
        level++;
    } while ((cost + baseCost * pow(multiplier, level)) <= Player::bits);
    
    return max_levels;
}
void Player::toggleBuyMode() {
    int mode = ((int)buy_mode + 1) % (int)BuyMode::Count;
    buy_mode = BuyMode(mode);
}

//---- Upgrades
bool Player::purchaseUpgrade(int id) {
    const auto& upgrade = upgrade_info[id];
    const double& cost = upgrade.cost;
    if (isUpgradePurchased(id) || bits < cost) return false;

    // Purchase the upgrade
    upgrades_purchased.insert(id);
    bits -= cost;

    // Apply the upgrade
    const auto& bitType = upgrade.bitType;
    const auto& upgradeType = upgrade.upgradeType;
    if (bitType == BitType::All) {
        Player::all_multiplier *= upgrade.value;
    }
    else {
        switch (upgradeType) {
        case UpgradeType::Value:
            bit_info[bitType].valueMultiplier *= upgrade.value; break;
        case UpgradeType::Capacity:
            bit_info[bitType].capacity += upgrade.value; break;
        default: break;
        }
    }
    return true;
}
bool Player::canBuyUpgrade() {
    for (auto& upgrade : upgrade_info) {
        if (isUpgradePurchased(upgrade.first)) continue;
        return Player::bits >= upgrade.second.cost;
    }
    return false;
}
bool Player::isUpgradePurchased(int id) {
    return upgrades_purchased.find(id) != upgrades_purchased.end();
}