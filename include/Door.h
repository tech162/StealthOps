// Door.h
// A Door blocks the way until the player has the matching keycard.
// Door 'A' needs keycard 'a', door 'B' needs keycard 'b', and so on.

#ifndef DOOR_H
#define DOOR_H

#include <string>
#include "Character.h"   // for Position

class Player;   // forward declaration: we only use a reference to Player here

class Door {
private:
    Position position;
    char keySymbol;          // the keycard that opens this door ('a', 'b' or 'c')
    std::string keyName;     // e.g. "Red Keycard" (used in messages)
    bool locked;

public:
    // CONSTRUCTOR: every door starts locked.
    Door(Position doorPosition, char neededKey, std::string neededKeyName);

    Position getPosition() const;
    bool isLocked() const;
    char getKeySymbol() const;
    std::string getKeyName() const;

    // 'A', 'B' or 'C' while locked, '/' once it is open.
    char getSymbol() const;

    // Opens the door if the player carries the right keycard. Returns true if it opened.
    bool tryUnlock(const Player& player);
};

#endif
