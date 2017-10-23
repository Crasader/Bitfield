#pragma once

enum BuyMode {
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