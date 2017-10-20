#include "Bit.h"
#include "Constants.h"

USING_NS_CC;

Bit::Bit(BitType type) {
    this->type = type;
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

void Bit::onEnter() {
    Sprite::onEnter();

    setColor(Color3B(Player::bit_info[type].color));
    setScale(BIT_SCALE);
    setRotation(rand_0_1() * 360);

    scheduleUpdate();
}

void Bit::onExit() {
    Sprite::onExit();
    unscheduleUpdate();
}

void Bit::update(float delta) {
    Sprite::update(delta);
}

BitType Bit::getType() {
    return type;
}