#ifndef __SHIP_H__
#define __SHIP_H__

#include "cocos2d.h"

class Bit;

class Ship : public cocos2d::Sprite
{
public:
    CREATE_FUNC(Ship);

    virtual void onEnter() override;
    virtual void onExit() override;
    virtual void update(float delta) override;

    void applyForce(cocos2d::Vec2 force, float scale = 1);

    cocos2d::Vec2 wander();
    cocos2d::Vec2 seek(cocos2d::Vec2 target, bool slowDown = false);
    cocos2d::Vec2 separate(const cocos2d::Vector<Ship*>& neighbours);
    cocos2d::Vec2 cohesion(const cocos2d::Vector<Ship*>& neighbours);
    cocos2d::Vec2 align(const cocos2d::Vector<Ship*>& neighbours);
    cocos2d::Vec2 seekBits(const cocos2d::Vector<Bit*>& neighbours);

    bool isFront(const cocos2d::Vector<Ship*>& neighbours);
    bool canSee(cocos2d::Node* target);
    bool inRange(cocos2d::Node* target);
    const cocos2d::Vec2& getVelocity();
    const cocos2d::Vec2& getAcceleration();
    const cocos2d::Vec2& getTargetOffset();

    void setAcceleration(const cocos2d::Vec2& acceleration);
    void setTargetOffset(const cocos2d::Vec2& targetOffset);

    void screenWrap();

private:
    cocos2d::Vec2 velocity;
    cocos2d::Vec2 acceleration;
    cocos2d::Vec2 targetOffset;
};

#endif // __SHIP_H__
