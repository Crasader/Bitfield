#pragma once

#include "cocos2d.h"
#include "rapidjson\document.h"
#include "Types.h"

class Player {
public:
    // General
    static rapidjson::Document document;
    static void load();
    static void save();
    static std::set<std::string> events_finished;

    static void dispatchEvent(const std::string& event, void* data = nullptr, bool once = false);
    static bool eventFinished(const std::string& event);
    
    // Bits
    static double bits;
    static std::string bitString;
    static double all_multiplier;
    static std::map<BitType, BitInfo> generators;
    static BuyMode buy_mode;
    static const int LEVEL_TIER[];

    static void addBits(double bits);
    static void subBits(double bits);
    static void updateBitString();

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
    static std::map<std::string, SquadronInfo> squadrons;
    static std::map<int, std::string> squadrons_equipped;
    static std::list<double> ship_costs;
    static std::list<double> squadron_costs;
    static int squadron_diamond_cost;

    static bool purchaseShip();
    static bool purchaseSquadron();
    static void unlockSlot(int slot);
    static bool isSlotUnlocked(int slot);

private:
    static void loadDocument();
    static void loadGeneral();
    static void loadUpgrades();
    static void loadBits();
    static void loadSquadrons();

    static void saveGeneral();
    static void saveBits();
    static void saveUpgrades();
    static void saveSquadrons();
    static void saveDocument();
};