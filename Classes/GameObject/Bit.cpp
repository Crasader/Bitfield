#include "Bit.h"
#include "Constants.h"

USING_NS_CC;

Bit::Bit(BitType type) {
    this->type = type;
    ship = nullptr;
    removed = false;

    setColor(Color3B(Player::generators[type].color));
    setScale(BIT_SCALE);
    setRotation(rand_0_1() * 360);
}

Bit* Bit::create(BitType type) {
    auto sprite = new (std::nothrow) Bit(type);
    if (sprite && sprite->initWithFile(SPRITE_BIT))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

BitType Bit::getType() {
    return type;
}

bool Bit::isTargettedBy(Ship* ship)
{
    if (!ship) return false;
    return this->ship == ship;
}

bool Bit::isTargetted() {
    return ship != nullptr;
}

void Bit::setShip(Ship* ship)
{
    this->ship = ship;
}

bool Bit::isRemoved()
{
    return removed;
}

void Bit::remove()
{
    removed = true;
}
