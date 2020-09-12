#include "Actor.h"
#include "StudentWorld.h"
#include "GameConstants.h"

using namespace std;

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

// Actor Class Definitions
// Base class for Socrates, DirtPile, Food, Pit, Pickup, and Bacteria
Actor::Actor(int startingHealth, int imageID, double startX, double startY, Direction dir, int depth, StudentWorld* world)
	: GraphObject(imageID, startX, startY, dir, depth) {
	m_alive = true;
	m_health = startingHealth;
	m_world = world;
}

double Actor::health() const {
	return m_health;
}

// Changes m_health by h, up to 100. If h dips at or below 0, Actor's state is set to dead
void Actor::increaseHealth(int h) {
	m_health += h;
	if (m_health > 100)
		m_health = 100;
	else if (m_health <= 0)
		m_alive = false;
}

inline bool Actor::alive() const {
	return m_alive;
}

inline void Actor::setAlive(bool pAlive) {
	m_alive = pAlive;
}

inline StudentWorld* Actor::getWorld() {
	return m_world;
}

// Socrates Class Definitions
Socrates::Socrates(double startX, double startY, StudentWorld* world)
	: Actor(100, IID_PLAYER, startX, startY, 0, 0, world) {
	m_sprayCharges = 20;
	m_flameCharges = 5;
	m_pAngle = 180;
}

int Socrates::getFlameCharges() const {
	return m_flameCharges;
}

void Socrates::addFlameCharges(int n) {
	m_flameCharges += n;
}

int Socrates::getSprayCharges() const {
	return m_sprayCharges;
}

void Socrates::doSomething() {
	const double PI = 4 * atan(1);
	if (!alive())
		return;
	int ch;
	if (getWorld()->getKey(ch)) {
		switch (ch) {
		case KEY_PRESS_SPACE:
			if (m_sprayCharges > 0) {
				// Adds Spray object 2*SPRITE_RADIUS pixels away
				double x = 0;
				double y = 0;
				getPositionInThisDirection(getDirection(), SPRITE_WIDTH, x, y);
				getWorld()->addActor(new Spray(x, y, getDirection(), getWorld()));
				m_sprayCharges--;
				getWorld()->playSound(SOUND_PLAYER_SPRAY);
			}
			break;
		case KEY_PRESS_ENTER:
			if (m_flameCharges > 0) {
				// Adds 16 Flame objects 2*SPRITE_RADIUS pixels away, in 22 degree increments
				for (int i = 0; i < 16; i++) {
					double x = 0;
					double y = 0;
					getPositionInThisDirection(i * 22, SPRITE_WIDTH, x, y);
					getWorld()->addActor(new Flame(x, y, i * 22, getWorld()));
				}
				m_flameCharges--;
				getWorld()->playSound(SOUND_PLAYER_FIRE);
			}
			break;
		case KEY_PRESS_LEFT:
			// Moves five degrees counter-clockwise
			setPositionalAngle(m_pAngle + 5);
			moveTo(VIEW_WIDTH / 2 + VIEW_WIDTH / 2 * cos(m_pAngle*1.0 / 360 * 2 * PI), VIEW_HEIGHT / 2 + VIEW_HEIGHT / 2 * sin(m_pAngle*1.0 / 360 * 2 * PI));
			setDirection(m_pAngle + 180);
			break;
		case KEY_PRESS_RIGHT:
			// Moves five degrees clockwise
			setPositionalAngle(m_pAngle - 5);
			moveTo(VIEW_WIDTH /2 + VIEW_WIDTH / 2 * cos(m_pAngle * 1.0 / 360 * 2 * PI), VIEW_HEIGHT / 2 + VIEW_HEIGHT / 2 * sin(m_pAngle * 1.0 / 360 * 2 * PI));
			setDirection(m_pAngle + 180);
		}
	}
	else {
		// If no key is pressed, add a spray to Socrates's inventory
		if (m_sprayCharges < 20)
			m_sprayCharges++;
	}
}

