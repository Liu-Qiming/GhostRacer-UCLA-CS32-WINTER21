#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include <string>
#include <vector>
#include <set>
using namespace std;
// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp


class Actor;
class GhostRacer;

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    virtual int init();
    virtual int move();
    virtual void cleanUp();

    //mutator
    int KilledGhostRacer(); //kill Ghost Racer
    void addActor(Actor* actor) { m_actor.push_back(actor); }   //add a new actor to the world
    bool hasDupLanesOrAddOne(int lane) ;     //return true if already checked this lane, so skip this one, false o/w
    void clearChoice() { choice.clear(); }  //clear the container of number of chosed lanes
    bool SprayedIfOverlapped(Actor* HWP);   //if overlapped, spray is activated

    //for game text
    void savedOneSoulWhetherFinished() { m_savedSouls++; }  //saved one more soul
    int addBonus(int amt) { m_bonus += amt; }   //add bonus

    //accessor
    GhostRacer* getGRptr() const { return m_GRacer; }
    bool overlapped(Actor* A, Actor* B)const;   //if A and B are overlapping
    Actor* checkClosestActorAtBot(int laneLeft, int laneRight, double startY = 0, double upperlimit = VIEW_HEIGHT / 3) const;   //check whether there are any actors in front of the startY
    Actor* checkClosestActorAtTop(int laneLeft, int laneRight, double startY = VIEW_HEIGHT, double lowerlimit = VIEW_HEIGHT * 2 / 3) const;    //check whether there are any actors behind the startY
    
    int getSouls() const { return m_savedSouls; }
    int getBonus() const { return m_bonus; }
    void decBonusEveryTick() { if (m_bonus>=1) m_bonus--; } //every tick decreases the bonus by 1 :(
    bool SavedEnoughSouls() const;  //return true if already saved 2 * L + 5 souls

    //housekeeping
    virtual ~StudentWorld();
private:
    vector <Actor*> m_actor;
    GhostRacer* m_GRacer;
    set<int> choice;    //make sure not to choose the lane one more time
    int m_savedSouls;
    int m_bonus;
};

inline
bool StudentWorld::hasDupLanesOrAddOne(int lane) {  //if a lane is already checked, skip to next lane, if not, insert it
    auto it = choice.end();
    it = choice.find(lane);
    if (it != choice.end())
        return true;
    choice.insert(lane);
    return false;
}

inline
int StudentWorld::KilledGhostRacer() {  //kill Ghost Racer 
    decLives();
    return GWSTATUS_PLAYER_DIED;
}



#endif // STUDENTWORLD_H_
