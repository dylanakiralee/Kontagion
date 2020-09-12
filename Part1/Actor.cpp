#include "Actor.h"
#include "StudentWorld.h"
#include "GameConstants.h"

using namespace std;

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp
// Actor Base Class Definitions
Actor::Actor(int startingHealth, int imageID, double startX, double startY, Direction dir, int depth) : GraphObject(imageID, startX, startY, dir, depth) {
	m_alive = true;
	m_health = startingHealth;
}

double Actor::health() const {
	return m_health;
}

void Actor::increaseHealth(int h) {
	m_health += h;
	if (m_health > 100)
		m_health = 100;
	else if (m_health <= 0)
		m_alive = false;
}

bool Actor::alive() const {
	return m_alive;
}

void Actor::setAlive(bool pAlive) {
	m_alive = pAlive;
}

Actor::~Actor() {

}

//ActorWithPointer Class Definitions
ActorWithPointer::ActorWithPointer(int startingHealth, int imageID, double startX, double startY, Direction dir, int depth, StudentWorld* world) : Actor(startingHealth, imageID, startX, startY, dir, depth) {
	m_world = world;
}

StudentWorld* ActorWithPointer::getWorldPointer() {
	return m_world;
}

// Socrates Class Definitions
Socrates::Socrates(double startX, double startY, StudentWorld* world) : ActorWithPointer(100, IID_PLAYER, startX, startY, 0, 0, world) {
	m_sprayCharges = 20;
	m_flameCharges = 5;
	m_pAngle = 180;
}

int Socrates::getFlameCharges() {
	return m_flameCharges;
}

void Socrates::addFlameCharges(int n) {
	m_flameCharges += n;
}

int Socrates::getSprayCharges() {
	return m_sprayCharges;
}

void Socrates::doSomething() {
	const double PI = 4 * atan(1);
	if (!alive())
		return;
	int ch;
	if (getWorldPointer()->getKey(ch)) {
		switch (ch) {
		case KEY_PRESS_SPACE:
			if (m_sprayCharges > 0) {
				// ADD SPRAY OBJECT 2*SPRITE_RADIUS PIXELS AWAY
				double x = 0;
				double y = 0;
				getPositionInThisDirection(getDirection(), SPRITE_WIDTH, x, y);
				getWorldPointer()->addSpray(x, y, getDirection());
				m_sprayCharges--;
				getWorldPointer()->playSound(SOUND_PLAYER_SPRAY);
			}
			break;
		case KEY_PRESS_ENTER:
			if (m_flameCharges > 0) {
				// ADD 16 FLAME OBJECTS 2*SPRITE_RADIUS PIXELS AWAY
				for (int i = 0; i < 16; i++) {
					double x = 0;
					double y = 0;
					getPositionInThisDirection(i * 22, SPRITE_WIDTH, x, y);
					getWorldPointer()->addFlame(x, y, i * 22);
				}
				m_flameCharges--;
				getWorldPointer()->playSound(SOUND_PLAYER_FIRE);
			}
			break;
		case KEY_PRESS_LEFT:
			setPositionalAngle(m_pAngle + 5);
			moveTo(VIEW_WIDTH / 2 + VIEW_WIDTH / 2 * cos(m_pAngle*1.0 / 360 * 2 * PI), VIEW_HEIGHT / 2 + VIEW_HEIGHT / 2 * sin(m_pAngle*1.0 / 360 * 2 * PI));
			setDirection(m_pAngle + 180);
			break;
		case KEY_PRESS_RIGHT:
			setPositionalAngle(m_pAngle - 5);
			moveTo(VIEW_WIDTH /2 + VIEW_WIDTH / 2 * cos(m_pAngle * 1.0 / 360 * 2 * PI), VIEW_HEIGHT / 2 + VIEW_HEIGHT / 2 * sin(m_pAngle * 1.0 / 360 * 2 * PI));
			setDirection(m_pAngle + 180);
		}
	}
	else {
		if (m_sprayCharges < 20)
			m_sprayCharges++;
	}
}

void Socrates::setPositionalAngle(Direction ang) {
	while (ang < 0)
		ang += 360;

	m_pAngle = ang % 360;
}

bool Socrates::damageable() {
	return true;
}

bool Socrates::blocks() {
	return false;
}

// DirtPile Class Definitions
DirtPile::DirtPile(double x, double y) : Actor(1, IID_DIRT, x, y, 0, 1) {

}

