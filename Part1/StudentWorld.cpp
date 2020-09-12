#include "StudentWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>
#include <list>
#include <algorithm>
#include <sstream>
#include <iomanip>
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
}

int StudentWorld::init()
{
	m_player = new Socrates(0, VIEW_HEIGHT / 2, this);

	int numDirtPiles = max(180 - 20 * getLevel(), 20);
	for (int i = 0; i < numDirtPiles; i++) {
		double x = 0;
		double y = 0;
		do {
			x = rand() % 256;
			y = rand() % 256;
		} while (getDistance(x, y, VIEW_WIDTH / 2, VIEW_HEIGHT / 2) > 120);
		m_actors.push_back(new DirtPile(x, y));
	}
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    // This code is here merely to allow the game to build, run, and terminate after you hit enter.
    // Notice that the return value GWSTATUS_PLAYER_DIED will cause our framework to end the current level.
	m_player->doSomething();
	list<Actor*>::iterator itr = m_actors.begin();
	while (itr != m_actors.end()) {
		if ((*itr)->alive()) {
			(*itr)->doSomething();
			if (!m_player->alive()) {
				playSound(SOUND_PLAYER_DIE);
				return GWSTATUS_PLAYER_DIED;
			}
			if (false /* Socrates completed level */)
				return GWSTATUS_FINISHED_LEVEL;
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
	addGoodies();

	// Update game status line
	updateGameStatText();

	return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
	delete m_player;
	list<Actor*>::iterator itr = m_actors.begin();
	while (itr != m_actors.end()) {
		delete* itr;
		itr = m_actors.erase(itr);
	}
}

void StudentWorld::addFlame(double x, double y, Direction d) {
	m_actors.push_back(new Flame(x, y, d, this));
}

void StudentWorld::addSpray(double x, double y, Direction d) {
	m_actors.push_back(new Spray(x, y, d, this));
}

void StudentWorld::addGoodies() {
	int chanceFungus = max(510 - getLevel() * 10, 200);
	int randFungus = rand() % chanceFungus;
	if (randFungus == 0) {
		double x = 0; double y = 0;
		getRandXY(x, y);
		m_actors.push_back(new Fungus(x, y, this));

	}
	int chanceGoodie = max(510 - getLevel() * 10, 250);
	int randGoodie = rand() % chanceGoodie;
	if (randGoodie == 0) {
		double x = 0; double y = 0;
		getRandXY(x, y);
		int typeGoodie = rand() % 100;
		if (typeGoodie < 60)
			m_actors.push_back(new HealthGoodie(x, y, this));
		else if (typeGoodie < 90)
			m_actors.push_back(new FlamethrowerGoodie(x, y, this));
		else
			m_actors.push_back(new ExtraLifeGoodie(x, y, this));
	}
}

void StudentWorld::addCharges(int n) {
	m_player->addFlameCharges(n);
}

void StudentWorld::increaseSocratesHealth(int h) {
	m_player->increaseHealth(h);
}

Actor* StudentWorld::checkCollision(Actor* src) {
	list<Actor*>::iterator itr = m_actors.begin();
	while (itr != m_actors.end()) {
		if (checkOverlap(src, (*itr)))
			return *itr;
		itr++;
	}
	return nullptr;
}

bool StudentWorld::checkSocratesCollision(Actor* src) {
	return checkOverlap(src, m_player);
}

StudentWorld::~StudentWorld() {
	cleanUp();
}

void StudentWorld::updateGameStatText() {
	ostringstream oss;
	oss << "Score: ";
	oss.fill('0');
	oss << setw(6) << getScore();
	oss << "  ";
	oss << "Level: " << getLevel();
	oss << "  ";
	oss << "Lives: " << getLives();
	oss << "  ";
	oss << "Health: ";
	oss << setw(3) << m_player->health();
	oss << "  ";
	oss.fill(' ');
	oss << "Sprays: ";
	oss << setw(2) << m_player->getSprayCharges();
	oss << "  ";
	oss << "Flames: ";
	oss << m_player->getFlameCharges();
	setGameStatText(oss.str());
}

double StudentWorld::getDistance(double x1, double y1, double x2, double y2) {
	double distance = sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
	return distance;
}

bool StudentWorld::checkOverlap(Actor* src, Actor* target) {
	if (src == target)
		return false;
	double x1 = src->getX(); double y1 = src->getY();
	double x2 = target->getX(); double y2 = target->getY();
	double distance = getDistance(x1, y1, x2, y2);
	return (distance <= SPRITE_WIDTH);
}

void StudentWorld::getRandXY(double x, double y) {
	const double PI = 4 * atan(1);
	int randAngle = rand() % 360;
	x = VIEW_WIDTH / 2 + VIEW_RADIUS * cos(randAngle * 1.0 / 360 * 2 * PI);
	y = VIEW_HEIGHT / 2 + VIEW_RADIUS * sin(randAngle * 1.0 / 360 * 2 * PI);
	cout << "(" << x << "," << y << ")" << endl;
}