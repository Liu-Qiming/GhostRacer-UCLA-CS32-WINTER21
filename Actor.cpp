#include "Actor.h"
#include "StudentWorld.h"
#include <math.h>
// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

//void GhostRacer::GRmove() {
//
//}


void GhostRacer::doSomething() {
	if (isDead())	//step 1
		return;
	if (getX() <= (ROAD_CENTER - ROAD_WIDTH / 2)) {	//step 2
		if (getDirection() > 90)
			takeDamageAndPossiblyDie(10);
		setDirection(82); 
		getWorld()->playSound(SOUND_VEHICLE_CRASH);
		GRmove();	//move ghost racer
	}
	if (getX() >= (ROAD_CENTER + ROAD_WIDTH / 2)) { //step 3
		if (getDirection() < 90)
			takeDamageAndPossiblyDie(10);
		setDirection(98); //maybe todo
		getWorld()->playSound(SOUND_VEHICLE_CRASH);
		GRmove();	//move ghost racer
	}

	//get key from user
	int ch;
	if (getWorld()->getKey(ch))
	{
		// user hit a key during this tick!
		int currDir = getDirection();
		switch (ch)
		{
		
		case KEY_PRESS_SPACE:
			if (getHolyWater() >= 1) {
				int theta = getDirection();
				int x = getX();
				int y = getY();
				if (theta > 90)
					getWorld()->addActor(new HolyWaterProjectile(getWorld(), x + sin(theta - 90) * SPRITE_HEIGHT, y + cos(theta - 90) * SPRITE_HEIGHT, theta));
				else if (theta<=90)
					getWorld()->addActor(new HolyWaterProjectile(getWorld(), x + cos(theta) * SPRITE_HEIGHT, y + sin(theta) * SPRITE_HEIGHT, theta));
				getWorld()->playSound(SOUND_PLAYER_SPRAY);
				sprayHolyWater();	//decrement holywater
			}
			GRmove();
			break;
		
		case KEY_PRESS_LEFT:
			if (currDir < 114) {
				setDirection(currDir + 8);
			}
			GRmove();
			break;
		case KEY_PRESS_RIGHT:
			if (currDir > 66) {
				setDirection(currDir - 8);
			}
			GRmove();
			break;
		case KEY_PRESS_UP:
			if (getVerSpeed() < 5) {
				setVerSpeed(getVerSpeed()+1);
			}
			GRmove();
			break;
		case KEY_PRESS_DOWN:
			if (getVerSpeed() > -1) {
				setVerSpeed(getVerSpeed() - 1);
			}
			GRmove();
			break;
		}
	}
}

void BorderLine::doSomething() {

	movePeds();
}

void GhostRacer::GRmove() {
	double max_shift_per_tick = 4.0;
	int direction = getDirection();
	double delta_x = cos(direction * 3.14159265 / 180.0) * max_shift_per_tick;
	double cur_x = getX();
	double cur_y = getY();
	moveTo(cur_x + delta_x, cur_y);
	
}


void humanPedestrians::doSomething() {
	if (!checkAlive())
		return;
	//if overlapped, die
	if (getWorld()->overlapped(this, getWorld()->getGRptr())) {
		getWorld()->getGRptr()->setDead();
		return;
	}
	movePeds();	//move peds
	decPlan();	//decrease plan
	if (getPlan() > 0)
		return;
	else
		newMovementPlan();	//new movement plan
	if (getMadness()) {
		setHorSpeed(getHorSpeed() * -1);
		if (getDirection() == 0)	//change direction
			setDirection(180);
		else
			setDirection(0);
		getWorld()->playSound(soundWhenHurt());
	}
}

void Actor::movePeds() {
	int vert_speed = getVerSpeed() - getWorld()->getGRptr()->getVerSpeed();
	int horiz_speed = getHorSpeed();
	int new_y = getY() + vert_speed;
	int new_x = getX() + horiz_speed;
	GraphObject::moveTo(new_x, new_y);
	checkOutOfBoundDead(); //check boundary
	
}

void Pedestrians::newMovementPlan() {
	setHorSpeed(randInt(randInt(-3, -1), randInt(1, 3)));
	setPlan();//set plan within 4 to 32
	if (getHorSpeed() < 0)
		setDirection(180);
	else if (getHorSpeed() > 0)
		setDirection(0);
}
void zombiePedestrians::doSomething() {
	if (!checkAlive())
		return;

	//<overlapped by ghost racer and died>
	if (getWorld()->overlapped(this, getWorld()->getGRptr())) {
		getWorld()->getGRptr()->takeDamageAndPossiblyDie(5);
		takeDamageAndPossiblyDie(2);
		getWorld()->playSound(SOUND_PED_DIE);
		addScores();
	}
	//</overlapped by ghost racer and died>


	//<meet Ghost Racer>
	if (abs(getX() - getWorld()->getGRptr()->getX()) <= 30 && getY() > getWorld()->getGRptr()->getY()) { //maybe todo
		setDirection(270);
		if (getX() < getWorld()->getGRptr()->getX())	//x coord is less than GR's x coord means it is on the left
			setHorSpeed(1);
		else if (getX() > getWorld()->getGRptr()->getX())	//y coord is less than GR's y coord means it is on the right
			setHorSpeed(-1);
		else
			setHorSpeed(0);
		decTicks();
		if (getTicks() <= 0) {
			getWorld()->playSound(SOUND_ZOMBIE_ATTACK);
			setTicksTo20();
		}
	}
	//</meet Ghost Racer>

	//<zombie move>
	movePeds(); //move peds as well as check if out of bound
	//</zombie move>

	//move to another position
	if (getPlan() > 0) {
		decPlan();
		return;
	}
	else
		newMovementPlan();
}

