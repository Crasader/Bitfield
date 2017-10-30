#ifndef __BIT_H__
#define __BIT_H__

#include "cocos2d.h"
#include "PlayerData/Player.h"

class Ship;

class Bit : public cocos2d::Sprite
{
public:
    Bit(BitType type);
    static Bit* create(BitType type);

    BitType getType();
    bool isTargettedBy(Ship* ship);
    bool isTargetted();
    void setShip(Ship* ship);

    bool isRemoved();
    void remove();

private:
    BitType type;
    Ship* ship;
    bool removed;
};

#endif // __BIT_H__
