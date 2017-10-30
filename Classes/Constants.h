#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__

#include "cocos2d.h"
#include "PlayerData/Player.h"
#include <string>
#include <vector>

//---- DEVELOPER
static const int WINDOW_WIDTH = 540;
static const int WINDOW_HEIGHT = 960;
static const bool USE_SAVE = true;
static const bool DEBUG_SHIP = false;
static const bool SHOW_FPS = false;

//---- GENERAL
static const std::string GAME_TITLE = "Bitfield";
static const int FPS = 60;
static const int GAME_WIDTH = 1080;
static const int GAME_HEIGHT = 1920;
static std::string DEFAULT_FILE = "data/default.json";
static std::string SAVE_FILE = "data/save.json";
static const std::string FONT_DEFAULT = "fonts/exprswy_free.ttf";
static const std::string BIT_STRINGS[] = {
    "Greenite", "Bluedium", "Reddite", "Yellominium", "Orangite",
    "Violum", "Indigine", "ALL"
};

//---- WORLD
static const int WORLD_WIDTH = 10000;
static const int WORLD_HEIGHT = 10000;
static const int WORLD_OFFSET = GAME_WIDTH / 2;
static const int GRID_RESOLUTION = 40;
static const int GRID_SIZE = WORLD_WIDTH / GRID_RESOLUTION;
static const float GRID_WIDTH = WORLD_WIDTH / GRID_SIZE;
static const float GRID_HEIGHT = WORLD_HEIGHT / GRID_SIZE;
static const float BIT_SCALE = 0.25f;

static const std::string SPRITE_SHIP = "sprites/ship.png";
static const std::string SPRITE_BIT = "sprites/bit.png";
static const std::string SPRITE_DIAMOND = "sprites/diamond.png";
static const cocos2d::Color4B SHIP_COLOR(225, 255, 255, 255);

//---- BIT COUNTER
const std::string ALPHABET = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
const std::string SUFFIX[] = { "K", "M", "B", "T", "Qa", "Qt", "Sx", "Sp", "Oc", "No", "Dc" };
const double BIT_MAX = 9.9999f * pow(10, 276);
const double BIT_INF = BIT_MAX + 1;

//---- UI
static const int PANEL_WIDTH = 1048;
static const int PANEL_HEIGHT = 632;
static const int SCROLL_WIDTH = 984;
static const int SCROLL_HEIGHT = 591;
static const int PANEL_Y = 98;
static const int UI_CENTER_X = GAME_WIDTH / 2;
static const int UI_CENTER_Y = GAME_HEIGHT / 2;
static const cocos2d::Vec2 VEC_CENTER(0.5f, 0.5f);
static const cocos2d::Vec2 ANCHOR_CENTER_BOTTOM(0.5f, 0);
static const cocos2d::Vec2 ANCHOR_BL(0, 0);

static const int BIT_COUNTER_SIZE = 86;
static const int PANEL_HEADER_SIZE = 84;
static const int BUY_AMOUNT_SIZE = 60;
static const int FONT_SIZE_MEDIUM = 42;
static const int BUY_BUTTON_PRICE_SIZE = 42;
static const int BUY_BUTTON_LEVELS_SIZE = 36;
static const int LEVEL_UP_SIZE = 36;
static const float BUY_BUTTON_FADE_PERCENT = 0.5f;

static const cocos2d::Color4B WORLD_COLOR(38, 40, 48, 255);
static const cocos2d::Color4B UI_COLOR_1(28, 29, 35, 255);
static const cocos2d::Color4B UI_COLOR_2(38, 40, 48, 255);
static const cocos2d::Color4B UI_COLOR_3(24, 25, 31, 255);
static const cocos2d::Color4B UI_COLOR_BLUE(79, 137, 196, 255);
static const cocos2d::Color4B UI_COLOR_RED(197, 114, 114, 255);
static const cocos2d::Color4B UI_COLOR_WHITE(245, 245, 245, 255);

static const std::string SPRITE_BLANK = "sprites/blank.png";
static const std::string SPRITE_CIRCLE = "sprites/circle.png";
static const std::string SPRITE_SILHOUETTE = "sprites/ship_silhouette.png";
static const std::string SPRITE_STREAK = "sprites/streak.png";
static const std::string UI_ROUNDED_RECT = "ui/default_9patch.png";
static const std::string UI_TAB_SELECTED = "ui/tab_selected_9patch.png";
static const std::string UI_ICON_VALUE = "ui/icon_bit_t1.png";
static const std::string UI_ICON_CAPACITY = "ui/icon_bit_t1.png";
static const std::string UI_ICON_SHIP = SPRITE_SHIP;

static const int NUM_TABS = 5;
static const std::string TAB_ICONS[NUM_TABS] = {
    "sprites/bit.png",
    "sprites/ship.png",
    "sprites/ship.png",
    "sprites/ship.png",
    "sprites/diamond.png"
};

#endif // __CONSTANTS_H__