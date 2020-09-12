#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include <string>
#include <list>

class Actor;
class Socrates;
class Projectile;
using Direction = int;

// Students:  Add code to this file, d.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    virtual int init();
    virtual int move();
    virtual void cleanUp();
	void addActor(Actor* a);
	void addPickups();
	void addCharges(int n);
	void increaseSocratesHealth(int h);
	bool checkProjectileCollision(Projectile* src);
	bool checkSocratesCollision(Actor* src);
	bool checkSocratesWithinRange(Actor* src, double range);
	bool checkDirtPileCollision(Actor* src);
	bool checkFoodOverlap(Actor* src);
	bool checkSpawnOverlap(double x, double y);
	bool getClosestFood(Actor* src, Direction &d);
	int getSocratesDirection(Actor* src);
	~StudentWorld();
private:
	Socrates *m_player;
	std::list <Actor*> m_actors;
	void updateGameStatText();
	double getDistance(double x1, double y1, double x2, double y2);
	int getAngleBetween(Actor* a1, Actor* a2);
	bool checkDistance(double x1, double y1, double x2, double y2, double overlapDistance);
	bool checkDistance(Actor* src, Actor* target, double overlapDistance);
	bool spawnValid(double x, double y);
	bool checkLevelEnded();
	void getRandXY(double &x, double &y);
};

#endif // STUDENTWORLD_H_
