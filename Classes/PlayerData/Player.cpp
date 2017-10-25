#include "Player.h"

#include "Util.h"
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

//---- Bits
double Player::bits = 0;
double Player::all_multiplier = 1;
std::map<BitType, BitInfo> Player::bit_info;
BuyMode Player::buy_mode = BuyMode::One;
const int Player::LEVEL_TIER[] = { 10, 25, 50, 50 }; // Last value of x means "Every x levels, level up"

//---- Upgrades
std::map<int, Upgrade> Player::upgrades;
std::set<int> Player::upgrades_purchased;

//---- Squadron
std::map<std::string, SquadronInfo> Player::squadron_defaults;
std::map<int, SquadronInfo> Player::squadrons;
int Player::squadron_slots;
double Player::ship_costs[6];

//---- PLAYER
void Player::load() {
    loadDocument();

    bits = document["bits"].GetDouble();
    all_multiplier = document["all_multiplier"].GetDouble();
    loadBits();
    loadUpgrades();
    loadSquadronDefaults();
    loadSquadrons();
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
    }

    const auto& upgradeArray = document["upgrades"].GetArray();
    for (SizeType i = 0; i < upgradeArray.Size(); i++) {
        // Skip purchased upgrades
        if (isUpgradePurchased(i)) continue;

        const auto& values = upgradeArray[i].GetArray();
        Upgrade info;
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

        upgrades[i] = info;
    }
}
void Player::loadSquadronDefaults() {
    const auto& category = document["squadron_defaults"].GetArray();
    for (SizeType i = 0; i < category.Size(); i++) {
        const auto& item = category[i];

        SquadronInfo info;
        for (auto it = item.MemberBegin(); it != item.MemberEnd(); it++) {
            auto key = it->name.GetString();
            const auto& value = it->value;

            if (value.IsString()) {
                info.strings[key] = value.GetString();
            } else if (value.IsInt()) {
                info.ints[key] = value.GetInt();
            } else if (value.IsDouble()) {
                info.doubles[key] = value.GetDouble();
            }
        }

        squadron_defaults[info.strings["type"]] = info;
    }

    const auto& costs = document["ship_costs"].GetArray();
    for (int i = 0; i < 6; i++) {
        const auto& cost = costs[i];
        ship_costs[i] = cost.GetDouble();
    }

    squadron_slots = document["squadron_slots"].GetInt();
}
void Player::loadSquadrons() {
    const auto& category = document["squadrons"].GetArray();
    for (SizeType i = 0; i < category.Size(); i++) {
        const auto& item = category[i];
        auto type = item["type"].GetString();

        // Load defaults and overwrite any differences
        SquadronInfo info = squadron_defaults["Default"];
        for (auto it = item.MemberBegin(); it != item.MemberEnd(); it++) {
            auto key = it->name.GetString();
            const auto& value = it->value;

            if (value.IsString()) {
                info.strings[key] = value.GetString();
            }
            else if (value.IsInt()) {
                info.ints[key] = value.GetInt();
            }
            else if (value.IsDouble()) {
                info.doubles[key] = value.GetDouble();
            }
        }

        squadrons[i] = info;
    }
}

void Player::save() {
    if (!USE_SAVE) return;

    document["bits"] = bits;
    document["all_multiplier"] = all_multiplier;
    saveBits();
    saveUpgrades();
    saveDocument();
    saveSquadrons();
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
void Player::saveDocument() {
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

    std::string jsonObjectData = Util::jsonToString(document);
    fputs(jsonObjectData.c_str(), fp);
    fclose(fp);
}
void Player::saveSquadrons() {

}

//---- Bit Generators
void Player::addBits(double bits) {
    Player::bits = std::min<double>(Player::bits + bits, BIT_MAX);
}
void Player::subBits(double bits) {
    Player::bits = std::max<double>(Player::bits - bits, 0);
}
bool Player::purchaseBitUpgrade(BitType type) {
    auto price = calculateCost(type);
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
double Player::calculateCost(BitType type) {
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
    const auto& upgrade = upgrades[id];
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
    for (auto& upgrade : upgrades) {
        if (isUpgradePurchased(upgrade.first)) continue;
        return Player::bits >= upgrade.second.cost;
    }
    return false;
}
bool Player::isUpgradePurchased(int id) {
    return upgrades_purchased.find(id) != upgrades_purchased.end();
}

//---- Squadrons
bool Player::buyShip() {
    auto count = squadrons[0].ints["count"];
    auto cost = ship_costs[count - 1];
    if (count >= 7 || bits < cost) return false;

    // Purchase the upgrade
    bits -= cost;
    squadrons[0].ints["count"]++;
    return true;
}