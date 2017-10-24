#include "Wanderer.h"

#include "Util.h"
#include "..\Scene\GameScene.h"
#include "..\Constants.h"
#include "PlayerData\Player.h"
#include "Bit.h"

USING_NS_CC;

Wanderer::Wanderer(SquadronInfo info)
    : Ship(info)
{
    w_stay_grouped = 0;
}

Wanderer* Wanderer::create(SquadronInfo info) {
    Wanderer* ship = new (std::nothrow) Wanderer(info);
    if (ship && ship->initWithFile(info.strings["sprite"])) {
        ship->autorelease();
        return ship;
    }
    CC_SAFE_DELETE(ship);
    return nullptr;
}