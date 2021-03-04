#include "StudentWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>
#include <algorithm>
#include <cmath>
#include <iostream> // defines the overloads of the << operator
#include <sstream>  // defines the type std::ostringstream
#include <iomanip> 
using namespace std;


GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
    m_GRacer = nullptr;
    m_savedSouls = 0;
    m_bonus = 5000;     //initialzing all private numbers
}

int StudentWorld::init()
{
    //m_levelFinished = false;
    m_savedSouls = 0;
    m_bonus = 5000;
    m_GRacer = new GhostRacer(this);
    
    //<add boarderlines>
    int N = VIEW_HEIGHT / SPRITE_HEIGHT;
    int LEFT_EDGE_YELLOW = ROAD_CENTER - ROAD_WIDTH / 2;
    int RIGHT_EDGE_YELLOW = ROAD_CENTER + ROAD_WIDTH / 2;
    for (int i = 0; i != N; i++) {
        for (int j=0; j!= N;j++) {
            //left
            m_actor.push_back(new yellowBorderLine(this, IID_YELLOW_BORDER_LINE, LEFT_EDGE_YELLOW, (j*SPRITE_HEIGHT)));

            //right
            m_actor.push_back(new yellowBorderLine(this, IID_YELLOW_BORDER_LINE, RIGHT_EDGE_YELLOW, (j * SPRITE_HEIGHT)));
        }
    }
    
    int M = VIEW_HEIGHT / (4 * SPRITE_HEIGHT);
    int LEFT_EDGE_WHITE = ROAD_CENTER - ROAD_WIDTH / 2;
    int RIGHT_EDGE_WHITE = ROAD_CENTER + ROAD_WIDTH / 2;
    for (int i = 0; i != M; i++) {
        for (int j = 0; j != M; j++) {
            //left
            m_actor.push_back(new whiteBorderLine(this, IID_WHITE_BORDER_LINE, (LEFT_EDGE_WHITE + ROAD_WIDTH / 3), 
                j * (4 * SPRITE_HEIGHT)));

            //right
            m_actor.push_back(new whiteBorderLine(this, IID_WHITE_BORDER_LINE, (RIGHT_EDGE_WHITE - ROAD_WIDTH / 3), 
                j * (4 * SPRITE_HEIGHT)));
        }
    }
    //</add boarderlines>
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    // This code is here merely to allow the game to build, run, and terminate after you hit enter.
    // Notice that the return value GWSTATUS_PLAYER_DIED will cause our framework to end the current level.
    //decLives();
    //return GWSTATUS_PLAYER_DIED;

   
    ///for each of the actors in the game world >
    /// 
    /// boarderlines X axis
    const int leftWhiteBoarderline = ROAD_CENTER - ROAD_WIDTH / 2 + ROAD_WIDTH / 3;
    const int rightWhiteBoarderline = ROAD_CENTER + ROAD_WIDTH / 2 - ROAD_WIDTH / 3;
    const int leftYellowBoarderline = ROAD_CENTER - ROAD_WIDTH / 2;
    const int rightYellowBoarderline = ROAD_CENTER + ROAD_WIDTH / 2;

    
    m_GRacer->doSomething();
    if (!m_GRacer->checkAlive()) 
        return KilledGhostRacer();

     /// <Give each actor a chance to do something, including Ghost Racer     
    for (auto iterator = m_actor.begin(); iterator != m_actor.end();iterator++) {
        if (m_GRacer->checkAlive()) {
            if ((*iterator)->checkAlive()) {
                (*iterator)->doSomething();
                if (m_GRacer->isDead()) {
                    playSound(SOUND_PLAYER_DIE);
                    return KilledGhostRacer(); 
                }
            }
        }
    }
    /// </Give each actor a chance to do something, including Ghost Racer
    ///for each of the actors in the game world >
    

    /// <Remove newly-dead actors after each tick>
    for (auto it = m_actor.begin(); it != m_actor.end();it++) {
        if (!(*it)->checkAlive()) {
            delete (*it);
            it=m_actor.erase(it);
        }
    }
    /// </Remove newly-dead actors after each tick>


    
    /// <// Potentially add new actors to the game>
    int new_border_y = VIEW_HEIGHT - SPRITE_HEIGHT;

    int delta_y;
    auto ite = m_actor.end();   //an iterator points to the end
    ite--;
    while (ite != m_actor.begin()) {
        if ((*ite)->isWhite()) {    //stop when it is a white boarderline
            break;
        }
        else
            ite--;
    }
    delta_y = new_border_y - (*ite)->getY();

    if (delta_y >= SPRITE_HEIGHT) {
        m_actor.push_back(new yellowBorderLine(this, IID_YELLOW_BORDER_LINE, ROAD_CENTER - ROAD_WIDTH / 2, new_border_y));
        m_actor.push_back(new yellowBorderLine(this, IID_YELLOW_BORDER_LINE, ROAD_CENTER + ROAD_WIDTH / 2, new_border_y));
    }

    if (delta_y >= 4 * SPRITE_HEIGHT) {
        m_actor.push_back(new whiteBorderLine(this, IID_WHITE_BORDER_LINE, 
            ROAD_CENTER - ROAD_WIDTH / 2 + ROAD_WIDTH / 3, new_border_y));
        m_actor.push_back(new whiteBorderLine(this, IID_WHITE_BORDER_LINE, 
            ROAD_CENTER + ROAD_WIDTH / 2 - ROAD_WIDTH / 3, new_border_y));
    }
    /// </// Potentially add new actors to the game>
   

    //<add human peds>
    int ChanceHumanPed = max(200 - getLevel() * 10, 30);
    int rand = randInt(0, ChanceHumanPed);
    if (rand == 0) {
        addActor(new humanPedestrians(this, randInt(0, VIEW_WIDTH), VIEW_HEIGHT));
    }

    //<add human peds>

    //<Zombie peds>
    int ChanceZombiePed = max(100-getLevel()*10,20);
    if (randInt(0, ChanceZombiePed) == 0) {
        addActor(new zombiePedestrians(this, randInt(0, VIEW_WIDTH), VIEW_HEIGHT));
    }
    //</Zombie peds>

    //<Zombie cabs>
    int ChanceVehicle = max(100 - getLevel() * 10, 20);
    
    if (randInt(0, ChanceVehicle) == 0) {
        int cur_lane;
        bool addedOneCab = false;   //check whether a cab is added
        bool atBot = false;         //check whether it is at bot
        for (int i = 0; i != 3; i++) {
            clearChoice();  //clean up container first
            
            
            while (choice.size() != 3) {
                //<choose a random lane>
                choose:
                switch (randInt(0, 2)) {    //make sure it is RANDOMLY CHOSEN
                case 0:
                    if (    (0)) //return true if already checked this lane, so skip this one, false o/w
                        goto choose;
                    cur_lane = ROAD_CENTER - ROAD_WIDTH / 3; //left
                    goto nextStage;
                case 1:
                    if (hasDupLanesOrAddOne(1)) //return true if already checked this lane, so skip this one, false o/w
                        goto choose;
                    cur_lane = ROAD_CENTER; //middle 
                    goto nextStage;
                case 2:
                    if (hasDupLanesOrAddOne(2)) //return true if already checked this lane, so skip this one, false o/w
                        goto choose;
                    cur_lane = ROAD_CENTER + ROAD_WIDTH / 3;    //right
                    goto nextStage;
                }
                //</choose a random lane>


                //<add new cabs>
            nextStage:
                switch (cur_lane) {
                case ROAD_CENTER - ROAD_WIDTH / 3:  //left
                    if (!checkClosestActorAtBot(leftYellowBoarderline, leftWhiteBoarderline)) {
                        atBot = true;
                        addedOneCab = true;
                        goto step3;
                    }
                    if (!checkClosestActorAtTop(leftYellowBoarderline, leftWhiteBoarderline)) {
                        
                        addedOneCab = true;
                        goto step3;
                    }
                    break;
                case ROAD_CENTER:   //middle
                    if (!checkClosestActorAtBot(leftWhiteBoarderline, rightWhiteBoarderline)) {
                        atBot = true;
                        addedOneCab = true;
                        goto step3;
                    }
                    if (!checkClosestActorAtTop(leftWhiteBoarderline, rightWhiteBoarderline)) {
                        
                        addedOneCab = true;
                        goto step3;
                    }
                    break;
                case ROAD_CENTER + ROAD_WIDTH / 3:  //right
                    if (!checkClosestActorAtBot(rightWhiteBoarderline, rightYellowBoarderline)) {
                        atBot = true;
                        addedOneCab = true;
                        goto step3;
                    }
                    if (!checkClosestActorAtTop(rightWhiteBoarderline, rightYellowBoarderline)) {
                        
                        addedOneCab = true;
                        goto step3;
                    }
                    break;
                }
                //</add new cabs>
            }
        }
        step3:
        if (addedOneCab && atBot) {
            addActor(new zombieCabs(this, cur_lane, SPRITE_HEIGHT / 2, getGRptr()->getVerSpeed() + randInt(2, 4))); //create at bot
            addedOneCab = false;
            atBot = false;
        }
        else if (addedOneCab && !atBot) {
            addActor(new zombieCabs(this, cur_lane, VIEW_HEIGHT - SPRITE_HEIGHT / 2, getGRptr()->getVerSpeed() - randInt(2, 4)));   //create at top
            addedOneCab = false;
            atBot = false;
        }
    }
    //<Zombie cabs>


    //<oil slicks>
    int ChanceOilSlick = max(150 - getLevel() * 10, 40);
    if (randInt(0, ChanceOilSlick)==0)
        addActor(new OilSlick(this, randInt(leftYellowBoarderline, rightYellowBoarderline), VIEW_HEIGHT));
    //</oil slicks>

    //<holy water refill>
    int ChanceOfHolyWater = (100 + 10 * getLevel());
    if (randInt(0, ChanceOfHolyWater) == 0)
        addActor(new HolyWaterGoodie(this, randInt(leftYellowBoarderline, rightYellowBoarderline), VIEW_HEIGHT));
    //</holy water refill>

    //<Soul>
    int ChanceOfLostSoul = 100;
    if (randInt(0, ChanceOfLostSoul) == 0)
        addActor(new SoulGoodie(this, randInt(leftYellowBoarderline, rightYellowBoarderline), VIEW_HEIGHT));
    //</Soul>



    //<add text to the screen>
    ostringstream oss;
    oss << " Score: " << getScore() << "  ";    //from the sample program, there is one space in front of the Score.
    oss << "Lvl: " << getLevel() << "  ";
    oss << "Souls2Save: " << 2 * getLevel() + 5 - getSouls() << "  ";
    oss << "Lives: " << getLives() << "  ";
    oss << "Health: " << m_GRacer->getHealth() << "  ";
    oss << "Sprays: " << m_GRacer->getHolyWater() << "  ";
    oss << "Bonus: " << getBonus();
    setGameStatText(oss.str());
    //</add text to the screen>

    //<player conpleted a level>
    if (SavedEnoughSouls()) {
        increaseScore(getBonus());
        playSound(SOUND_FINISHED_LEVEL);
        return GWSTATUS_FINISHED_LEVEL;
    }
    //</player conpleted a level>


    decBonusEveryTick();    //decrease bonus point every tick

    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    for (auto p : m_actor)
        delete p;
    m_actor.clear();

    delete m_GRacer;
}

