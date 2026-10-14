// Guard.cpp
// Code for Guard, PatrolGuard and RandomGuard.

#include "Guard.h"
#include "Level.h"
#include <cstdlib>   // for rand()

using namespace std;

// The static member must be defined once in a .cpp file. It starts at 0.
int Guard::guardCount = 0;

// The four directions a guard can face: up, down, left, right.
const Position DIRECTIONS[4] = { Position(-1, 0), Position(1, 0), Position(0, -1), Position(0, 1) };

// ---------------------------------------------------------------
// Guard (base class for all guards)
// ---------------------------------------------------------------

Guard::Guard(string guardName, Position startPosition, Position startFacing, int range)
    : Character(guardName, startPosition) {
    facing = startFacing;
    visionRange = range;
    guardCount++;          // one more guard on duty
}

Guard::~Guard() {
    guardCount--;          // this guard has left the game
}

char Guard::getSymbol() const {
    return 'G';
}

Position Guard::getFacing() const {
    return facing;
}

// Walk forward from the guard, tile by tile, in the facing direction.
// Stop at the edge of vision, at a wall, or at a locked door.
vector<Position> Guard::getVisionTiles(const Level& level) const {
    vector<Position> tiles;
    Position current = position;

    for (int step = 1; step <= visionRange; step++) {
        current = current + facing;   // uses our overloaded + operator
        if (!level.isInside(current) || level.blocksSight(current)) {
            break;
        }
        tiles.push_back(current);
    }
    return tiles;
}

// The guard sees the target if it is one of the tiles in its line of sight.
bool Guard::canSee(Position target, const Level& level) const {
    vector<Position> tiles = getVisionTiles(level);
    for (int i = 0; i < (int)tiles.size(); i++) {
        if (tiles[i] == target) {     // uses our overloaded == operator
            return true;
        }
    }
    return false;
}

// Guards always notice someone who bumps right into them, whichever way they face.
bool Guard::isCloseTo(Position target) const {
    return distanceBetween(position, target) <= 1;
}

int Guard::getGuardCount() {
    return guardCount;
}

// ---------------------------------------------------------------
// PatrolGuard
// ---------------------------------------------------------------

PatrolGuard::PatrolGuard(Position startPosition, Position startFacing, int range)
    : Guard("Patrol Guard", startPosition, startFacing, range) {
}

// Rule: step forward. If the way is blocked, turn around and step the other way.
void PatrolGuard::move(const Level& level) {
    Position next = position + facing;

    if (level.canGuardWalk(next)) {
        position = next;
        return;
    }

    // Blocked: turn around (reverse the direction).
    facing = Position(-facing.row, -facing.col);
    next = position + facing;
    if (level.canGuardWalk(next)) {
        position = next;
    }
}

// ---------------------------------------------------------------
// RandomGuard
// ---------------------------------------------------------------

RandomGuard::RandomGuard(Position startPosition, int range)
    : Guard("Roaming Guard", startPosition, Position(0, 1), range) {
}

// Rule: 1 time in 4 the guard stands still and looks in a random direction.
// Otherwise it walks one step in a random open direction.
void RandomGuard::move(const Level& level) {
    int chance = rand() % 4;
    if (chance == 0) {
        facing = DIRECTIONS[rand() % 4];   // just look around
        return;
    }

    // Collect every direction the guard can walk in.
    vector<Position> openDirections;
    for (int i = 0; i < 4; i++) {
        if (level.canGuardWalk(position + DIRECTIONS[i])) {
            openDirections.push_back(DIRECTIONS[i]);
        }
    }

    if (openDirections.empty()) {
        return;   // boxed in, stay still
    }

    // RANDOM NUMBER: pick one of the open directions.
    facing = openDirections[rand() % openDirections.size()];
    position = position + facing;
}
