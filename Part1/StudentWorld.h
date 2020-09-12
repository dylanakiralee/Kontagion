#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include <string>
#include <list>

class Actor;
class Socrates;
using Direction = int;

// Students:  Add code to this file, d.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    virtual int init();
    virtual int move();
    virtual void cleanUp();
	void addFlame(double x, double y, Direction d);
	void addSpray(double x, double y, Direction d);
	void addGoodies();
	void addCharges(int n);
	void increaseSocratesHealth(int h);
	Actor* checkCollision(Actor* src);
	bool checkSocratesCollision(Actor* src);
	~StudentWorld();
private:
	Socrates *m_player;
	std::list <Actor*> m_actors;
	void updateGameStatText();
	double getDistance(double x1, double y1, double x2, double y2);
	bool checkOverlap(Actor* src, Actor* target);
	void getRandXY(double x, double y);
};

#endif // STUDENTWORLD_H_
