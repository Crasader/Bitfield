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
std::set<std::string> Player::events_finished;

//---- Bits
double Player::bits = 0;
std::string Player::bitString = "";
double Player::all_multiplier = 1;
std::map<BitType, BitInfo> Player::generators;
BuyMode Player::buy_mode = BuyMode::One;
const int Player::LEVEL_TIER[] = { 10, 25, 50, 50 }; // Last value of x means "Every x levels, level up"

//---- Upgrades
std::map<int, Upgrade> Player::upgrades;
std::set<int> Player::upgrades_purchased;

//---- Squadron
std::map<std::string, SquadronInfo> Player::squadrons;
std::map<int, std::string> Player::squadrons_equipped;
std::list<double> Player::ship_costs;
std::list<double> Player::squadron_costs;
int Player::squadron_diamond_cost;
int Player::slot_selected = 0;

//---- PLAYER
void Player::load() {
    loadDocument();
    loadGeneral();
    loadBits();
    loadUpgrades();
    loadSquadrons();
}
void Player::loadDocument() {
    std::string filePath = FileUtils::getInstance()->getWritablePath() + SAVE_FILE;
    if (!USE_SAVE || !FileUtils::getInstance()->isFileExist(filePath)) {
        filePath = DEFAULT_FILE;
    }

    auto fileData = FileUtils::getInstance()->getStringFromFile(filePath);
    document.Parse(fileData.c_str());
    CCASSERT(!document.IsNull(), "Document failed to load.");
}
void Player::loadGeneral()
{
    bits = document["bits"].GetDouble();
    updateBitString();
    all_multiplier = document["all_multiplier"].GetDouble();
    squadron_diamond_cost = document["squadron_diamond_cost"].GetInt();

    // Ship costs
    {
        const auto& arr = document["ship_costs"].GetArray();
        for (const auto& item : arr) {
            ship_costs.push_back(item.GetDouble());
        }
    }

    // Squadron costs
    {
        const auto& arr = document["squadron_costs"].GetArray();
        for (const auto& item : arr) {
            squadron_costs.push_back(item.GetDouble());
        }
    }

    // Events
    {
        const auto& arr = document["events_finished"].GetArray();
        for (auto& item : arr) {
            events_finished.insert(item.GetString());
        }
    }
}
void Player::loadBits() {
    const auto& arr = document["resources"].GetArray();
    for (int i = 0; i < arr.Size(); i++) {
        const auto& item = arr[i];
        auto type = BitType(i);

        BitInfo info;
        info.name = item["name"].GetString();
        const auto& colors = item["color"].GetArray();
        info.color = Color4B(colors[0].GetInt(), colors[1].GetInt(), colors[2].GetInt(), colors[3].GetInt());
        info.icon_filepath = item["icon_filepath"].GetString();
        info.level = item["level"].GetInt();
        info.baseCost = item["baseCost"].GetDouble();
        info.baseValue = item["baseValue"].GetDouble();
        info.costMultiplier = item["costMultiplier"].GetDouble();
        info.valueMultiplier = item["valueMultiplier"].GetDouble();
        info.timer = item["timer"].GetDouble();
        info.spawnTime = item["spawnTime"].GetDouble();
        info.spawned = item["spawned"].GetInt();
        info.capacity = item["capacity"].GetInt();
        generators[type] = info;

        // Runtime
        generators[type].cost = calculateCost(type);
        generators[type].costString = Util::getFormattedDouble(generators[type].cost);
        generators[type].value = calculateValue(type);
        generators[type].valueString = Util::getFormattedDouble(generators[type].value);
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
        info.color = (info.bitType == BitType::All) ? Color4B(WORLD_COLOR) : generators[info.bitType].color;

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
void Player::loadSquadrons() {
    {
        const auto& arr = document["squadrons"].GetArray();
        for (const auto& squadron_info : arr) {
            SquadronInfo info;
            for (auto it = squadron_info.MemberBegin(); it != squadron_info.MemberEnd(); it++) {
                const auto& key = it->name.GetString();
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
            squadrons[info.strings["type"]] = info;
        }

        // Fill in gaps
        for (auto& pair : squadrons) {
            if (pair.first == "Basic") continue;

            auto& info = pair.second;
            SquadronInfo defaults = squadrons["Basic"];
            for (auto& pair : defaults.strings) {
                auto key = pair.first;
                if (info.strings.find(key) == info.strings.end()) {
                    info.strings[key] = defaults.strings[key];
                }
            }

            for (auto& pair : defaults.ints) {
                auto key = pair.first;
                if (info.ints.find(key) == info.ints.end()) {
                    info.ints[key] = defaults.ints[key];
                }
            }

            for (auto& pair : defaults.doubles) {
                auto key = pair.first;
                if (info.doubles.find(key) == info.doubles.end()) {
                    info.doubles[key] = defaults.doubles[key];
                }
            }
        }
    }

    {
        const auto& arr = document["squadrons_equipped"].GetArray();
        for (int i = 0; i < 7; i++) {
            squadrons_equipped[i] = arr[i].GetString();
        }
    }
}

void Player::save() {
    if (!USE_SAVE) return;

    saveGeneral();
    saveBits();
    saveUpgrades();
    saveSquadrons();

    saveDocument();
}
void Player::saveGeneral()
{
    document["bits"] = bits;
    document["all_multiplier"] = all_multiplier;

    Document::AllocatorType& allocator = document.GetAllocator();

    // Ship costs
    {
        document["ship_costs"].SetArray();
        const auto& arr = document["ship_costs"].GetArray();
        for (const auto& item : ship_costs) {
            auto val = rapidjson::Value(item);
            arr.PushBack(val, allocator);
        }
    }

    // Squadron costs
    {
        document["squadron_costs"].SetArray();
        const auto& arr = document["squadron_costs"].GetArray();
        for (const auto& item : squadron_costs) {
            auto val = rapidjson::Value(item);
            arr.PushBack(val, allocator);
        }
    }

    // Events
    {
        document["events_finished"].SetArray();
        const auto& arr = document["events_finished"].GetArray();
        for (const auto& item : events_finished) {
            auto val = rapidjson::Value(item.c_str(), allocator);
            arr.PushBack(val, allocator);
        }
    }

}
void Player::saveBits() {
    const auto& resources = document["resources"].GetArray();
    for (SizeType i = 0; i < resources.Size(); i++) {
        auto& resource = resources[i];

        BitInfo info = generators[BitType(i)];
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
void Player::saveSquadrons() {
    auto& arr = document["squadrons"];
    Document::AllocatorType& allocator = document.GetAllocator();
    for (auto it = arr.Begin(); it != arr.End(); it++) {
        auto& obj = *it;
        
        auto& typeValue = obj["type"];
        auto type = typeValue.GetString();
        SquadronInfo& info = squadrons[type];
        obj["owned"].SetInt(info.ints["owned"]);
        if (obj.HasMember("count")) {
            obj["count"].SetInt(info.ints["count"]);
        }
    }

    {
        const auto& arr = document["squadrons_equipped"].GetArray();
        for (int i = 0; i < 7; i++) {
            arr[i].SetString(squadrons_equipped[i].c_str(), allocator);
        }
    }
}
void Player::saveDocument() {
    std::ofstream outputFile;
    auto filepath = FileUtils::getInstance()->getWritablePath() + "data";
    if (!FileUtils::getInstance()->isDirectoryExist(filepath)) {
        FileUtils::getInstance()->createDirectory(filepath);
    }
    filepath += "/save.json";

    FILE *fp = fopen(filepath.c_str(), "w");
    if (!fp) {
        return;
    }

    std::string jsonObjectData = Util::jsonToString(document);
    fputs(jsonObjectData.c_str(), fp);
    fclose(fp);
}

//---- Events
void Player::dispatchEvent(const std::string& name, void* data, bool once) {
    Director::getInstance()->getEventDispatcher()->dispatchCustomEvent(name, data);
    if (once) {
        events_finished.insert(name);
        Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(name);
    }
}
bool Player::eventFinished(const std::string& event) {
    return events_finished.find(event) != events_finished.end();
}


//---- Bit Generators
void Player::addBits(double bits) {
    Player::bits = std::min<double>(Player::bits + bits, BIT_MAX);
    updateBitString();
    if (Player::bits > 50) {
        dispatchEvent(EVENT_SQUADRON_UNLOCKED, nullptr, true);
    }
}
void Player::subBits(double bits) {
    Player::bits = std::max<double>(Player::bits - bits, 0);
    updateBitString();
}
void Player::updateBitString() {
    const auto& newString = Util::getFormattedDouble(Player::bits);
    if (newString != bitString) {
        bitString = newString;
        dispatchEvent(EVENT_BITS_CHANGED);
    }
}

bool Player::purchaseBitUpgrade(BitType type) {
    auto price = calculateCost(type);
    if (bits < price) return false;

    auto& info = generators[type];
    int amount = getBuyAmount(type);
    subBits(price);

    // Apply effects
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

    // Update runtime values
    info.cost = calculateCost(type);
    info.costString = Util::getFormattedDouble(info.cost);
    info.value = calculateValue(type);
    info.valueString = Util::getFormattedDouble(info.value);

    return true;
}
int Player::getTier(BitType type) {
    auto level = generators[type].level;
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
    auto level = generators[type].level;
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
    int buyAmount = getBuyAmount(type);
    auto info = generators[type];
    if (info.level == 0) {
        return info.baseCost;
    }

    double b = info.baseCost;
    double r = info.costMultiplier;
    double k = info.level;

    double cost = b * (pow(r, k) * (pow(r, buyAmount) - 1)) / (r - 1);
    return std::min(cost, BIT_INF);
}
double Player::calculateValue(BitType type) {
    auto info = generators[type];
    return std::min(std::max(1, info.level) * info.baseValue * info.valueMultiplier * Player::all_multiplier, BIT_INF);
}
int Player::getBuyAmount(BitType type) {
    auto info = generators[type];
    if (info.level == 0) return 1;

    int amount = 0;
    switch (buy_mode) {
    case BuyMode::One: amount = 1; break;
    case BuyMode::Ten: amount = 10; break;
    case BuyMode::Fifty: amount = 50; break;
    case BuyMode::Max: amount = calculateMaxLevels(info.level, info.baseCost, info.costMultiplier); break;
    default: break;
    }
    return amount;
}
int Player::calculateMaxLevels(int level, double baseCost, double multiplier) {
    // https://www.gamasutra.com/blogs/AnthonyPecorella/20161013/282422/The_Math_of_Idle_Games_Part_I.php
    double numerator = bits * (multiplier - 1);
    double denom = baseCost * pow(multiplier, level);
    return std::min(double(9999), std::max(double(1), floor(log(numerator / denom + 1) / log(multiplier))));
}
void Player::toggleBuyMode() {
    int mode = ((int)buy_mode + 1) % (int)BuyMode::Count;
    buy_mode = BuyMode(mode);

    for (int i = 0; i < BitType::All; i++) {
        auto type = BitType(i);
        generators[type].cost = calculateCost(type);
        generators[type].costString = Util::getFormattedDouble(generators[type].cost);
    }
}

//---- Upgrades
bool Player::purchaseUpgrade(int id) {
    const auto& upgrade = upgrades[id];
    const double& cost = upgrade.cost;
    if (isUpgradePurchased(id) || bits < cost) return false;

    // Purchase the upgrade
    upgrades_purchased.insert(id);
    subBits(cost);

    // Apply the upgrade
    const auto& bitType = upgrade.bitType;
    const auto& upgradeType = upgrade.upgradeType;
    if (bitType == BitType::All) {
        Player::all_multiplier *= upgrade.value;
    }
    else {
        switch (upgradeType) {
        case UpgradeType::Value:
            generators[bitType].valueMultiplier *= upgrade.value; break;
        case UpgradeType::Capacity:
            generators[bitType].capacity += upgrade.value; break;
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
bool Player::purchaseShip() {
    if (ship_costs.empty()) return false;
    auto cost = ship_costs.front();
    if (bits < cost) return false;

    // Purchase the upgrade
    Player::subBits(cost);
    squadrons["Basic"].ints["count"]++;
    ship_costs.pop_front();

    return true;
}

bool Player::purchaseSquadron()
{
    if (squadron_costs.empty()) return false;

    auto cost = squadron_costs.front();
    if (bits < cost) return false;
    subBits(cost);
    
    auto it = squadrons.begin();
    auto index = cocos2d::random() % squadrons.size();
    for (int i = 0; i < index; i++) {
        it++;
    }
    auto pair = *it;
    squadron_costs.pop_front();

    squadrons[pair.first].ints["owned"]++;
    dispatchEvent(EVENT_SQUADRON_PURCHASED, (void*)pair.first.c_str());
    return true;
}

void Player::unlockSlot(int slot)
{
    squadrons_equipped[slot] = "Empty";
    dispatchEvent(EVENT_SLOT_CHANGED, (void*)slot);
}

bool Player::isSlotUnlocked(int slot)
{
    return squadrons_equipped[slot] != "Locked";
}

const std::string & Player::getEquippedType(int slot)
{
    return squadrons_equipped[slot];
}

void Player::equipSquadron(int slot, const std::string & type)
{
    squadrons_equipped[slot] = type;
    dispatchEvent(EVENT_SLOT_CHANGED, (void*)slot);
}