void Socrates::setPositionalAngle(Direction ang) {
	// Acounts for angles < 0 and >= 360
	while (ang < 0)
		ang += 360;

	m_pAngle = ang % 360;
}

inline bool Socrates::damageable() {
	return false;
}

inline bool Socrates::blocks() {
	return false;
}

inline bool Socrates::edible() {
	return false;
}

inline bool Socrates::noSpawnOverlap() {
	return false;
}

inline bool Socrates::enemy() {
	return false;
}

// DirtPile Class Definitions
DirtPile::DirtPile(double x, double y)
	: Actor(0, IID_DIRT, x, y, 0, 1) {

}

inline void DirtPile::doSomething() {
	// Does nothing.
	return;
}

inline bool DirtPile::damageable() {
	return true;
}

inline bool DirtPile::blocks() {
	return true;
}

inline bool DirtPile::edible() {
	return false;
}

inline bool DirtPile::noSpawnOverlap() {
	return false;
}

inline bool DirtPile::enemy() {
	return false;
}

// Food Class Definitions
Food::Food(double x, double y)
	: Actor(0, IID_FOOD, x, y, 90, 1) {

}

inline void Food::doSomething() {
	return;
}

inline bool Food::damageable() {
	return false;
}

inline bool Food::blocks() {
	return false;
}

bool Food::edible() {
	return true;
}

inline bool Food::noSpawnOverlap() {
	return true;
}

inline bool Food::enemy() {
	return false;
}

// Projectile Class Definitions
// Base class for Spray and Flame
Projectile::Projectile(double x, double y, Direction d, int imgID, int maxDistance, int damage, StudentWorld* world)
	: Actor(1, imgID, x, y, d, 1, world) {
	m_distance = 0; // Current travel distance
	m_maxDistance = maxDistance; // Max travel distance
	m_damage = damage; // Damage Projectile deals
}

inline int Projectile::getDistance() const {
	return m_distance;
}

inline void Projectile::increaseDistance(int d) {
	m_distance += d;
}

inline int Projectile::getMaxDistance() const {
	return m_maxDistance;
}

int Projectile::getDamage() const {
	return m_damage;
}

void Projectile::doSomething() {
	if (!this->alive())
		return;
	// Checks if it overlaps with a damageable object
	if (getWorld()->checkProjectileCollision(this))
		return;
	else {
		// Moves in the current direction
		moveAngle(getDirection(), SPRITE_WIDTH);
		increaseDistance(SPRITE_WIDTH);
		if (getDistance() >= getMaxDistance())
			setAlive(false);
	}
}

inline bool Projectile::damageable() {
	return false;
}

inline bool Projectile::blocks() {
	return false;
}

inline bool Projectile::edible() {
	return false;
}

inline bool Projectile::noSpawnOverlap() {
	return false;
}

inline bool Projectile::enemy() {
	return false;
}

// Flame Class Definitions
Flame::Flame(double x, double y, Direction d, StudentWorld* world)
	: Projectile(x, y, d, IID_FLAME, 32, 5, world) {

}

// Spray Class Definitions
Spray::Spray(double x, double y, Direction d, StudentWorld* world)
	: Projectile(x, y, d, IID_SPRAY, 112, 2, world) {

}

// Pickup Class Definitions
// Base class for HealthGoodie, FlamethrowerGoodie, ExtraLifeGoodie, and Fungus
Pickup::Pickup(int imageID, double x, double y, StudentWorld* world)
	: Actor(0, imageID, x, y, 0, 1, world) {
	m_age = 0; // How many ticks the Pickup has been alive for
	m_lifetime = max(randInt(0, 300 - 10 * world->getLevel() - 1), 50); // How many ticks the Pickup lasts
} 

void Pickup::doSomething() {
	if (!this->alive())
		return;
	m_age++;
	// Checks overlap with Socrates
	if (getWorld()->checkSocratesCollision(this)) {
		effectOnSocrates(); // Different for every Pickup
		this->setAlive(false);
		getWorld()->playSound(SOUND_GOT_GOODIE);
		return;
	}
	// Sets to dead if the Pickup hits its lifetime
	if (m_age == m_lifetime)
		this->setAlive(false);
}

