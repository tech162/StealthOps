// Level.h
// A Level holds one map of the facility plus everything on it:
// items, doors and guards. It also stores the difficulty settings for that level.

#ifndef LEVEL_H
#define LEVEL_H

#include <string>
#include <vector>
#include "Character.h"
#include "Item.h"
#include "Door.h"
#include "Guard.h"

class Player;

class Level {
private:
    int number;                    // 1, 2 or 3
    std::string title;             // e.g. "The Security Wing"
    std::string briefing;          // story text shown before the level
    std::string targetName;        // the object to steal in this level

    std::vector<std::string> map;  // the walls and floor. Each string is one row.
    std::vector<Item> items;       // keycards, medkits and the target
    std::vector<Door> doors;       // locked doors
    std::vector<Guard*> guards;    // POINTERS to guards created with "new" (see Level.cpp)

    Position playerStart;

    // Difficulty settings (bigger numbers on later levels)
    int visionRange;     // how far guards can see
    int spotAlert;       // alert added when a guard sees the player
    int hearAlert;       // alert added when a guard hears the player
    int catchDamage;     // health lost when a guard catches the player
    int startingAlert;   // alert level at the start of the level

    void loadMap(const std::vector<std::string>& rows);

public:
    // CONSTRUCTOR: builds level 1, 2 or 3 from the maps written in Level.cpp.
    Level(int levelNumber);

    // DESTRUCTOR: deletes the guards that were created with "new".
    ~Level();

    // A level owns its guards, so copying a level is not allowed
    // (two copies would both try to delete the same guards).
    Level(const Level&) = delete;
    Level& operator=(const Level&) = delete;

    int getNumber() const;
    std::string getTitle() const;
    std::string getBriefing() const;
    std::string getTargetName() const;
    Position getPlayerStart() const;
    int getSpotAlert() const;
    int getHearAlert() const;
    int getCatchDamage() const;
    int getStartingAlert() const;

    bool isInside(Position p) const;
    char getTile(Position p) const;   // '#', '.', '~' or 'E'

    bool isWalkable(Position p) const;     // can the PLAYER step here?

    bool canGuardWalk(Position p) const;   // guards never walk through doors
    bool blocksSight(Position p) const;    // walls and locked doors block vision

    Door* findDoorAt(Position p);   // returns nullptr if there is no door there
    Item* findItemAt(Position p);   // returns nullptr if there is no item there

    const std::vector<Guard*>& getGuards() const;
    void moveAllGuards();

    std::string getObjective(const Player& player) const;
    void draw(const Player& player) const;
};

#endif
