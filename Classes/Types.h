#pragma once

#include <cocos2d.h>

#include <string>
#include <map>

enum BuyMode {
    One,
    Ten,
    Fifty,
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

    // Runtime, not saved
    double cost;
    double value;
    std::string costString;
    std::string valueString;
};

enum UpgradeType {
    Value,
    Capacity
};

struct Upgrade {
    double cost;
    BitType bitType;
    UpgradeType upgradeType;
    double value;

    std::string name;
    std::string desc;
    std::string icon_filepath;
    cocos2d::Color4B color;
};

struct SquadronInfo {
    std::map<std::string, std::string> strings;
    std::map<std::string, int> ints;
    std::map<std::string, double> doubles;
};

struct StreakInfo {
    float fade;
    float minSeg;
    float stroke;
    cocos2d::Color3B color;
    std::string path;
};