inline bool Pickup::damageable() {
	return true;
}

inline bool Pickup::blocks() {
	return false;
}

inline bool Pickup::edible() {
	return false;
}

inline bool Pickup::noSpawnOverlap() {
	return false;
}

inline bool Pickup::enemy() {
	return false;
}

// HealthGoodie Class Definitions
HealthGoodie::HealthGoodie(double x, double y, StudentWorld* world)
	: Pickup(IID_RESTORE_HEALTH_GOODIE, x, y, world) {

}

void HealthGoodie::effectOnSocrates() {
	// Increase score, play sound, and refill health
	getWorld()->increaseScore(250);
	getWorld()->playSound(SOUND_GOT_GOODIE);
	getWorld()->increaseSocratesHealth(100);
}

// FlamethrowerGoodie Class Definitions
FlamethrowerGoodie::FlamethrowerGoodie(double x, double y, StudentWorld* world)
	: Pickup(IID_FLAME_THROWER_GOODIE, x, y, world) {

}

inline void FlamethrowerGoodie::effectOnSocrates() {
	// Increase score, play sound, and add five flame charges to Socrates
	getWorld()->increaseScore(300);
	getWorld()->playSound(SOUND_GOT_GOODIE);
	getWorld()->addCharges(5);
}

// ExtraLifeGoodie Class Definitions
ExtraLifeGoodie::ExtraLifeGoodie(double x, double y, StudentWorld* world)
	: Pickup(IID_EXTRA_LIFE_GOODIE, x, y, world) {
	
}

inline void ExtraLifeGoodie::effectOnSocrates() {
	// Increase score, play sound, and increment lives
	getWorld()->increaseScore(500);
	getWorld()->playSound(SOUND_GOT_GOODIE);
	getWorld()->incLives();
}

// Fungus Class Definitions
Fungus::Fungus(double x, double y, StudentWorld* world)
	: Pickup(IID_FUNGUS, x, y, world) {

}

inline void Fungus::effectOnSocrates() {
	// Decrease score and health
	getWorld()->increaseScore(-50);
	getWorld()->increaseSocratesHealth(-20);
}

// Pit Class Definitions
Pit::Pit(double x, double y, StudentWorld* world)
	: Actor(0, IID_PIT, x, y, 0, 1, world) {
	// Can spawn 5 RegularSalmonella, 3 AggressiveSalmonella, and 2 EColi
	m_regSalmonella = 5;
	m_aggSalmonella = 3;
	m_eColi = 2;
}

void Pit::doSomething() {
	if (m_regSalmonella == 0 && m_aggSalmonella == 0 && m_eColi == 0) {
		// Notify StudentWorld that all of bacteria has been emitted
		this->setAlive(false);
	}
	else {
		// 1 in 50 chance of spawning a bacteria every tick
		int spawnChance = rand() % 50;
		if (spawnChance == 0) {
			// Makes sure every remaining Bacteria has an equal chance of being spawned
			bool spawn = false;
			while (!spawn) {
				int fungusType = rand() % 3;
				switch (fungusType) {
				case 0:
					// Create regular salmonella
					if (m_regSalmonella > 0) {
						getWorld()->addActor(new RegularSalmonella(getX(), getY(), getWorld()));
						m_regSalmonella--;
						spawn = true;
					}
					break;
				case 1:
					// Create aggressive salmonella
					if (m_aggSalmonella > 0) {
						getWorld()->addActor(new AggressiveSalmonella(getX(), getY(), getWorld()));
						m_aggSalmonella--;
						spawn = true;
					}
					break;
				case 2:
					// Create E Coli
					if (m_eColi > 0) {
						getWorld()->addActor(new EColi(getX(), getY(), getWorld()));
						m_eColi--;
						spawn = true;
					}
				}
			}
			getWorld()->playSound(SOUND_BACTERIUM_BORN);
		}
	}
}

inline bool Pit::damageable() {
	return false;
}

inline bool Pit::blocks() {
	return false;
}

