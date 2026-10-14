// Item.cpp
// Code for the Item class.

#include "Item.h"

using namespace std;

// CONSTRUCTOR: fills in every detail of the item. A new item is not collected yet.
Item::Item(string itemName, ItemType itemType, Position itemPosition, char itemSymbol, int itemPoints) {
    name = itemName;
    type = itemType;
    position = itemPosition;
    symbol = itemSymbol;
    points = itemPoints;
    collected = false;
}

string Item::getName() const {
    return name;
}

ItemType Item::getType() const {
    return type;
}

Position Item::getPosition() const {
    return position;
}

char Item::getSymbol() const {
    return symbol;
}

int Item::getPoints() const {
    return points;
}

bool Item::isCollected() const {
    return collected;
}

void Item::collect() {
    collected = true;
}
