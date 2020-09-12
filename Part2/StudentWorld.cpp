#include "StudentWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>
#include <list>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <stdio.h>
#include <time.h>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
	: GameWorld(assetPath)
{
	m_player = nullptr;
	srand(time(0));
}

int StudentWorld::init()
{
	m_player = new Socrates(0, VIEW_HEIGHT / 2, this);

	// Initializes Pits, Food, and DirtPiles, making sure they are within 120 pixels
	// of the middle and don't overlap with existing Food and DirtPiles
	int numPits = getLevel();
	for (int i = 0; i < numPits; i++) {
		double x = 0, y = 0;
		do {
			x = rand() % 256;
			y = rand() % 256;
		} while (!spawnValid(x, y));
		m_actors.push_back(new Pit(x, y, this));
	}

	int numFood = min(5 * getLevel(), 25);
	for (int i = 0; i < numFood; i++) {
		double x = 0, y = 0;
		do {
			x = rand() % 256;
			y = rand() % 256;
		} while (!spawnValid(x, y));
		m_actors.push_back(new Food(x, y, this));
	}

	int numDirtPiles = max(180 - 20 * getLevel(), 20);
	for (int i = 0; i < numDirtPiles; i++) {
		double x = 0, y = 0;
		do {
			x = rand() % 256;
			y = rand() % 256;
		} while (!spawnValid(x, y));
		m_actors.push_back(new DirtPile(x, y, this));
	}

    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    // Have Socrates do something
	m_player->doSomething();
	// Iterate through m_actors, calling every Actor's doSomething function if they're alive
	list<Actor*>::iterator itr = m_actors.begin();
	while (itr != m_actors.end()) {
		if ((*itr)->alive()) {
			(*itr)->doSomething();
			// If Socrates is dead, end the level
			if (!m_player->alive()) {
				playSound(SOUND_PLAYER_DIE);
				decLives();
				return GWSTATUS_PLAYER_DIED;
			}
			// If all enemies are dead, end the level
			if (checkLevelEnded()) {
				playSound(SOUND_FINISHED_LEVEL);
				return GWSTATUS_FINISHED_LEVEL;
			}
		}
		itr++;
	}

	// Remove newly-dead actors
	itr = m_actors.begin();
	while (itr != m_actors.end()) {
		if (!(*itr)->alive()) {
			delete* itr;
			itr = m_actors.erase(itr);
		}
		else
			itr++;
	}

	// Potentially add new actors
	addPickups();

	// Update game status line
	updateGameStatText();

	return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
	// Deallocate all dynamically-allocated Actors
	delete m_player;
	list<Actor*>::iterator itr = m_actors.begin();
	while (itr != m_actors.end()) {
		delete* itr;
		itr = m_actors.erase(itr);
	}
}

void StudentWorld::addActor(Actor* a) {
	// Push the Actor to the front of the list, so finding an enemy in checkLevelEnded() takes less time
	m_actors.push_front(a);
}

void StudentWorld::addPickups() {
	// Potentially generate a fungus
	int chanceFungus = max(510 - getLevel() * 10, 200);
	int randFungus = rand() % chanceFungus;
	if (randFungus == 0) {
		double x = 0; double y = 0;
		getRandXY(x, y);
		m_actors.push_back(new Fungus(x, y, this));

	}
	// Potetially generate a goodie
	int chanceGoodie = max(510 - getLevel() * 10, 250);
	int randGoodie = rand() % chanceGoodie;
	if (randGoodie == 0) {
		double x = 0; double y = 0;
		getRandXY(x, y);
		int typeGoodie = rand() % 100;
		// 60% for HealthGoodie
		if (typeGoodie < 60)
			m_actors.push_back(new HealthGoodie(x, y, this));
		// 30% chance for FlamethrowerGoodie
		else if (typeGoodie < 90)
			m_actors.push_back(new FlamethrowerGoodie(x, y, this));
		// 10% chance for ExtraLifeGoodie
		else
			m_actors.push_back(new ExtraLifeGoodie(x, y, this));
	}
}

void StudentWorld::addCharges(int n) {
	m_player->addFlameCharges(n);
}

void StudentWorld::increaseSocratesHealth(int h) {
	if (h < 0)
		playSound(SOUND_PLAYER_HURT);
	m_player->increaseHealth(h);
}

bool StudentWorld::checkProjectileCollision(Projectile* src) {
	// Iterate through m_actors, returning whether src overlaps with a damageable Actor
	list<Actor*>::iterator itr = m_actors.begin();
	while (itr != m_actors.end()) {
		if ((*itr)->damageable() && checkDistance(src, (*itr), SPRITE_WIDTH)) {
			(*itr)->increaseHealth(-src->getDamage());
			src->setAlive(false);
			return true;
		}
		++itr;
	}
	return false;
}

bool StudentWorld::checkSocratesCollision(Actor* src) {
	// Returns whether src is within SPRITE_WIDTH pixels from Socrates
	return checkDistance(src, m_player, SPRITE_WIDTH);
}

bool StudentWorld::checkSocratesWithinRange(Actor* src, double range) {
	// Returns whether src is within range pixels from Socrates
	return checkDistance(src, m_player, range);
}

bool StudentWorld::checkDirtPileCollision(Actor* src) {
	// Returns whether src is within SPRITE_WIDTH / 2 pixels of a DirtPile object
	list<Actor*>::iterator itr = m_actors.begin();
	while (itr != m_actors.end()) {
		if ((*itr)->blocks() && checkDistance(src, *itr, SPRITE_WIDTH / 2))
			return true;
		++itr;
	}
	return false;
}