void zombieCabs::doSomething() {
	if (!checkAlive())
		return;
	if (getWorld()->overlapped(this, getWorld()->getGRptr())) {
		//<overlapped with GR>
		if (damagedAlready()) {
			goto step3;
		}
		else {
			getWorld()->playSound(SOUND_VEHICLE_CRASH);
			getWorld()->getGRptr()->takeDamageAndPossiblyDie(20);

			if (getX() <= getWorld()->getGRptr()->getX()) {	//on the left
				setHorSpeed(-5);
				setDirection(120 + randInt(0, 19));
			}
			else {	//on the right
				setHorSpeed(5);
				setDirection(60 - randInt(0, 19));
			}
			setDamagedAlreadyTrue();
		}
		//</overlapped with GR>
	}
step3:
	movePeds();	//moving zombie cab

	////<slow down cab>

	int lane_num = checkLane(getX());//check cab is at which lane
	Actor* temp;
	temp = checkCollisionActor(this, lane_num, true);	//check if there is a collsion avoidance actor at front
	if (temp && temp != this) {
		if (temp->getY() - getY() < 96 && temp->getY() - getY() >= 0) {	//in the 96 range
			if (getVerSpeed() > getWorld()->getGRptr()->getVerSpeed()) {	//moving forward
				
				setVerSpeed(getVerSpeed() - .5);	//slower the cab
				return;
			}
		}
	}

	temp = checkCollisionActor(this, lane_num, false);//check if there is a collsion avoidance actor behind
	if (temp && temp != this) {
		if (getY() - temp->getY() < 96 && getY() - temp->getY() >= 0) {		//in the range
			if (getVerSpeed() <= getWorld()->getGRptr()->getVerSpeed()) {	//moving downward
				
				setVerSpeed(getVerSpeed() + .5);	//slow down moving downward
				return;
			}
		}
	}


	////</slow down cab>

	////<plan decrement>
	decPlan();
	if (getPlan() > 0) {
		decPlan();
		return;
	}
	////</plan decrement>

	////<set new plan>
	else {
		setPlan();
		setVerSpeed(getVerSpeed() + randInt(-2, 2));
	}
	////</set new plan>



}

int zombieCabs::checkLane(int x) {
	const int leftWhiteBoarderline = ROAD_CENTER - ROAD_WIDTH / 2 + ROAD_WIDTH / 3;
	const int rightWhiteBoarderline = ROAD_CENTER + ROAD_WIDTH / 2 - ROAD_WIDTH / 3;
	const int leftYellowBoarderline = ROAD_CENTER - ROAD_WIDTH / 2;
	const int rightYellowBoarderline = ROAD_CENTER + ROAD_WIDTH / 2;
	const int LANE_1 = 1;
	const int LANE_2 = 2;
	const int LANE_3 = 3;
	const int INVALID_LANE = -999;
	if (getX() < leftWhiteBoarderline && getX() > leftYellowBoarderline)
		return LANE_1;
	else if (getX() < rightWhiteBoarderline && getX() > leftWhiteBoarderline)
		return LANE_2;
	else if (getX() < rightYellowBoarderline && getX() > rightWhiteBoarderline)
		return LANE_3;
	return INVALID_LANE;
}

Actor* zombieCabs::checkCollisionActor(zombieCabs* zc, int lane_num, bool isFront) {

	const int leftWhiteBoarderline = ROAD_CENTER - ROAD_WIDTH / 2 + ROAD_WIDTH / 3;
	const int rightWhiteBoarderline = ROAD_CENTER + ROAD_WIDTH / 2 - ROAD_WIDTH / 3;
	const int leftYellowBoarderline = ROAD_CENTER - ROAD_WIDTH / 2;
	const int rightYellowBoarderline = ROAD_CENTER + ROAD_WIDTH / 2;
	Actor* Fronttemp = nullptr;	//actor in front of cab
	Actor* Backtemp = nullptr;	//actor behind cab

	switch (lane_num) {
	case 1:		//at left lane
		
		Fronttemp = getWorld()->checkClosestActorAtBot(leftYellowBoarderline, leftWhiteBoarderline, zc->getY(), VIEW_HEIGHT);
		if (Fronttemp && Fronttemp != zc && isFront)	//has an actor at front
			return Fronttemp;
		
		Backtemp = getWorld()->checkClosestActorAtTop(leftYellowBoarderline, leftWhiteBoarderline, zc->getY(), 0);
		if (Backtemp && Backtemp != zc && Backtemp != getWorld()->getGRptr() && !isFront)	//has an actor behind
			return Backtemp;
		break;
	case 2:
		
		Fronttemp = getWorld()->checkClosestActorAtBot(leftWhiteBoarderline, rightWhiteBoarderline, zc->getY(), VIEW_HEIGHT);
		if (Fronttemp && Fronttemp != zc && isFront)	//has an actor at front
			return Fronttemp;
		
		Backtemp = getWorld()->checkClosestActorAtTop(leftWhiteBoarderline, rightWhiteBoarderline, zc->getY(), 0);
		if (Backtemp && Backtemp != zc && Backtemp != getWorld()->getGRptr() && !isFront)	//has an actor behind
			return Backtemp;
		break;
	case 3:
		
		Fronttemp = getWorld()->checkClosestActorAtBot(rightWhiteBoarderline, rightYellowBoarderline, zc->getY(), VIEW_HEIGHT);
		if (Fronttemp && Fronttemp != zc && isFront)	//has an actor at front
			return Fronttemp;
		
		Backtemp = getWorld()->checkClosestActorAtTop(rightWhiteBoarderline, rightYellowBoarderline, zc->getY(), 0);
		if (Backtemp && Backtemp != zc && Backtemp != getWorld()->getGRptr() && !isFront)	//has an actor behind
			return Backtemp;
		break;
	}
	return nullptr;
}

