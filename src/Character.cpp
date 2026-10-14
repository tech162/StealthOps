// Character.cpp
// Code for the Character base class and the distanceBetween helper.

#include "Character.h"
#include <cstdlib>   // for abs()

using namespace std;

// Steps needed to walk from a to b (up/down/left/right only).
int distanceBetween(Position a, Position b) {
    return abs(a.row - b.row) + abs(a.col - b.col);
}

// CONSTRUCTOR: gives the character a name and a starting position.
Character::Character(string characterName, Position startPosition) {
    name = characterName;
    position = startPosition;
}

string Character::getName() const {
    return name;
}

Position Character::getPosition() const {
    return position;
}

void Character::setPosition(Position newPosition) {
    position = newPosition;
}
