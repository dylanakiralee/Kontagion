#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "StudentWorld.h"
#include <algorithm>

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
class Actor : public GraphObject {
public:
	Actor(int startingHealth, int imageID, double startX, double startY, Direction dir = 0, int depth = 0);
	double health() const;
	void increaseHealth(int h);
	bool alive() const;
	void setAlive(bool pAlive);
	virtual void doSomething() = 0;
	virtual bool damageable() = 0;
	virtual bool blocks() = 0;
	virtual ~Actor();
private:
	int m_health;
	bool m_alive;
};

class ActorWithPointer : public Actor {
public:
	ActorWithPointer(int startingHealth, int imageID, double startX, double startY, Direction dir = 0, int depth = 0, StudentWorld* world = nullptr);
	StudentWorld* getWorldPointer();
private:
	StudentWorld* m_world;
};

class Socrates : public ActorWithPointer {
public:
	Socrates(double startX, double startY, StudentWorld* world);
	int getFlameCharges();
	void addFlameCharges(int n);
	int getSprayCharges();
	virtual void doSomething();
	virtual bool damageable();
	virtual bool blocks();
private:
	void setPositionalAngle(Direction ang);
	int m_flameCharges;
	int m_sprayCharges;
	Direction m_pAngle;
};

class DirtPile : public Actor {
public:
	DirtPile(double x, double y);
	virtual void doSomething();
	virtual bool damageable();
	virtual bool blocks();
};

class Food : public Actor {
	Food(double x, double y);
	virtual void doSomething();
	virtual bool damageable();
	virtual bool blocks();
};

class Projectile : public ActorWithPointer {
public:
	Projectile(double x, double y, Direction d, int imgID, int maxDistance, int damage, StudentWorld* world);
	int getDistance();
	void increaseDistance(int d);
	int getMaxDistance();
	int getDamage();
	virtual bool damageable();
	virtual bool blocks();
	virtual void doSomething();
private:
	int m_distance;
	int m_maxDistance;
	int m_damage;
};

class Flame : public Projectile {
public:
	Flame(double x, double y, Direction d, StudentWorld* world);
};

class Spray : public Projectile {
public:
	Spray(double x, double y, Direction d, StudentWorld* world);
};

class Goodie : public ActorWithPointer {
public:
	Goodie(int imageID, double x, double y, StudentWorld* world);
	virtual void doSomething();
	virtual void effectOnSocrates() = 0;
	bool damageable();
	bool blocks();
private:
	int m_age;
	int m_lifetime;
};

class HealthGoodie : public Goodie {
public:
	HealthGoodie(double x, double y, StudentWorld* world);
	virtual void effectOnSocrates();
};

class FlamethrowerGoodie : public Goodie {
public:
	FlamethrowerGoodie(double x, double y, StudentWorld* world);
	virtual void effectOnSocrates();
};

class ExtraLifeGoodie : public Goodie {
public:
	ExtraLifeGoodie(double x, double y, StudentWorld* world);
	virtual void effectOnSocrates();
};

class Fungus : public Goodie {
public:
	Fungus(double x, double y, StudentWorld* world);
	virtual void effectOnSocrates();
};

#endif // ACTOR_H_