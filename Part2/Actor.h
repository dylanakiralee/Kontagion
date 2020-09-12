#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "StudentWorld.h"
#include <algorithm>

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
class Actor : public GraphObject {
public:
	Actor(int startingHealth, int imageID, double startX, double startY, Direction dir = 0, int depth = 0, StudentWorld* world = nullptr);
	double health() const; // Returns health of Actor
	virtual void increaseHealth(int h); // Changes health of Actor by h
	bool alive() const; // Returns state of Actor (true for alive and false for dead)
	void setAlive(bool pAlive); // Change state of Actor
	StudentWorld* getWorld(); // Get a pointer to the StudentWorld object the Actor belongs to
	virtual void doSomething() = 0; // Each Actor will do something each tick
	virtual bool damageable() = 0; // Returns whether the Actor can be damaged by Projectiles
	virtual bool blocks() = 0; // Returns whether the Actor blocks the movement of Bacteria
	virtual bool edible() = 0; // Returns whether the Actor can be aeaten by Bacteria
	virtual bool noSpawnOverlap() = 0; // Returns whether or not the Actor should not overlap with other Actors when it is created
	virtual bool enemy() = 0; // Returns whether or not the Actor must be destroyed before the level is over
private:
	int m_health;
	bool m_alive;
	StudentWorld* m_world;
};

class Socrates : public Actor {
public:
	Socrates(double startX, double startY, StudentWorld* world);
	int getFlameCharges() const; // Returns the number of flamethrower charges Socrates has remaining
	void addFlameCharges(int n); // Adds n flamethrower charges to Socrates stockpile
	int getSprayCharges() const; // Returns the number of sprays Socrates has remaining
	virtual void doSomething(); // Moves Socrates based on keyboard input
	virtual bool damageable(); // Returns false
	virtual bool blocks(); // Returns false
	virtual bool edible(); // Returns false
	virtual bool noSpawnOverlap(); // Returns false
	virtual bool enemy(); // Returns false
private:
	void setPositionalAngle(Direction ang);
	int m_flameCharges;
	int m_sprayCharges;
	Direction m_pAngle;
};

class DirtPile : public Actor {
public:
	DirtPile(double x, double y, StudentWorld* world);
	virtual void doSomething(); // Returns immediately
	virtual bool damageable(); // Returns true
	virtual bool blocks(); // Returns true
	virtual bool edible(); // Returns false
	virtual bool noSpawnOverlap(); // Returns false
	virtual bool enemy(); // Returns false
};

class Food : public Actor {
public:
	Food(double x, double y, StudentWorld* world);
	virtual void doSomething(); // Returns immediately
	virtual bool damageable(); // Returns false
	virtual bool blocks(); // Returns false
	virtual bool edible(); // Returns true
	virtual bool noSpawnOverlap(); // Returns true
	virtual bool enemy(); // Returns false
};

class Projectile : public Actor {
public:
	Projectile(double x, double y, Direction d, int imgID, int maxDistance, int damage, StudentWorld* world);
	int getDistance() const; // Returns the total distance travelled
	void increaseDistance(int d); // Increases total distance travelled by d
	int getMaxDistance() const; // Returns maximum distance Projectile can travel
	int getDamage() const; // Returns amount of damage Projectile does to enemies
	virtual void doSomething(); // Damages enemies, continues moving, or gets removed
	virtual bool damageable(); // Returns false
	virtual bool blocks(); // Returns false
	virtual bool edible(); // Returns false
	virtual bool noSpawnOverlap(); // Returns false
	virtual bool enemy(); // Returns false
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
	virtual void doSomething(); // Is picked up by Socrates or reaches its lifetime and is removed
	virtual void effectOnSocrates() = 0; // Affects Socrates in some way (different for each subclass)
	virtual bool damageable(); // Returns true
	virtual bool blocks(); // Returns false
	virtual bool edible(); // Returns false
	virtual bool noSpawnOverlap(); // Returns false
	virtual bool enemy(); // Returns false
private:
	int m_age;
	int m_lifetime;
};

class HealthGoodie : public Pickup {
public:
	HealthGoodie(double x, double y, StudentWorld* world);
	virtual void effectOnSocrates(); // Increases score and refills Socrates's health
};

class FlamethrowerGoodie : public Pickup {
public:
	FlamethrowerGoodie(double x, double y, StudentWorld* world);
	virtual void effectOnSocrates(); // Increases score and gives Socrates five more flamethrower charges
};

