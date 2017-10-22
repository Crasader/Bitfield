#ifndef __BIT_H__
#define __BIT_H__

#include "cocos2d.h"
#include "PlayerData/Player.h"

class Bit : public cocos2d::Sprite
{
public:
    Bit(BitType type);
    static Bit* create(BitType type);

    virtual void onEnter() override;
    virtual void onExit() override;
    virtual void update(float delta) override;

    BitType getType();

private:
    BitType type;
};

#endif // __BIT_H__
