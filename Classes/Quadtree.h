#pragma once

#include <cocos2d.h>

class Quadtree {
public:
    enum Quadrant {
        BottomLeft, TopLeft, BottomRight, TopRight, None
    };
    Quadtree(int level, cocos2d::Rect boundary);
    ~Quadtree();

    void clear();
    void insert(cocos2d::Node* node);
    cocos2d::Vector<cocos2d::Node*>* retrieve(cocos2d::Node* node);

    cocos2d::Rect boundary;
    Quadtree* children[4];

private:
    const int MAX_NODES = 1;
    const int MAX_LEVELS = 50;
    

    int level;
    cocos2d::Vector<cocos2d::Node*> nodes;
    

    void split();
    Quadrant getQuadrant(cocos2d::Node* node);
};