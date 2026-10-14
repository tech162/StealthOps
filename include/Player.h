// Player.h
// The Player class (Agent SHADOW). It INHERITS from Character.

#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include <vector>
#include <iostream>
#include "Character.h"
#include "Item.h"

// Builds a text bar like [#####-----] for health, noise and alert.
std::string makeBar(int value, int maxValue, int width);

// INHERITANCE: Player "is a" Character, so it gets name, position,
// getPosition(), setPosition() ... from Character automatically.
class Player : public Character {
private:
    // ENCAPSULATION: health and score can only be changed through the
    // public functions below, so they can never become invalid (e.g. health 250).
    int health;
    int score;
    int noise;                    // 0 = silent, 10 = very loud
    std::vector<Item> inventory;  // VECTOR: grows as the player collects items

public:
    static const int MAX_HEALTH = 100;
    static const int MAX_NOISE = 10;

    // CONSTRUCTOR
    Player();

    // Called at the start of every level: new position, empty inventory, silence.
    void prepareForLevel(Position startPosition);

    // Used by load game / retry level.
    void restoreStats(int savedHealth, int savedScore);

    int getHealth() const;
    int getScore() const;
    int getNoise() const;
    bool isAlive() const;

    void takeDamage(int amount);
    void heal(int amount);

    // FUNCTION OVERLOADING: two functions with the same name, different parameters.
    void addScore(int points);          // add a plain number of points
    void addScore(const Item& item);    // add the points that an item is worth
    void loseScore(int points);

    // Old noise fades a little, then the noise of the new action is added.
    void updateNoise(int actionNoise);
    void resetNoise();

    void addToInventory(const Item& item);
    bool hasKey(char keySymbol) const;
    bool hasTarget() const;
    std::string getInventoryText() const;

    // Overrides the pure virtual function from Character.
    char getSymbol() const override;

    // FRIEND FUNCTION + OPERATOR OVERLOADING:
    // lets us write  cout << player;  to print the status line.
    // It is a friend so it can read the private health, score and noise directly.
    friend std::ostream& operator<<(std::ostream& out, const Player& player);
};

#endif