StudentWorld::~StudentWorld() {
    cleanUp();
}

bool StudentWorld::SavedEnoughSouls() const {
    if (m_savedSouls == 2 * getLevel() + 5)
        return true;
    return false;
}

bool StudentWorld::overlapped(Actor* A, Actor* B) const {
    int delta_x = abs(A->getX() - B->getX());
    int delta_y= abs(A->getY() - B->getY());
    double radius_sum = A->getRadius() + B->getRadius();
    if (delta_x < radius_sum * .25 && delta_y < radius_sum * .6)
        return true;
    return false;
}

Actor* StudentWorld::checkClosestActorAtBot(int laneLeft, int laneRight, double startY, double upperlimit) const {
    //<compare actors only>

    //find the first actor which is on that lane and above startY and under the upperlimit
    auto temp = m_actor.begin();
    bool found = false;
    while (temp != m_actor.end()) {
        if ((*temp)->getCollisionable() && (*temp)->getX() < laneRight && (*temp)->getX() > laneLeft &&
            (*temp)->getY() > startY + 4.0 && (*temp)->getY() < upperlimit - 4.0) {
            found = true;
            break;
        }
        temp++;
    }
    //now temp points to the position of the first colliable actor

    //compare the temp pointer to every other actors which fullfills the condition, to find the closest
    if (found) {
        for (auto it = m_actor.begin(); it != m_actor.end(); it++) {
            if ((*it)->getCollisionable() && it != temp && (*it)->getX() < laneRight && (*it)->getX() > laneLeft
                && (*it)->getY() < upperlimit - 4.0 && (*it)->getY() > startY + 4.0) {
                if ((*it)->getY() < (*temp)->getY())
                    temp = it;
            }
        }
    }
    //now temp points to the minimum distance from the bottom line
    //</compare actors only>

    //<compare actors with GR>
    if (m_GRacer->getX() < laneRight && m_GRacer->getX() > laneLeft) {  //Ghost Racer is on that lane
        if (found && m_GRacer->getY() > (*temp)->getY() && (*temp)->getY() < upperlimit-4.0) {
            return (*temp); //temp actor has less Y coordinate
        }
        return m_GRacer;    //Ghost Racer has less Y coordinate
    }
    else if (found&& (*temp)->getY() > startY + 4.0 && (*temp)->getY() < upperlimit - 4.0) {
        return (*temp); //temp actor has less Y coordinate
    }
    //</compare actors with GR>

    
    
    return nullptr;   //all actors are greater than VIEW_HEIGHT / 3 or this lane is empty
}

