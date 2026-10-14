// Item.h
// An Item is something the player can pick up: a keycard, a medkit or the mission target.

#ifndef ITEM_H
#define ITEM_H

#include <string>
#include "Character.h"   // for Position

// The three kinds of items in the game.
enum ItemType { KEYCARD, MEDKIT, TARGET };

class Item {
private:
    // ENCAPSULATION: all data is private. Other classes use the public functions below.
    std::string name;     // e.g. "Red Keycard"
    ItemType type;        // KEYCARD, MEDKIT or TARGET
    Position position;    // where it lies on the map
    char symbol;          // how it looks on the map: 'a', 'b', 'c', '+', 'T'
    int points;           // score given when it is collected
    bool collected;       // true after the player picks it up

public:
    // CONSTRUCTOR
    Item(std::string itemName, ItemType itemType, Position itemPosition, char itemSymbol, int itemPoints);

    std::string getName() const;
    ItemType getType() const;
    Position getPosition() const;
    char getSymbol() const;
    int getPoints() const;
    bool isCollected() const;

    void collect();   // marks the item as picked up
};

#endif
