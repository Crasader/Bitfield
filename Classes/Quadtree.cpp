#include "Quadtree.h"

Quadtree::Quadtree(int level, cocos2d::Rect boundary)
{
    this->level = level;
    this->boundary = boundary;
    for (int i = 0; i < 4; i++) {
        children[i] = nullptr;
    }
}

Quadtree::~Quadtree()
{
    clear();
}

void Quadtree::clear()
{
    nodes.clear();
    for (int i = 0; i < 4; i++) {
        if (children[i]) {
            children[i]->clear();
            delete children[i];
            children[i] = nullptr;
        }
    }
}

void Quadtree::insert(cocos2d::Node* node)
{
    // If children[0] null, we haven't split yet
    // So if it isn't null, we have leaves to insert into
    if (children[0] != nullptr) {
        Quadrant quadrant = getQuadrant(node);

        if (quadrant != None) {
            children[quadrant]->insert(node);
            return;
        }
        else {
            cocos2d::log("size %.2f %.2f, level %d",
                boundary.origin.x, boundary.origin.y, level);
        }
    }

    nodes.pushBack(node);

    if (nodes.size() > MAX_NODES && level < MAX_LEVELS) {
        if (children[0] == nullptr) {
            split();
        }

        int i = 0;
        while (i < nodes.size()) {
            // Sort all our children into their new quadrants
            auto _node = nodes.at(i);
            int index = getQuadrant(_node);

            // If we found a quadrant, put our node there
            if (index != None) {
                nodes.eraseObject(_node);
                children[index]->insert(_node);
            }
            else {
                i++;
            }
        }
    }
}

cocos2d::Vector<cocos2d::Node*>* Quadtree::retrieve(cocos2d::Node * node)
{
    int index = getQuadrant(node);

    // If the node is deeper, ask a child
    if (index != None && children[0] != nullptr) {
        return children[index]->retrieve(node);
    }

    return &nodes;
}

void Quadtree::split()
{
    float subWidth = boundary.size.width / 2.f;
    float subHeight = boundary.size.height / 2.f;
    float x = boundary.origin.x;
    float y = boundary.origin.y;

    children[BottomLeft] = new Quadtree(level + 1, cocos2d::Rect(x, y, subWidth, subHeight));
    children[TopLeft] = new Quadtree(level + 1, cocos2d::Rect(x, y + subHeight, subWidth, subHeight));
    children[BottomRight] = new Quadtree(level + 1, cocos2d::Rect(x + subWidth, y, subWidth, subHeight));
    children[TopRight] = new Quadtree(level + 1, cocos2d::Rect(x + subWidth, y + subHeight, subWidth, subHeight));
}

Quadtree::Quadrant Quadtree::getQuadrant(cocos2d::Node* node)
{
    Quadrant index = None;

    auto midX = boundary.origin.x + (boundary.size.width / 2.f);
    auto midY = boundary.origin.y + (boundary.size.height / 2.f);

    const auto& pos = node->getPosition();
    //bool fitsBottom = (rect.origin.y < midY && rect.origin.y + rect.size.height * node->getScaleY() < midY);
    //bool fitsTop = rect.origin.y >= midY;
    //bool fitsLeft = rect.origin.x < midX && rect.origin.x + rect.size.width * node->getScaleX() < midX;
    //bool fitsRight = rect.origin.x >= midX;

    bool fitsBottom = pos.y < midY;
    bool fitsTop = pos.y >= midY;
    bool fitsLeft = pos.x < midX;
    bool fitsRight = pos.x >= midX;

    if (fitsLeft) {
        if (fitsTop) {
            index = TopLeft;
        }
        else if (fitsBottom) {
            index = BottomLeft;
        }
    }
    // Object can completely fit within the right quadrants
    else if (fitsRight) {
        if (fitsTop) {
            index = TopRight;
        }
        else if (fitsBottom) {
            index = BottomRight;
        }
    }

    return index;
}