void GhostRacer::spin() {
	int curDir = getDirection();
	int randomInt = randInt(0, 1);	//random choose a direction to spin the ghostracer
	switch (randomInt) {
	case 0:
		if (curDir >= 60) {
			if (curDir - 20 >= 60)
				setDirection(curDir - randInt(5, 20));
			else
				setDirection(curDir - randInt(5, curDir - 60));
		}
	case 1:
		if (curDir <= 120) {
			if (curDir + 20 <= 120)
				setDirection(curDir + randInt(5, 20));
			else
				setDirection(curDir + randInt(5, 120 - curDir));
		}
	}
}
void OilSlick::doSomething() {
	movePeds();
	doActivity(getWorld()->getGRptr());
}

void OilSlick::doActivity(GhostRacer* gr) {
	if (getWorld()->overlapped(this, gr)) {
		getWorld()->playSound(getSound());
		gr->spin();
	}
}

void HealingGoodie::doSomething() {
	movePeds();
	doActivity(getWorld()->getGRptr());
	//todo damaged by the spray
}

void HealingGoodie::doActivity(GhostRacer* gr) {
	if (getWorld()->overlapped(this, gr)) {
		gr->getHealed(10);
		setDead();
		getWorld()->playSound(getSound());
		addScores();
	}
}

void HolyWaterGoodie::doSomething() {
	movePeds();
	doActivity(getWorld()->getGRptr());
}
void HolyWaterGoodie::doActivity(GhostRacer* gr) {
	if (getWorld()->overlapped(this, gr)) {
		gr->increaseHolyWater(10);
		setDead();
		getWorld()->playSound(getSound());
		addScores();
	}
	//todo spray
}

void SoulGoodie::doSomething() {
	movePeds();
	doActivity(getWorld()->getGRptr());
}
void SoulGoodie::doActivity(GhostRacer* gr) {
	if (getWorld()->overlapped(this, gr)) {
		getWorld()->savedOneSoulWhetherFinished();
		setDead();
		getWorld()->playSound(getSound());
		addScores();
	}

	//rotate itself by CW 10 degrees
	setDirection(getDirection()-10);	
}

void HolyWaterProjectile::doSomething() {
	if (!checkAlive())
		return;
	//<meet any sprayable actor>

	
	
	if (getWorld()->SprayedIfOverlapped(this))
		return;
		
	
	GraphObject::moveForward(SPRITE_HEIGHT);	//forward holy water
		
	//</meet any sprayable actor>

	checkOutOfBoundDead();	//if out of bound then dead

	//get their delta_x and _y, and use sqrt to calculate the distance
	int x = abs(getX() - getWorld()->getGRptr()->getX());
	int y = abs(getY() - getWorld()->getGRptr()->getY());
	if (sqrt(x * x + y * y) >= 160)
		setDead();	//out of range dead
}


void Actor::takeDamageAndPossiblyDie(int amount) {
	m_health -= amount;
	getWorld()->playSound(soundWhenHurt());	//not dead play sound when hurt
	if (getHealth() <= 0) {
		setDead();
		getWorld()->playSound(soundWhenDie());	//dead play sound when dead
	}


}

void zombiePedestrians::dropSomething() {
	getWorld()->addActor(new HealingGoodie(getWorld(), getX(), getY()));
}

void zombieCabs::dropSomething() {
	getWorld()->addActor(new OilSlick(getWorld(), getX(), getY()));
}

void zombiePedestrians::addScores() {
	getWorld()->increaseScore(150);
}

void zombieCabs::addScores() {
	getWorld()->increaseScore(200);
}

void HealingGoodie::addScores() {
	getWorld()->increaseScore(250);
}

void HolyWaterGoodie::addScores() {
	getWorld()->increaseScore(50);
}

void SoulGoodie::addScores() {
	getWorld()->increaseScore(100);
}