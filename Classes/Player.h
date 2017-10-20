#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "cocos2d.h"
#include "rapidjson\document.h"
#include <string>
#include <map>
#include <set>

enum class BuyMode {
    One,
    Ten,
    Hundred,
    Max,
    Count
};

enum BitType {
    Green,
    Blue,
    Red,
    Yellow,
    Orange,
    Violet,
    Indigo,
    All
};

struct BitInfo {
    std::string name;
    std::string icon_filepath;
    cocos2d::Color4B color;
    int level;
    double baseCost;
    double baseValue;
    double costMultiplier;
    double valueMultiplier;
    double timer;
    double spawnTime;
    int spawned;
    int capacity;
};

enum UpgradeType {
    Value,
    Capacity
};

struct UpgradeInfo {
    double cost;
    BitType bitType;
    UpgradeType upgradeType;
    double value;

    std::string name;
    std::string desc;
    std::string icon_filepath;
    cocos2d::Color4B color;
};

class Player {
public:
    static rapidjson::Document document;
    static double bits;
    static double time_played;

    static void save();
    static void load();

    static void addBits(double bits);
    static void subBits(double bits);
    static std::string getFormattedBits(double bits);
    static std::string getSuffix(int exponent);

    // Resources
    static double all_multiplier;
    static std::map<BitType, BitInfo> bit_info;
    static BuyMode buy_mode;
    static const int LEVEL_TIER[];

    static int getTier(BitType type);
    static int getNextTier(BitType type);
    static double calculatePrice(BitType type);
    static double calculateValue(BitType type);
    static int getBuyAmount(BitType type);
    static int calculateMaxLevels(int level, double baseCost, double multiplier);
    static bool purchaseBitUpgrade(BitType type);
    static void toggleBuyMode();

    // Upgrades
    static std::map<int, UpgradeInfo> upgrade_info;
    static std::set<int> upgrades_purchased;
    static bool purchaseUpgrade(int id);
    static bool canBuyUpgrade();
    static bool isUpgradePurchased(int id);

    // Eventually squadron_info;
    static float alignment;
    static float cohesion;
    static float separation;
    static float wander;
    static float seek;
    static float seekBits;
    static int ship_count;
    static float ship_speed;
    static float ship_force;
    static float ship_vision;
    static float ship_separation;

    static bool touch_down;
    static cocos2d::Vec2 touch_location;

private:
    static void loadDocument();
    static void loadUpgrades();
    static void loadBits();

    static void saveBits();
    static void saveUpgrades();
};

#endif // __PLAYER_H__
