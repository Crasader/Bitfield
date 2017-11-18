#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__

#include "cocos2d.h"
#include "PlayerData/Player.h"
#include <string>
#include <vector>


//---- DEVELOPER
#define SHOW_FPS true

static const int WINDOW_WIDTH = 540;
static const int WINDOW_HEIGHT = 960;
static const bool USE_SAVE = false;
static const bool DEBUG_SHIP = false;

//---- GENERAL
static const std::string GAME_TITLE = "Bitfield";
static const int FPS = 60;
static int GAME_WIDTH = 1080;
static int GAME_HEIGHT = 1920;
static std::string DEFAULT_FILE = "data/default.json";
static std::string SAVE_FILE = "data/save.json";
static const std::string FONT_DEFAULT = "fonts/exprswy_free.ttf";
static const std::string BIT_STRINGS[] = {
    "Greenite", "Bluedium", "Reddite", "Yellominium", "Orangite",
    "Violum", "Indigine", "ALL"
};

//---- WORLD
static const int WORLD_WIDTH = 12000;
static const int WORLD_HEIGHT = 12000;
static const int WORLD_OFFSET = GAME_WIDTH / 2;
static const int GRID_RESOLUTION = 40;
static const int GRID_SIZE = WORLD_WIDTH / GRID_RESOLUTION;
static const float BIT_SCALE = 0.25f;

static const std::string SPRITE_SHIP = "sprites/ship_basic.png";
static const std::string SPRITE_BIT = "sprites/bit.png";
static const std::string SPRITE_GLOW = "sprites/glow.png";
static const std::string SPRITE_DIAMOND = "sprites/diamond.png";
static const std::string SPRITE_STREAK = "sprites/streak.png";
static const cocos2d::Color4B SHIP_COLOR(225, 255, 255, 255);

//---- BIT COUNTER
const std::string ALPHABET = "abcdefghijklmnopqrstuvwxyz";
const std::string SUFFIX[] = { "k", "m", "b", "t", "qa", "qt", "sx", "sp", "oc", "no", "dc" };
const double BIT_MAX = 9.9999f * pow(10, 278);
const double BIT_INF = 1.00f * pow(10, 279);

//---- UI
static const int SCROLL_WIDTH = 984;
static const int SCROLL_HEIGHT = 591;
static const int PANEL_Y = 98;
static const int UI_CENTER_X = GAME_WIDTH / 2;
static const int UI_CENTER_Y = GAME_HEIGHT / 2;
static const cocos2d::Vec2 VEC_ZERO(0, 0);
static const cocos2d::Vec2 VEC_CENTER(0.5f, 0.5f);
static const cocos2d::Vec2 VEC_BOTTOM(0.5f, 0);
static const cocos2d::Vec2 VEC_TOP(0.5f, 1);

static const int FONT_SIZE_HUGE = 86;
static const int FONT_SIZE_LARGE = 64;
static const int FONT_SIZE_MEDIUM = 42;
static const int FONT_SIZE_SMALL = 30;

static const cocos2d::Color4B WORLD_COLOR(38, 40, 51, 255);
static const cocos2d::Color4F GRID_COLOR(1, 1, 1, 0.2f);
static const cocos2d::Color4B UI_COLOR_1(28, 29, 35, 255);
static const cocos2d::Color4B UI_COLOR_2(38, 40, 51, 255);
static const cocos2d::Color4B UI_COLOR_3(24, 25, 31, 255);
static const cocos2d::Color4B UI_COLOR_BLUE(79, 137, 196, 255);
static const cocos2d::Color4B UI_COLOR_RED(197, 114, 114, 255);
static const cocos2d::Color4B UI_COLOR_WHITE(245, 245, 245, 255);
static const cocos2d::Color4B UI_COLOR_INDIGO(114, 124, 196, 255);
static const float OPACITY_FULL = 255;
static const float OPACITY_HALF = 255 * 0.5f;
static const float OPACITY_UI = 255 * 0.95f;
static const float OPACITY_UI_TABS = 255 * 0.8f;
static const float OPACITY_UNFOCUS = 255 * 0.7f;

static const std::string UI_ICON_CIRCLE = "sprites/circle.png";
static const std::string UI_ICON_SILHOUETTE = "sprites/ship_silhouette.png";
static const std::string UI_ROUNDED_RECT = "ui/default_9patch.png";
static const std::string UI_TAB_SELECTED = "ui/tab_selected_9patch.png";
static const std::string UI_ICON_VALUE = "ui/icon_bit_t1.png";
static const std::string UI_ICON_CAPACITY = "ui/icon_bit_t1.png";
static const std::string UI_ICON_LOCK = "ui/lockOLD.png";
static const std::string UI_ICON_PLUS = "ui/plus.png";

static const cocos2d::Size UI_SIZE_PANEL(1048, 632);
static const cocos2d::Size UI_SIZE_PANEL_TABS(1048, 648);
static const cocos2d::Size UI_SIZE_TAB_LAYER(1080, 98);

//----FLEET PANEL
static const cocos2d::Size UI_SIZE_FLEET_SLOT_BACK(128, 160);
static const cocos2d::Size UI_SIZE_FLEET_SLOT_FRONT(124, 128);

static const cocos2d::Size UI_SIZE_FLEET_PANEL(1010, 280);
static const cocos2d::Size UI_SIZE_FLEET_SCROLLVIEW(994, 280);
//static const cocos2d::Size UI_OFFSET_FLEET_SCROLLVIEW(8, 24);

static const cocos2d::Size UI_SIZE_FLEET_BUTTON(495, 128);


static const int NUM_TABS = 5;
static const std::string TAB_ICONS[NUM_TABS] = {
    SPRITE_BIT,
    SPRITE_SHIP,
    SPRITE_SHIP,
    SPRITE_SHIP,
    SPRITE_DIAMOND
};

static const cocos2d::Vec2 POLYGON_VERTS[4] = {
    cocos2d::Vec2(1, 0),
    cocos2d::Vec2(0, 1),
    cocos2d::Vec2(1, 2),
    cocos2d::Vec2(2, 1)
};

// One-time Events
static const std::string EVENT_SQUADRON_UNLOCKED = "EVENT_SQUADRON_UNLOCKED";
static const std::string EVENT_FLEET_UNLOCKED = "EVENT_FLEET_UNLOCKED";
static const std::string EVENT_ALL_GENERATORS_UNLOCKED = "EVENT_ALL_GENERATORS_UNLOCKED";
static const std::string EVENT_ALL_SLOTS_UNLOCKED = "EVENT_ALL_SLOTS_UNLOCKED";

// Recurring Events
static const std::string EVENT_BITS_CHANGED = "EVENT_BITS_CHANGED";
static const std::string EVENT_GENERATOR_UNLOCKED = "EVENT_GENERATOR_UNLOCKED";
static const std::string EVENT_SLOT_UNLOCKED = "EVENT_SLOT_UNLOCKED";
static const std::string EVENT_SLOT_CHANGED = "EVENT_SLOT_CHANGED";
static const std::string EVENT_SLOT_SELECTED = "EVENT_SLOT_SELECTED";
static const std::string EVENT_SQUADRON_PURCHASED = "EVENT_SQUADRON_PURCHASED";
static const std::string EVENT_BIT_PICKUP = "EVENT_BIT_PICKUP";

static const std::string EVENT_PULSER_PULSE = "EVENT_PULSER_PULSE";

#endif // __CONSTANTS_H__