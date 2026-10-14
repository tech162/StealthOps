// Guard.h
// Guards patrol the facility. There are two kinds:
//   PatrolGuard - walks in a straight line and turns back at walls ('H' and 'V' on the maps)
//   RandomGuard - wanders in random directions ('R' on the maps)

#ifndef GUARD_H
#define GUARD_H

#include <string>
#include <vector>
#include "Character.h"

class Level;   // forward declaration (Level.h includes Guard.h, so we avoid a loop)

// INHERITANCE: Guard "is a" Character.
// Guard is also an ABSTRACT class: move() is pure virtual, so only
// PatrolGuard and RandomGuard objects can be created.
class Guard : public Character {
protected:
    Position facing;     // direction the guard walks and looks, e.g. (0, 1) = right
    int visionRange;     // how many tiles ahead the guard can see

    // STATIC DATA MEMBER: one shared counter for ALL guards (not one per guard).
    static int guardCount;

public:
    Guard(std::string guardName, Position startPosition, Position startFacing, int range);
    virtual ~Guard();

    char getSymbol() const override;   // 'G'
    Position getFacing() const;

    // The tiles in a straight line in front of the guard (stops at walls and locked doors).
    std::vector<Position> getVisionTiles(const Level& level) const;

    bool canSee(Position target, const Level& level) const;
    bool isCloseTo(Position target) const;   // same tile or right next to the guard

    // PURE VIRTUAL FUNCTION: each type of guard moves in its own way.
    // This is where POLYMORPHISM happens: the Level keeps a list of Guard*
    // and calls move() without knowing which kind of guard it is.
    virtual void move(const Level& level) = 0;

    static int getGuardCount();
};

// Walks forward; when blocked, turns around.
class PatrolGuard : public Guard {
public:
    PatrolGuard(Position startPosition, Position startFacing, int range);
    void move(const Level& level) override;
};

// Wanders randomly. Sometimes stops to look around.
class RandomGuard : public Guard {
public:
    RandomGuard(Position startPosition, int range);
    void move(const Level& level) override;
};

#endif
