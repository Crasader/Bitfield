#pragma once

#include "cocos2d.h"
#include "rapidjson\document.h"
#include <map>
#include <set>
#include "Types.h"

class Player {
public:
    // Loading and Saving
    static rapidjson::Document document;
    static void load();
    static void save();
    
    // Bits
    static double bits;
    static double all_multiplier;
    static std::map<BitType, BitInfo> bit_info;
    static BuyMode buy_mode;
    static const int LEVEL_TIER[];

    static void addBits(double bits);
    static void subBits(double bits);
    static int getTier(BitType type);
    static int getNextTier(BitType type);
    static double calculateCost(BitType type);
    static double calculateValue(BitType type);
    static int getBuyAmount(BitType type);
    static int calculateMaxLevels(int level, double baseCost, double multiplier);
    static bool purchaseBitUpgrade(BitType type);
    static void toggleBuyMode();

    // Upgrades
    static std::map<int, Upgrade> upgrades;
    static std::set<int> upgrades_purchased;
    static bool purchaseUpgrade(int id);
    static bool canBuyUpgrade();
    static bool isUpgradePurchased(int id);

    // Squadron
    static std::map<std::string, SquadronInfo> squadron_defaults;
    static std::map<int, SquadronInfo> squadrons;
    static int squadron_slots;
    static double ship_costs[7];
    static bool buyShip();

private:
    static void loadDocument();
    static void loadUpgrades();
    static void loadBits();
    static void loadSquadronDefaults();
    static void loadSquadrons();

    static void saveBits();
    static void saveUpgrades();
    static void saveSquadrons();
    static void saveDocument();
};