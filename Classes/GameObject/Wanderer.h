#pragma once

#include "cocos2d.h"

class Wanderer : public Ship
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

    void screenWrap();
};