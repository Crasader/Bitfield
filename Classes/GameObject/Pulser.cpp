#include "Pulser.h"

#include "Input.h"
#include "Util.h"
#include "Constants.h"
#include "GameObject\Bit.h"
#include "UI\World.h"
USING_NS_CC;

std::vector<int> Pulser::number_reached_target = std::vector<int>(7);

Pulser::Pulser(World* world, SquadronInfo info, int squadronID, int shipID)
    : Ship(world, info, squadronID, shipID)
{
    reached_target = false;
    target_x = -1;
    target_y = -1;
    timer = 0;
    pulse_t = info.doubles["pulse_t"];
}

Pulser* Pulser::create(World* world, SquadronInfo info, int squadronID, int shipID) {
    Pulser* ship = new (std::nothrow) Pulser(world, info, squadronID, shipID);
    if (ship && ship->initWithFile(info.strings["sprite"])) {
        ship->autorelease();
        return ship;
    }
    CC_SAFE_DELETE(ship);
    return nullptr;
}

bool Pulser::initWithFile(const std::string & path)
{
    if (!Ship::initWithFile(path)) return false;

    createEventListener();

    return true;
}

void Pulser::calculateForces(float delta)
{
    timer += delta;

    // Check if it's time to pulse
    if (timer > pulse_t) {

        // If we don't have a target, find one
        if (shipID == 0) {
            if (target_x == -1) {
                auto cell = world->getCellIndex(getPosition());
                target_x = cell.x;
                target_y = cell.y;

                // Make sure neighbours have a place to land
                if (target_x == 0) target_x++;
                else if (target_x == GRID_RESOLUTION - 1) target_x--;
                if (target_y == 0) target_y++;
                else if (target_y == GRID_RESOLUTION - 1) target_y--;
            }
        }
        else { // Neighbours base their tile on the leader's tile
            if (target_x == -1) {
                auto leader = static_cast<Pulser*>(world->getSquadron(squadronID).at(0));
                if (leader->target_x < 0) return;
                target_x = leader->target_x - 1;
                target_y = leader->target_y - 1;

                if (shipID <= 3) {
                    target_x += shipID - 1;
                }
                else if (shipID == 4 || shipID == 5) {
                    target_y++;
                    if (shipID == 5) target_x += 2;
                }
                else {
                    target_y += 2;
                    target_x += shipID - 6;
                }
            }
        }

        auto target_pos = world->getCellPosition(target_x, target_y, true);
        auto current_pos = world->getCellIndex(getPosition());
        if (current_pos.x == target_x && current_pos.y == target_y) {
            if (!reached_target) {
                reached_target = true;
                number_reached_target[squadronID]++;
                if (number_reached_target[squadronID] == count) {
                    Player::dispatchEvent(EVENT_PULSER_PULSE, (void*)squadronID);
                }
            }
        }
        applyForce(seek(target_pos, true));
        return;
    }
    Ship::calculateForces(delta);
}

void Pulser::createEventListener()
{
    auto l_pulse = EventListenerCustom::create(EVENT_PULSER_PULSE, [=](EventCustom* event) {
        auto squadID = (int)event->getUserData();
        if (squadID != squadronID) return;
        world->addTileGlow(target_x, target_y, Color3B(UI_COLOR_BLUE), 0.8f);
        world->consumeTile(target_x, target_y);
        number_reached_target[squadronID]--;
        reached_target = false;
        target_x = -1;
        target_y = -1;
        timer = 0;
    });
    getEventDispatcher()->addEventListenerWithSceneGraphPriority(l_pulse, this);
}