class ExtraLifeGoodie : public Pickup {
public:
	ExtraLifeGoodie(double x, double y, StudentWorld* world);
	virtual void effectOnSocrates(); // Increases score and gives Socrates one more life
};

class Fungus : public Pickup {
public:
	Fungus(double x, double y, StudentWorld* world);
	virtual void effectOnSocrates(); // Decreases score and Socrates's health
};

class Pit : public Actor {
public:
	Pit(double x, double y, StudentWorld* world);
	virtual void doSomething(); // Has a chance to spawn a Bacteria, if there are some left
	virtual bool damageable(); // Returns false
	virtual bool blocks(); // Returns false
	virtual bool edible(); // Returns false
	virtual bool noSpawnOverlap(); // Returns true
	virtual bool enemy(); // Returns true
private:
	int m_regSalmonella;
	int m_aggSalmonella;
	int m_eColi;
};

class Bacteria : public Actor {
public:
	Bacteria(int startingHealth, int imageID, double x, double y, int damage, int movementDistance, StudentWorld* world);
	virtual void doSomething(); // Attacks Socrates, eats Food, spawns a new Bacteria, or moves in a new or current direction, depending on the circumstances
	virtual bool doSomethingExtra() = 0; // AggressiveSalmonella does something extra, RegularSalmonella and EColi return
	virtual void addBacteria(double x, double y) = 0; // Adds a bacteria of a derived class's type at (x,y)
	virtual void increaseHealth(int h); // Changes health by h
	virtual void playHurtSound() = 0; // Plays a Bacteria's hurt sound
	virtual void playDieSound() = 0; // Plays a Bacteria's die sound
	virtual bool damageable(); // Returns true
	virtual bool blocks(); // Returns false
	virtual bool edible(); // Returns false
	virtual bool noSpawnOverlap(); // Returns false
	virtual bool enemy(); // Returns true
	virtual void attemptMovement() = 0; // Attempts to move the Bacteria in its own way
	bool checkMovement(int n); // Checks a Bacteria's movement n pixels in its current direction
	int getFoodEaten() const; // Returns the number of Food objects the Bacteria has eaten
	void resetFoodEaten(); // Resets the number of Food objects the Bacteria has eaten to zero
	void incFoodEaten(); // Increments the number of Food objects the Bacteria has eaten by one
	int getDamage() const; // Returns the amount of damage the Bacteria inflicts on the player in a collision per tick
	int getMovementDistance() const; // Returns the amount of pixels the Bacteria moves per tick
private:
	int m_foodEaten;
	int m_damage;
	int m_movementDistance;
};

class Salmonella : public Bacteria {
public:
	Salmonella(int startingHealth, double x, double y, int damage, StudentWorld* world);
	virtual void playHurtSound(); // Plays Salmonella's hurt sound
	virtual void playDieSound(); // Plays Salmonella's die sound
	void resetDirection(); // Sets the Bacteria's direction to a random angle between 0 and 359 degrees, and resets its movement plan distance
	int getMovementPlanDistance() const; // Returns the Bacteria's movement plan distance
	void resetMovementPlanDistance(); // Resets the Bacteria's movement plan distance to ten
	void decMovementPlanDistance(); // Decrements the Bacteria's movement plan distance by one
	virtual void attemptMovement(); // Moves in its current direction, towards a Food object, or resets its direction
private:
	int m_movementPlanDistance;
};

class RegularSalmonella : public Salmonella {
public:
	RegularSalmonella(double x, double y, StudentWorld* world);
	virtual bool doSomethingExtra(); // Returns false, nothing is done
	virtual void addBacteria(double x, double y); // Adds a RegularSalmonella at (x,y)
};

class AggressiveSalmonella : public Salmonella {
public:
	AggressiveSalmonella(double x, double y, StudentWorld* world);
	virtual bool doSomethingExtra(); // Reuturns true, attempts to move towards Socrates if he is within 72 pixels
	virtual void addBacteria(double x, double y); // Adds an AggressiveBacteria at (x,y)
};

class EColi : public Bacteria {
public:
	EColi(double x, double y, StudentWorld* world);
	virtual bool doSomethingExtra(); // Returns false
	virtual void playHurtSound(); // Plays EColi's hurt sound
	virtual void playDieSound(); // Plays EColi's die sound
	virtual void attemptMovement(); // Moves toward Socrates or stays still
	virtual void addBacteria(double x, double y); // Adds an EColi at (x,y)
};

#endif // ACTOR_H_