inline bool Pit::edible() {
	return false;
}

inline bool Pit::noSpawnOverlap() {
	return true;
}

inline bool Pit::enemy() {
	return true;
}

// Bacteria class definitions
// Base Class for Salmonella and EColi
Bacteria::Bacteria(int startingHealth, int imageID, double x, double y, int damage, int movementDistance, StudentWorld* world)
	: Actor(startingHealth, imageID, x, y, 90, 0, world) {
	m_foodEaten = 0; // Number of Food objects Bacteria has eaten
	m_damage = damage; // Amount of damage Bacteria does to player per tick
	m_movementDistance = movementDistance; // Amount of pixels Bacteria moves per tick
}

void Bacteria::doSomething() {
	if (!this->alive())
		return;
	// Agressive Salmonella goes after Socrates during this step
	bool extra = doSomethingExtra();
	
	// Checks collision with Socrates
	if (getWorld()->checkSocratesCollision(this)) {
		getWorld()->increaseSocratesHealth(-getDamage());
	}
	// Adds a new Bacteria nearby if three Food objects have been eaten
	else if (getFoodEaten() == 3) {
		double newx = getX();
		if (getX() < VIEW_WIDTH / 2)
			newx += SPRITE_WIDTH / 2;
		else if (getX() > VIEW_WIDTH / 2)
			newx -= SPRITE_WIDTH / 2;

		double newy = getY();
		if (getY() < VIEW_HEIGHT / 2)
			newy += SPRITE_WIDTH / 2;
		else if (getY() > VIEW_HEIGHT / 2)
			newy -= SPRITE_WIDTH / 2;

		addBacteria(newx, newy);
		resetFoodEaten();
	}
	// Eats Food object if overlap
	else if (getWorld()->checkFoodOverlap(this)) {
		incFoodEaten();
	}

	// If AggressiveSalmonella was within range of Socrates, they return here
	if (extra)
		return;

	// Salmonella and EColi attempt movement in different ways
	attemptMovement();
}

void Bacteria::increaseHealth(int h) {
	Actor::increaseHealth(h);
	if (this->alive())
		playHurtSound();
	else {
		playDieSound();
		getWorld()->increaseScore(100);
		// When Bacteria dies, there is a 50% chance it will spawn a Food object
		int foodChance = rand() % 2;
		if (foodChance == 0)
			getWorld()->addActor(new Food(getX(), getY()));
	}
}

inline bool Bacteria::damageable() {
	return true;
}

inline bool Bacteria::blocks() {
	return false;
}

inline bool Bacteria::edible() {
	return false;
}

inline bool Bacteria::noSpawnOverlap() {
	return false;
}

inline bool Bacteria::enemy() {
	return true;
}

bool Bacteria::checkMovement(int n) {
	// Moves the Bacteria n units in its current direction,
	// checks conditions, returns a bool, then moves back
	this->moveAngle(getDirection(), n);
	bool movementBlocked = getWorld()->checkDirtPileCollision(this);
	bool outOfBounds = sqrt(pow(getX() - VIEW_WIDTH / 2, 2) + pow(getY() - VIEW_HEIGHT / 2, 2)) >= VIEW_RADIUS;
	this->moveAngle(getDirection() + 180, n);

	if (movementBlocked || outOfBounds)
		return false;
	return true;
}

inline int Bacteria::getFoodEaten() const {
	return m_foodEaten;
}

inline void Bacteria::resetFoodEaten() {
	m_foodEaten = 0;
}

inline void Bacteria::incFoodEaten() {
	m_foodEaten++;
}

inline int Bacteria::getDamage() const {
	return m_damage;
}

inline int Bacteria::getMovementDistance() const {
	return m_movementDistance;
}

// Salmonella class definitions
Salmonella::Salmonella(int startingHealth, double x, double y, int damage, StudentWorld* world)
	: Bacteria(startingHealth, IID_SALMONELLA , x, y, damage, 3, world) {
	m_movementPlanDistance = 0; // Number of ticks Salmonella plans to continue moving in current direction
}

