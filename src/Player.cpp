// Player.cpp
// Code for the Player class.

#include "Player.h"

using namespace std;

// Makes a bar such as [######----]. value/maxValue decides how much is filled.
string makeBar(int value, int maxValue, int width) {
    int filled = (value * width) / maxValue;
    if (filled < 0) filled = 0;
    if (filled > width) filled = width;

    string bar = "[";
    for (int i = 0; i < width; i++) {
        if (i < filled) {
            bar += "#";
        } else {
            bar += "-";
        }
    }
    bar += "]";
    return bar;
}

// CONSTRUCTOR: calls the Character constructor first (base class part),
// then sets up the player's own data.
Player::Player() : Character("Agent Shadow", Position(0, 0)) {
    health = MAX_HEALTH;
    score = 0;
    noise = 0;
}

void Player::prepareForLevel(Position startPosition) {
    position = startPosition;
    inventory.clear();   // keycards only work in the level where they were found
    noise = 0;
}

void Player::restoreStats(int savedHealth, int savedScore) {
    health = savedHealth;
    score = savedScore;
    if (health > MAX_HEALTH) health = MAX_HEALTH;
    if (health < 1) health = 1;
    if (score < 0) score = 0;
}

int Player::getHealth() const {
    return health;
}

int Player::getScore() const {
    return score;
}

int Player::getNoise() const {
    return noise;
}

bool Player::isAlive() const {
    return health > 0;
}

void Player::takeDamage(int amount) {
    health = health - amount;
    if (health < 0) health = 0;
}

void Player::heal(int amount) {
    health = health + amount;
    if (health > MAX_HEALTH) health = MAX_HEALTH;
}

// Version 1 of addScore: a plain number.
void Player::addScore(int points) {
    score = score + points;
}

// Version 2 of addScore: an item tells us how many points it is worth.
void Player::addScore(const Item& item) {
    score = score + item.getPoints();
}

void Player::loseScore(int points) {
    score = score - points;
    if (score < 0) score = 0;
}

void Player::updateNoise(int actionNoise) {
    noise = noise - 2;            // old noise fades away
    if (noise < 0) noise = 0;
    noise = noise + actionNoise;  // new noise from this action
    if (noise > MAX_NOISE) noise = MAX_NOISE;
}

void Player::resetNoise() {
    noise = 0;
}

void Player::addToInventory(const Item& item) {
    inventory.push_back(item);
}

// Looks through the inventory for a keycard with the given symbol.
bool Player::hasKey(char keySymbol) const {
    for (int i = 0; i < (int)inventory.size(); i++) {
        if (inventory[i].getType() == KEYCARD && inventory[i].getSymbol() == keySymbol) {
            return true;
        }
    }
    return false;
}

bool Player::hasTarget() const {
    for (int i = 0; i < (int)inventory.size(); i++) {
        if (inventory[i].getType() == TARGET) {
            return true;
        }
    }
    return false;
}

// Returns something like "Red Keycard, Blue Keycard" or "(empty)".
string Player::getInventoryText() const {
    if (inventory.empty()) {
        return "(empty)";
    }
    string text = "";
    for (int i = 0; i < (int)inventory.size(); i++) {
        if (i > 0) text += ", ";
        text += inventory[i].getName();
    }
    return text;
}

char Player::getSymbol() const {
    return '@';
}

// FRIEND FUNCTION: not a member of Player, but allowed to read private data.
ostream& operator<<(ostream& out, const Player& player) {
    out << "Health: " << makeBar(player.health, Player::MAX_HEALTH, 10) << " " << player.health
        << "   Noise: " << makeBar(player.noise, Player::MAX_NOISE, 10) << " " << player.noise
        << "   Score: " << player.score;
    return out;
}