bool StudentWorld::checkFoodOverlap(Actor* src) {
	// Returns whether src is within SPRITE_WIDTH pixels of a Food object
	list<Actor*>::iterator itr = m_actors.begin();
	while (itr != m_actors.end()) {
		if ((*itr)->edible() && checkDistance(src, *itr, SPRITE_WIDTH)) {
			(*itr)->setAlive(false);
			return true;
		}
		++itr;
	}
	return false;
}

bool StudentWorld::checkSpawnOverlap(double x, double y) {
	// Returns whether an object that shouldn't overlap with another is within SPRITE_WIDTH pixels of (x,y)
	list<Actor*>::iterator itr = m_actors.begin();
	while (itr != m_actors.end()) {
		if ((*itr)->noSpawnOverlap() && checkDistance(x, y, (*itr)->getX(), (*itr)->getY(), SPRITE_WIDTH))
			return true;
		itr++;
	}
	return false;
}

bool StudentWorld::getClosestFood(Actor* src, Direction &d) {
	// Returns whether a Food object is within 128 pixels of src
	// If true, sets d to the Food object's direction relative to src
	list<Actor*>::iterator itr = m_actors.begin();
	double distance = 0, minDistance = 1000; // Checks track of closest food distance
	bool foundFood = false; // Keeps track of whether a Food object was found within 128 pixels
	list<Actor*>::iterator closestFood;
	while (itr != m_actors.end()) {
		if ((*itr)->edible()) {
			distance = getDistance(src->getX(), src->getY(), (*itr)->getX(), (*itr)->getY());
			// If Actor is within 128 pixels and its distance is less than minDistance
			if (distance <= 128 && distance < minDistance) {
				minDistance = distance;
				closestFood = itr;
				foundFood = true;
			}
		}
		itr++;
	}
	// Set d to the angle between src and closestFood if food was found
	if (foundFood)
		d = getAngleBetween(src, (*closestFood));
	return foundFood;
}

int StudentWorld::getSocratesDirection(Actor* src) {
	return getAngleBetween(src, m_player);
}

StudentWorld::~StudentWorld() {
	cleanUp();
}

void StudentWorld::updateGameStatText() {
	// Updates text at the top of the screen showing Score, Level, Lives, Health, Sprays, and Flames
	ostringstream oss;
	int score = getScore();
	oss << "Score: ";
	oss.fill('0');
	if (score < 0)
		oss << "-" << setw(5) << -score;
	else
		oss << setw(6) << score;
	oss << "  ";
	oss << "Level: " << getLevel();
	oss << "  ";
	oss << "Lives: " << getLives();
	oss << "  ";
	oss.fill(' ');
	oss << "Health: ";
	oss << setw(3) << m_player->health();
	oss << "  ";
	oss << "Sprays: ";
	oss << setw(2) << m_player->getSprayCharges();
	oss << "  ";
	oss << "Flames: ";
	oss << m_player->getFlameCharges();
	setGameStatText(oss.str());
}

double StudentWorld::getDistance(double x1, double y1, double x2, double y2) {
	// Returns distance between (x1,y1) and (x2,y2)
	double distance = sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
	return distance;
}

int StudentWorld::getAngleBetween(Actor* a1, Actor* a2) {
	// Returns the angle between two objects in degrees
	const double PI = 4 * atan(1);
	double dx = a2->getX() - a1->getX();
	double dy = a2->getY() - a1->getY();
	double angle = atan2(dy, dx);
	angle *= (180 / PI);
	return (int) round(angle);
}

bool StudentWorld::checkDistance(double x1, double y1, double x2, double y2, double overlapDistance) {
	double distance = getDistance(x1, y1, x2, y2);
	return (distance <= (overlapDistance*1.0));
}

bool StudentWorld::checkDistance(Actor* src, Actor* target, double overlapDistance) {
	if (src == target)
		return false;
	double x1 = src->getX(), y1 = src->getY();
	double x2 = target->getX(), y2 = target->getY();
	return checkDistance(x1, y1, x2, y2, overlapDistance);
}

bool StudentWorld::spawnValid(double x, double y) {
	// Returns whether (x,y) is within 120 pixels of the middle of the screen and is not SPRITE_WIDTH pixels from an overlap object
	return getDistance(x, y, VIEW_WIDTH / 2, VIEW_HEIGHT / 2) <= 120 && !checkSpawnOverlap(x, y);
}

bool StudentWorld::checkLevelEnded() {
	// Iterates through m_actors, checking to see if any enemies are still alive
	list<Actor*>::iterator itr = m_actors.begin();
	while (itr != m_actors.end()) {
		if (((*itr)->enemy() && (*itr)->alive()))
			return false;
		++itr;
	}
	return true;
}

void StudentWorld::getRandXY(double &x, double &y) {
	// Sets x and y to random x and y values around the perimeter of the petri dish
	const double PI = 4 * atan(1);
	int randAngle = rand() % 360;
	x = VIEW_WIDTH / 2 + VIEW_RADIUS * cos(randAngle * 1.0 / 360 * 2 * PI);
	y = VIEW_HEIGHT / 2 + VIEW_RADIUS * sin(randAngle * 1.0 / 360 * 2 * PI);
}