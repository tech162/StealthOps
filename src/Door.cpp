// Door.cpp
// Code for the Door class.

#include "Door.h"
#include "Player.h"
#include <cctype>   // for toupper()

using namespace std;

Door::Door(Position doorPosition, char neededKey, string neededKeyName) {
    position = doorPosition;
    keySymbol = neededKey;
    keyName = neededKeyName;
    locked = true;
}

Position Door::getPosition() const {
    return position;
}

bool Door::isLocked() const {
    return locked;
}

char Door::getKeySymbol() const {
    return keySymbol;
}

string Door::getKeyName() const {
    return keyName;
}

char Door::getSymbol() const {
    if (locked) {
        return (char)toupper(keySymbol);   // keycard 'a' opens door 'A'
    }
    return '/';                            // an open doorway
}

// The door asks the player: "do you have my keycard?"
bool Door::tryUnlock(const Player& player) {
    if (player.hasKey(keySymbol)) {
        locked = false;
        return true;
    }
    return false;
}