inline void Salmonella::playHurtSound() {
	getWorld()->playSound(SOUND_SALMONELLA_HURT);
}

inline void Salmonella::playDieSound() {
	getWorld()->playSound(SOUND_SALMONELLA_DIE);
}

void Salmonella::resetDirection() {
	// Resets Bacteria's direction and its movement plan distance
	int randDirection = rand() % 360;
	setDirection(randDirection);
	resetMovementPlanDistance();
}

inline int Salmonella::getMovementPlanDistance() const {
	return m_movementPlanDistance;
}

inline void Salmonella::resetMovementPlanDistance() {
	m_movementPlanDistance = 10;
}

inline void Salmonella::decMovementPlanDistance() {
	m_movementPlanDistance--;
}

void Salmonella::attemptMovement() {
	// If movement plan distance is not zero
	if (getMovementPlanDistance() > 0) {
		decMovementPlanDistance();
		// Salmonella moves in its current direction if it can
		if (checkMovement(getMovementDistance()))
			moveAngle(getDirection(), getMovementDistance());
		// Else reset the direction
		else
			resetDirection();
		return;
	}
	else {
		// If getClosetFood returns false, meaning a food was not found within 128 pixels
		Direction closestFood = 0;
		if (!getWorld()->getClosestFood(this, closestFood)) {
			resetDirection();
			return;
		}
		else {
			// If a closestFood was found, try to move in its direction, else reset direction
			setDirection(closestFood);
			if (checkMovement(getMovementDistance()))
				moveAngle(getDirection(), getMovementDistance());
			else {
				resetDirection();
				return;
			}
		}
	}
}

// RegularSalmonella class definitions
RegularSalmonella::RegularSalmonella(double x, double y, StudentWorld* world)
	: Salmonella(4, x, y, 1, world) {

}

inline bool RegularSalmonella::doSomethingExtra() {
	// RegularSalmonella doesn't do anything extra
	return false;
}

inline void RegularSalmonella::addBacteria(double x, double y) {
	getWorld()->addActor(new RegularSalmonella(x, y, getWorld()));
}

// AggressiveSalmonella class definitions
AggressiveSalmonella::AggressiveSalmonella(double x, double y, StudentWorld* world)
	: Salmonella(10, x, y, 2, world) {

}

bool AggressiveSalmonella::doSomethingExtra() {
	// If Socrates is within 72 pixels, attempt to move in their direction
	if (getWorld()->checkSocratesWithinRange(this, 72)) {
		Direction d = getWorld()->getSocratesDirection(this);
		setDirection(d);
		if (checkMovement(3))
			moveAngle(getDirection(), 3);
		return true;
	}
	// If can't move in Socrates's direction, or Socrates is not within range, do nothing
	return false;
}

inline void AggressiveSalmonella::addBacteria(double x, double y) {
	getWorld()->addActor(new AggressiveSalmonella(x, y, getWorld()));
}

// EColi class definitions
EColi::EColi(double x, double y, StudentWorld* world)
	: Bacteria(5, IID_ECOLI, x, y, 4, 2, world) {
	
}

inline bool EColi::doSomethingExtra() {
	return false;
}

inline void EColi::playHurtSound() {
	getWorld()->playSound(SOUND_ECOLI_HURT);
}

inline void EColi::playDieSound() {
	getWorld()->playSound(SOUND_ECOLI_DIE);
}

void EColi::attemptMovement() {
	// If Socrates is within 256 (which he should always be)
	if (getWorld()->checkSocratesWithinRange(this, 256)) {
		setDirection(getWorld()->getSocratesDirection(this));
		// Try to move in his direction or increments of 10 degrees
		// If EColi is blocked on every side, don't move
		for (int i = 0; i < 10; i++) {
			if (checkMovement(getMovementDistance())) {
				moveAngle(getDirection(), getMovementDistance());
				break;
			}
			else
				setDirection(getDirection() + 10);
		}
	}
}

inline void EColi::addBacteria(double x, double y) {
	getWorld()->addActor(new EColi(x, y, getWorld()));
}