// Character.h
// Contains the Position structure and the Character base class.
// Character is the PARENT class of Player and Guard.

#ifndef CHARACTER_H
#define CHARACTER_H

#include <string>

// Position stores a place on the map as (row, column).
// We also use it to store a DIRECTION, for example (0, 1) means "one step right".
struct Position {
    int row;
    int col;

    Position(int r = 0, int c = 0) {
        row = r;
        col = c;
    }

    // OPERATOR OVERLOADING (+)
    // position + direction gives the next position.
    // Example: (2, 3) + (0, 1) = (2, 4)  -> one step to the right.
    Position operator+(const Position& other) const {
        return Position(row + other.row, col + other.col);
    }

    // OPERATOR OVERLOADING (==)
    // Two positions are equal when both row and column match.
    bool operator==(const Position& other) const {
        return row == other.row && col == other.col;
    }
};

// Number of steps between two positions (no diagonal moves).
int distanceBetween(Position a, Position b);

// BASE CLASS for every moving character in the game (the player and the guards).
// It is an ABSTRACT class because it has a pure virtual function (getSymbol).
// You cannot create a plain "Character" object, only a Player or a Guard.
class Character {
protected:
    // protected = child classes (Player, Guard) can use these directly,
    // but code outside the class family cannot.
    std::string name;
    Position position;

public:
    Character(std::string characterName, Position startPosition);

    // Virtual destructor: makes it safe to delete a child object through a base pointer.
    virtual ~Character() {}

    std::string getName() const;
    Position getPosition() const;
    void setPosition(Position newPosition);

    // PURE VIRTUAL FUNCTION: every kind of character must say how it looks on the map.
    // Player returns '@', Guard returns 'G'.
    virtual char getSymbol() const = 0;
};

#endif