Actor* StudentWorld::checkClosestActorAtTop(int laneLeft, int laneRight, double startY, double lowerlimit) const {

    //<compare actors only>

    //find the first ever actor which is on that lane and above the lowerlimit and under startY
    auto temp = m_actor.begin();
    bool found = false;
    while (temp != m_actor.end()) {
        if ((*temp)->getCollisionable() && (*temp)->getX() < laneRight && (*temp)->getX() > laneLeft &&
            (*temp)->getY() > lowerlimit + 4.0 && (*temp)->getY() < startY - 4.0) {
            found = true;
            break;
        }
        temp++;
    }
    //now temp points to the position of the first colliable actor

    //compare this actor with all other actors which fullfilled the condition
    if (found) {
        for (auto it = m_actor.begin(); it != m_actor.end(); it++) {
            if ((*it)->getCollisionable() && it != temp && (*it)->getX() < laneRight && (*it)->getX() > laneLeft
                && (*it)->getY() > lowerlimit + 4.0 && (*it)->getY() < startY - 4.0) {
                if ((*it)->getY() > (*temp)->getY())
                    temp = it;
            }
        }
    }

    if (found&&(*temp)->getY() > lowerlimit + 4.0 && (*temp)->getY() < startY - 4.0) {  //no GR any more because zombie cab won't accelarate when the ghost racer is behind, only actors other than GR
       
        return (*temp);
    }

   
    return nullptr;   //all actors are less than VIEW_HEIGHT * 2 / 3 or this lane is empty
}