void DirtPile::doSomething() {
	return;
}

bool DirtPile::damageable() {
	return true;
}

bool DirtPile::blocks() {
	return true;
}

// Food Class Definitions
Food::Food(double x, double y) : Actor(1, IID_FOOD, x, y, 90, 1) {

}

void Food::doSomething() {
	return;
}

bool Food::damageable() {
	return false;
}

bool Food::blocks() {
	return false;
}

// Projectile Class Definitions
Projectile::Projectile(double x, double y, Direction d, int imgID, int maxDistance, int damage, StudentWorld* world) : ActorWithPointer(1, imgID, x, y, d, 1, world) {
	m_distance = 0;
	m_maxDistance = maxDistance;
	m_damage = damage;
}

int Projectile::getDistance() {
	return m_distance;
}

void Projectile::increaseDistance(int d) {
	m_distance += d;
}

int Projectile::getMaxDistance() {
	return m_maxDistance;
}

int Projectile::getDamage() {
	return m_damage;
}

bool Projectile::damageable() {
	return false;
}

bool Projectile::blocks() {
	return false;
}

void Projectile::doSomething() {
	if (!this->alive())
		return;
	Actor* ptr = getWorldPointer()->checkCollision(this);
	if (ptr != nullptr && ptr->damageable()) {
		ptr->increaseHealth(-m_damage);
		this->setAlive(false);
		return;
	}
	else {
		moveAngle(getDirection(), SPRITE_WIDTH);
		increaseDistance(SPRITE_WIDTH);
		if (getDistance() >= getMaxDistance())
			setAlive(false);
	}
}

// Flame Class Definitions
Flame::Flame(double x, double y, Direction d, StudentWorld* world) : Projectile(x, y, d, IID_FLAME, 32, 5, world) {

}

Spray::Spray(double x, double y, Direction d, StudentWorld* world) : Projectile(x, y, d, IID_SPRAY, 112, 2, world) {

}

// Goodie Class Definitions
Goodie::Goodie(int imageID, double x, double y, StudentWorld* world) : ActorWithPointer(1, imageID, x, y, 0, 1, world) {
	m_age = 0;
	m_lifetime = max(randInt(0, 300 - 10 * world->getLevel() - 1), 50);
}

void Goodie::doSomething() {
	StudentWorld* world = getWorldPointer();
	m_age++;
	if (!this->alive())
		return;
	if (world->checkSocratesCollision(this)) {
		effectOnSocrates();
		return;
	}
	if (m_age == m_lifetime)
		this->setAlive(false);
}

bool Goodie::damageable() {
	return true;
}

bool Goodie::blocks() {
	return false;
}

// HealthGoodie Class Definitions
HealthGoodie::HealthGoodie(double x, double y, StudentWorld* world) : Goodie(IID_RESTORE_HEALTH_GOODIE, x, y, world) {

}

void HealthGoodie::effectOnSocrates() {
	getWorldPointer()->increaseScore(250);
	this->setAlive(false);
	getWorldPointer()->playSound(SOUND_GOT_GOODIE);
	getWorldPointer()->increaseSocratesHealth(100);
}

// FlamethrowerGoodie Class Definitions
FlamethrowerGoodie::FlamethrowerGoodie(double x, double y, StudentWorld* world) : Goodie(IID_FLAME_THROWER_GOODIE, x, y, world) {

}

void FlamethrowerGoodie::effectOnSocrates() {
	getWorldPointer()->increaseScore(300);
	this->setAlive(false);
	getWorldPointer()->playSound(SOUND_GOT_GOODIE);
	getWorldPointer()->addCharges(5);
}

// ExtraLifeGoodie Class Definitions
ExtraLifeGoodie::ExtraLifeGoodie(double x, double y, StudentWorld* world) : Goodie(IID_EXTRA_LIFE_GOODIE, x, y, world) {
	
}

void ExtraLifeGoodie::effectOnSocrates() {
	getWorldPointer()->increaseScore(500);
	this->setAlive(false);
	getWorldPointer()->playSound(SOUND_GOT_GOODIE);
	getWorldPointer()->incLives();
}

// Fungus Class Definitions
Fungus::Fungus(double x, double y, StudentWorld* world) : Goodie(IID_FUNGUS, x, y, world) {

}

void Fungus::effectOnSocrates() {
	getWorldPointer()->increaseScore(-50);
	this->setAlive(false);
	getWorldPointer()->increaseSocratesHealth(-20);
}