#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

//const double ONE_TICK = 1 / 20;

class StudentWorld;
class Actor : public GraphObject  //most base class
{
public:
	Actor(StudentWorld* world,int imageID, int startX, int startY, int startDirection, double size, int depth, bool collisionable, int health,
		double verSpeed =0, double horSpeed =0)
		:GraphObject(imageID, startX, startY, startDirection, size, depth), m_isAlive(true), m_world(world),
		m_collisionable(collisionable), m_health(health), m_horSpeed(horSpeed), m_verSpeed(verSpeed){}
	
	
	//accessor
	bool checkAlive() const { return m_isAlive; }	//check if any actor is still alive
	bool isDead() const { return m_health<=0; }	//return true if any actor has 0 health
	int getHealth() const { return m_health; }	//return actor's health
	bool getCollisionable() const { return m_collisionable; }	//any actor which is a collision avoidance actor should return true
	StudentWorld* getWorld() const { return m_world; }	//return StudentWorld pointer
	virtual bool isWhite() const{ return false; }	//default to white, distinguish white and yellow boarderlines
	double getVerSpeed() { return m_verSpeed; }	//get actor's vertical speed
	double getHorSpeed() { return m_horSpeed; }	//get actor's horizontal speed
	virtual bool isHuman() { return false; }	//default to false, distinguish human and nonhuman actors
	

	virtual bool isSprayable() const { return false; }//default to false, distinguish sprayable actors
	virtual bool canTakeDamagedFromSpray() { return false; }	//default to false, distinguish damagable actors
	//mutator

	virtual void doSomething()=0;	//every actor has a chance to do something
	
	void setDead() { m_isAlive = false; }	//set the actor to dead
	void setVerSpeed(double VerSpeed) { m_verSpeed = VerSpeed; }	//set actor's vertical speed
	void setHorSpeed(double HorSpeed) { m_horSpeed = HorSpeed; }	//set actor's horizontal speed
	void checkOutOfBoundDead() { 
		if (getX() < 0 || getY() < 0 || getX() > VIEW_WIDTH || getY() > VIEW_HEIGHT)
			setDead();
	}
	void takeDamageAndPossiblyDie(int amount);
	
	void movePeds();	//make human and zombie and cabs moving
	void getHealed(int amount) { //only for GR	to get healed
		if (m_health<=100-amount)
			m_health += amount; 
	}
	
	virtual void makeTheyMad() {}//only for human pedestrain to make their mad

	virtual int soundWhenHurt() { return 9999; }	//dummy function but cannot be purely virtual
	virtual int soundWhenDie() { return 9999; }		//dummy function but cannot be purely virtual

	virtual bool dropGoodies() { return false; }	//for zombie cab and ped to drop goodies
	virtual void dropSomething(){}		//for zombie cab and ped to drop goodies

	virtual void addScores() {}		//dummy version but cannot be pure virtual

	//housekeeping
	virtual ~Actor() {}
private:
	bool m_isAlive;
	bool m_collisionable;
	int m_health;
	double m_horSpeed, m_verSpeed;
	StudentWorld* m_world;
};

class GhostRacer : public Actor {
public:
	GhostRacer(StudentWorld* world)
		:Actor(world, IID_GHOST_RACER, 128, 32, 90, 4.0, 0, true, 100, 0, 0),
		m_holyWaterLeft(10)  {}
		
	//check
	//mutator
	virtual void doSomething();
	void sprayHolyWater() { m_holyWaterLeft--; }
	void GRmove();	//moving algorism for Ghost Racer

	// Spin as a result of hitting an oil slick.
	void spin();
	void increaseHolyWater(int amt) { m_holyWaterLeft += amt; }
	int getHolyWater() { return m_holyWaterLeft; }
	virtual int soundWhenHurt() { return SOUND_VEHICLE_CRASH; }
	virtual int soundWhenDie() { return SOUND_PLAYER_DIE; }

private:
	int m_holyWaterLeft;

};

class BorderLine : public Actor {
public:
	BorderLine(StudentWorld* world, int imageID, int startX, int startY)
		:Actor(world, imageID, startX, startY, 0, 2.0, 2, false, 999, -4, 0){}
	virtual void doSomething();
};

class yellowBorderLine : public BorderLine {
public:
	yellowBorderLine(StudentWorld* world, int imageID, int startX, int startY)
		:BorderLine(world, imageID, startX, startY) {}
};

class whiteBorderLine : public BorderLine {
public:
	whiteBorderLine(StudentWorld* world, int imageID, int startX, int startY)
		:BorderLine(world, imageID, startX, startY){}
	virtual bool isWhite() const { return true; }		//only for distinguishing whiteboarder lines
};

class Pedestrians : public Actor {	//base class
public:

	Pedestrians(StudentWorld* world, int imageID, int startX, int startY, double size)
		:Actor(world, imageID, startX, startY, 0, size, 0, true, 2, -4, 0), m_plan(0){}
	
	void decPlan() { m_plan--; }
	int getPlan() { return m_plan; }
	void setPlan() { m_plan = randInt(4, 32); }	//set plan for all pedestrains are the same
	void newMovementPlan();	//give pedestrians a new movement plan
	virtual bool isSprayable() const { return true; }		//spayable
	virtual int soundWhenHurt() { return SOUND_PED_HURT; }
	virtual ~Pedestrians(){}
private:
	int m_plan;
	

};

