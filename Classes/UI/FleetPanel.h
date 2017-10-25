#pragma once

#include <cocos2d.h>

class FleetPanel : public cocos2d::Node
{
public:
    enum View {
        Fleet, Squadrons
    };

    CREATE_FUNC(FleetPanel);
    virtual bool init() override;

    void setView(View view);

private:
    // Initialize
    void addBackground();
    void addTabs();
    
    void addFleetLayer();

    View currentView;
};