//check if the actor is sprayable
bool StudentWorld::SprayedIfOverlapped(Actor* HWP) {

    for (int i = 0; i != m_actor.size(); i++)
    {
        if (overlapped(HWP, m_actor[i]) && HWP != m_actor[i])       //overlapped
        {
            
            if (m_actor[i]->checkAlive() && m_actor[i]->isSprayable())      //alive and sprayable
            {
                if (m_actor[i]->isHuman()) {    //is human
                    m_actor[i]->makeTheyMad();  //make their mad
                    HWP->setDead();             //set holy water to dead
                    return true;
                }
                else{                                        //other actor which is damagable
                    m_actor[i]->takeDamageAndPossiblyDie(1);//taking damage
                    if (!m_actor[i]->checkAlive())          //dead because of damage
                        m_actor[i]->addScores();            //add specific scores
                    if (m_actor[i]->dropGoodies()&&!overlapped(m_actor[i], getGRptr())&&    //an actor drops goodies and it is killed by spray instead of crash
                        !m_actor[i]->checkAlive()) {    //dead because of spray
                        if (randInt(1, 5) == 1)
                            m_actor[i]->dropSomething();//dropped
                    }
                    HWP->setDead(); //set holy water to dead
                    return true;
                }
            }
            else {
                return false;
            }
                

        }
    }
    return false;
}