class humanPedestrians : public Pedestrians {
public:
	humanPedestrians(StudentWorld* world, int x, int y)
		:Pedestrians(world, IID_HUMAN_PED, x,y, 2.0), m_mad(false) {}
	virtual void doSomething();
	virtual bool isHuman() { return true; }		//only for human pedestrain
	virtual void makeTheyMad() { m_mad = true; }	//get mad from spaying holy water
	bool getMadness() { return m_mad; }
private:
	bool m_mad;
};

class zombiePedestrians : public Pedestrians {
public:
	zombiePedestrians(StudentWorld* world, int x, int y)
		:Pedestrians(world, IID_ZOMBIE_PED, x, y, 3.0), zombie_tick(0){}
	virtual void doSomething();
	//accessor
	int getTicks() const { return zombie_tick; }
	//mutator
	void decTicks() { zombie_tick--; }
	void setTicksTo20() { zombie_tick = 20; }
	virtual bool canTakeDamagedFromSpray() { return true; }		
	virtual int soundWhenDie() { return SOUND_PED_DIE; }
	virtual bool dropGoodies() { return true; }		//able to drop goodies
	virtual void dropSomething();	//drop healing goodies when they are shot dead by holy water
	virtual void addScores();	//unique scores add
private:
	int zombie_tick;
};

class zombieCabs : public Actor {
public:
	zombieCabs(StudentWorld* world, int x, int y, int verSpeed)
		:Actor(world, IID_ZOMBIE_CAB, x, y, 90, 4.0, 0, true, 3, verSpeed), m_damagedeGR(false), m_plan(0) {}
	virtual void doSomething();
	bool damagedAlready(){return m_damagedeGR;}		
	void setDamagedAlreadyTrue() { m_damagedeGR = true; }
	Actor* checkCollisionActor(zombieCabs* zc, int cur_lane, bool isFront);		//check whether front/behind has collision avoidance actors
	int checkLane(int x);	//check which lane he is at
	void decPlan() { m_plan--; }
	int getPlan() { return m_plan; }
	void setPlan() { m_plan = randInt(4, 32); }
	virtual bool canTakeDamagedFromSpray() { return true; }//damagable
	virtual bool isSprayable() const { return true; }	//spayable

	virtual int soundWhenDie() { return SOUND_VEHICLE_DIE; }
	virtual int soundWhenHurt() { return SOUND_VEHICLE_HURT; }
	virtual bool dropGoodies() { return true; }	//able to drop something after shot dead by holy water
	virtual void dropSomething();	//drop oil slicks
	virtual void addScores();
private:
	bool m_damagedeGR;
	int m_plan;
};

class GhostRacerActivatedObject : public Actor
{
public:
	GhostRacerActivatedObject(StudentWorld* sw, int imageID, double x, double y, double size, int dir, int verSpeed )
		:Actor(sw, imageID, x, y, dir, size, 2, false,1, verSpeed ) {}

	// Do the object's special activity (increase health, spin Ghostracer, etc.)
	virtual void doActivity(GhostRacer* gr) = 0;

	// Return the object's increase to the score when activated.
	//virtual int getScoreIncrease() const = 0;

	// Return the sound to be played when the object is activated.
	virtual int getSound() const { return SOUND_GOT_GOODIE; }

	// Return whether the object dies after activation.
	//virtual bool selfDestructs() const = 0;

	virtual ~GhostRacerActivatedObject() {}
};

class OilSlick : public GhostRacerActivatedObject
{
public:
	OilSlick(StudentWorld* sw, double x, double y)
		:GhostRacerActivatedObject(sw, IID_OIL_SLICK, x, y, randInt(2, 5), 0, -4) {}
	virtual void doSomething();
	//virtual int getScoreIncrease() const;
	//virtual bool selfDestructs() const;
	virtual void doActivity(GhostRacer* gr);
	virtual int getSound() const { return SOUND_OIL_SLICK; }
};

class HealingGoodie : public GhostRacerActivatedObject
{
public:
	HealingGoodie(StudentWorld* sw, double x, double y)
		:GhostRacerActivatedObject(sw, IID_HEAL_GOODIE, x, y, 1.0, 0, -4) {}
	virtual void doSomething();
	virtual void doActivity(GhostRacer* gr);
	virtual bool canTakeDamagedFromSpray() { return true; }//damagable
	virtual bool isSprayable() const { return true; }	//spayable
	virtual void addScores();
};

class HolyWaterGoodie : public GhostRacerActivatedObject
{
public:
	HolyWaterGoodie(StudentWorld* sw, double x, double y)
		:GhostRacerActivatedObject(sw, IID_HOLY_WATER_GOODIE, x, y, 2.0, 90, -4) {}
	virtual void doSomething();
	virtual void doActivity(GhostRacer* gr);
	virtual bool canTakeDamagedFromSpray() { return true; }	//damagable
	virtual bool isSprayable() const { return true; }	//spayable
	virtual void addScores();
};

class SoulGoodie : public GhostRacerActivatedObject
{
public:
	SoulGoodie(StudentWorld* sw, double x, double y)
		:GhostRacerActivatedObject(sw, IID_SOUL_GOODIE, x, y, 4.0, 0, -4) {}
	virtual void doSomething();
	virtual void doActivity(GhostRacer* gr);
	virtual int getSound() const { return SOUND_GOT_SOUL; }
	virtual void addScores();
};

class HolyWaterProjectile : public Actor {
public:
	HolyWaterProjectile(StudentWorld* sw, double x, double y, int dir)
		:Actor(sw, IID_HOLY_WATER_PROJECTILE, x, y, dir, 1.0, 1, false, 999) {}
	virtual void doSomething();
	virtual bool isHolyWaterProjectile() { return true; }	//later to check not the projectile itself
};

/// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////









#endif // ACTOR_H_
