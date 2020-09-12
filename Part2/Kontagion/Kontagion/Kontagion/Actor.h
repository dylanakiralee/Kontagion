#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "StudentWorld.h"
#include <algorithm>

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
class Actor : public GraphObject {
public:
	Actor(int startingHealth, int imageID, double startX, double startY, Direction dir = 0, int depth = 0, StudentWorld* world = nullptr);
	double health() const;
	virtual void increaseHealth(int h);
	bool alive() const;
	void setAlive(bool pAlive);
	StudentWorld* getWorld();
	virtual void doSomething() = 0;
	virtual bool damageable() = 0;
	virtual bool blocks() = 0;
	virtual bool edible() = 0;
	virtual bool noSpawnOverlap() = 0;
	virtual bool enemy() = 0;
private:
	int m_health;
	bool m_alive;
	StudentWorld* m_world;
};

class Socrates : public Actor {
public:
	Socrates(double startX, double startY, StudentWorld* world);
	int getFlameCharges() const;
	void addFlameCharges(int n);
	int getSprayCharges() const;
	virtual void doSomething();
	virtual bool damageable();
	virtual bool blocks();
	virtual bool edible();
	virtual bool noSpawnOverlap();
	virtual bool enemy();
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
	virtual bool edible();
	virtual bool noSpawnOverlap();
	virtual bool enemy();
};

class Food : public Actor {
public:
	Food(double x, double y);
	virtual void doSomething();
	virtual bool damageable();
	virtual bool blocks();
	virtual bool edible();
	virtual bool noSpawnOverlap();
	virtual bool enemy();
};

class Projectile : public Actor {
public:
	Projectile(double x, double y, Direction d, int imgID, int maxDistance, int damage, StudentWorld* world);
	int getDistance() const;
	void increaseDistance(int d);
	int getMaxDistance() const;
	int getDamage() const;
	virtual void doSomething();
	virtual bool damageable();
	virtual bool blocks();
	virtual bool edible();
	virtual bool noSpawnOverlap();
	virtual bool enemy();
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

class Pickup : public Actor {
public:
	Pickup(int imageID, double x, double y, StudentWorld* world);
	virtual void doSomething();
	virtual void effectOnSocrates() = 0;
	virtual bool damageable();
	virtual bool blocks();
	virtual bool edible();
	virtual bool noSpawnOverlap();
	virtual bool enemy();
private:
	int m_age;
	int m_lifetime;
};

class HealthGoodie : public Pickup {
public:
	HealthGoodie(double x, double y, StudentWorld* world);
	virtual void effectOnSocrates();
};

class FlamethrowerGoodie : public Pickup {
public:
	FlamethrowerGoodie(double x, double y, StudentWorld* world);
	virtual void effectOnSocrates();
};

class ExtraLifeGoodie : public Pickup {
public:
	ExtraLifeGoodie(double x, double y, StudentWorld* world);
	virtual void effectOnSocrates();
};

class Fungus : public Pickup {
public:
	Fungus(double x, double y, StudentWorld* world);
	virtual void effectOnSocrates();
};

class Pit : public Actor {
public:
	Pit(double x, double y, StudentWorld* world);
	virtual void doSomething();
	virtual bool damageable();
	virtual bool blocks();
	virtual bool edible();
	virtual bool noSpawnOverlap();
	virtual bool enemy();
private:
	int m_regSalmonella;
	int m_aggSalmonella;
	int m_eColi;
};

class Bacteria : public Actor {
public:
	Bacteria(int startingHealth, int imageID, double x, double y, int damage, int movementDistance, StudentWorld* world);
	virtual void doSomething();
	virtual bool doSomethingExtra() = 0;
	virtual void addBacteria(double x, double y) = 0;
	virtual void increaseHealth(int h);
	virtual void playHurtSound() = 0;
	virtual void playDieSound() = 0;
	virtual bool damageable();
	virtual bool blocks();
	virtual bool edible();
	virtual bool noSpawnOverlap();
	virtual bool enemy();
	virtual void attemptMovement() = 0;
	bool checkMovement(int n);
	int getFoodEaten() const;
	void resetFoodEaten();
	void incFoodEaten();
	int getDamage() const;
	int getMovementDistance() const;
private:
	int m_foodEaten;
	int m_damage;
	int m_movementDistance;
};

class Salmonella : public Bacteria {
public:
	Salmonella(int startingHealth, double x, double y, int damage, StudentWorld* world);
	virtual void playHurtSound();
	virtual void playDieSound();
	void resetDirection();
	int getMovementPlanDistance() const;
	void resetMovementPlanDistance();
	void decMovementPlanDistance();
	virtual void attemptMovement();
private:
	int m_movementPlanDistance;
};

class RegularSalmonella : public Salmonella {
public:
	RegularSalmonella(double x, double y, StudentWorld* world);
	virtual bool doSomethingExtra();
	virtual void addBacteria(double x, double y);
};

class AggressiveSalmonella : public Salmonella {
public:
	AggressiveSalmonella(double x, double y, StudentWorld* world);
	virtual bool doSomethingExtra();
	virtual void addBacteria(double x, double y);
};

class EColi : public Bacteria {
public:
	EColi(double x, double y, StudentWorld* world);
	virtual bool doSomethingExtra();
	virtual void playHurtSound();
	virtual void playDieSound();
	virtual void attemptMovement();
	virtual void addBacteria(double x, double y);
};

#endif // ACTOR_H_