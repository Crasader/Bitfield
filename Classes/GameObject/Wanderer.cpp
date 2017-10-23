#include "Wanderer.h"

#include "..\Scene\GameScene.h"
#include "..\Constants.h"
#include "PlayerData\Player.h"
#include "Bit.h"

USING_NS_CC;

Wanderer* Wanderer::create() {
    Wanderer *btn = new (std::nothrow) Wanderer();
    if (btn && btn->init()) {
        btn->autorelease();
        return btn;
    }
    CC_SAFE_DELETE(btn);
    return nullptr;
}