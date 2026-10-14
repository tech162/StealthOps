// Level.cpp
// The three level maps and the code that reads and draws them.
//
// MAP LEGEND (as written below):
//   #  wall               .  floor              ~  creaky floor (makes noise)
//   @  player start       E  exit
//   a b c  keycards       A B C  locked doors (A needs a, B needs b, C needs c)
//   T  mission target     +  medkit
//   H  patrol guard walking left/right    V  patrol guard walking up/down
//   R  roaming guard (random movement)

#include "Level.h"
#include "Player.h"
#include <iostream>
#include <cctype>

using namespace std;

// Returns the name of a keycard from its letter.
string keycardName(char symbol) {
    if (symbol == 'a') return "Red Keycard";
    if (symbol == 'b') return "Blue Keycard";
    return "Gold Keycard";
}

Level::Level(int levelNumber) {
    number = levelNumber;
    vector<string> rows;

    if (number == 1) {
        title = "The Security Wing";
        targetName = "Security Override Chip";
        briefing = "The outer Security Wing guards the override chip that controls\n"
                   "the inner locks. Grab the Red Keycard, slip past the patrol in the\n"
                   "main hall, take the chip and leave through the exit.";
        visionRange = 3;
        spotAlert = 15;
        hearAlert = 5;
        catchDamage = 25;
        startingAlert = 0;

        rows.push_back("#####################");
        rows.push_back("#@...#.........#....#");
        rows.push_back("#....#.........#..T.#");
        rows.push_back("#..a.#....H..#.A....#");
        rows.push_back("#....#.........#....#");
        rows.push_back("##.#####.#######....#");
        rows.push_back("#.......~.......#####");
        rows.push_back("#..........+........#");
        rows.push_back("#...................#");
        rows.push_back("#..................E#");
        rows.push_back("#####################");
    } else if (number == 2) {
        title = "The Research Laboratory";
        targetName = "NIGHTFALL Research Drive";
        briefing = "The override chip got you into the labs. The research drive holds\n"
                   "the vault access codes. Two keycards stand between you and the\n"
                   "server room, and a roaming guard walks the corridor.";
        visionRange = 3;
        spotAlert = 20;
        hearAlert = 10;
        catchDamage = 25;
        startingAlert = 10;

        rows.push_back("#########################");
        rows.push_back("#@...#.........#........#");
        rows.push_back("#....#.........#....T...#");
        rows.push_back("#....#.........#........#");
        rows.push_back("#....#..b.H....#........#");
        rows.push_back("#....#.........#........#");
        rows.push_back("##.#####A#########B######");
        rows.push_back("#...........~...........#");
        rows.push_back("#..R................V...#");
        rows.push_back("#.......~...............#");
        rows.push_back("##.##...................#");
        rows.push_back("#.a+#................E..#");
        rows.push_back("#########################");
    } else {
        title = "The Vault";
        targetName = "NIGHTFALL Core";
        briefing = "This is it. The NIGHTFALL Core sits in the vault behind three locked\n"
                   "doors. Guards see further here and the alarm is already twitchy.\n"
                   "Take the core and make it back to the exit near where you started.";
        visionRange = 4;
        spotAlert = 25;
        hearAlert = 10;
        catchDamage = 35;
        startingAlert = 20;

        rows.push_back("###########################");
        rows.push_back("#.....#..........#........#");
        rows.push_back("#..b..#...H......#...T....#");
        rows.push_back("#.....#..........#........#");
        rows.push_back("#..+..#......c...#...R....#");
        rows.push_back("#.....#.....H....#........#");
        rows.push_back("#.....#..........#........#");
        rows.push_back("###A########B#########C####");
        rows.push_back("#.........~.......~.......#");
        rows.push_back("#.......H.................#");
        rows.push_back("#.........................#");
        rows.push_back("#.......~.........####.####");
        rows.push_back("#.E...............#...a...#");
        rows.push_back("#@.......+........#....H..#");
        rows.push_back("###########################");
    }

    loadMap(rows);
}

// Reads the map row by row. Every special letter becomes an object
// (item, door or guard) and its tile is replaced by plain floor '.'.
// Only walls, floor, creaky floor and the exit stay inside "map".
void Level::loadMap(const vector<string>& rows) {
    map = rows;

    for (int r = 0; r < (int)map.size(); r++) {
        for (int c = 0; c < (int)map[r].length(); c++) {
            char tile = map[r][c];
            Position here(r, c);

            if (tile == '@') {
                playerStart = here;
            } else if (tile == 'a' || tile == 'b' || tile == 'c') {
                items.push_back(Item(keycardName(tile), KEYCARD, here, tile, 100));
            } else if (tile == 'A' || tile == 'B' || tile == 'C') {
                char key = (char)tolower(tile);
                doors.push_back(Door(here, key, keycardName(key)));
            } else if (tile == 'T') {
                items.push_back(Item(targetName, TARGET, here, 'T', 500));
            } else if (tile == '+') {
                items.push_back(Item("Medkit", MEDKIT, here, '+', 25));
            } else if (tile == 'H') {
                // DYNAMIC MEMORY: "new" creates the guard. The Level deletes it later.
                guards.push_back(new PatrolGuard(here, Position(0, 1), visionRange));
            } else if (tile == 'V') {
                guards.push_back(new PatrolGuard(here, Position(1, 0), visionRange));
            } else if (tile == 'R') {
                guards.push_back(new RandomGuard(here, visionRange));
            } else {
                continue;   // '#', '.', '~', 'E' stay as they are
            }

            map[r][c] = '.';   // the object now sits on normal floor
        }
    }
}

Level::~Level() {
    for (int i = 0; i < (int)guards.size(); i++) {
        delete guards[i];   // free the memory taken with "new"
    }
    guards.clear();
}

int Level::getNumber() const { return number; }
string Level::getTitle() const { return title; }
string Level::getBriefing() const { return briefing; }
string Level::getTargetName() const { return targetName; }
Position Level::getPlayerStart() const { return playerStart; }
int Level::getSpotAlert() const { return spotAlert; }
int Level::getHearAlert() const { return hearAlert; }
int Level::getCatchDamage() const { return catchDamage; }
int Level::getStartingAlert() const { return startingAlert; }

bool Level::isInside(Position p) const {
    return p.row >= 0 && p.row < (int)map.size() &&
           p.col >= 0 && p.col < (int)map[p.row].length();
}

char Level::getTile(Position p) const {
    if (!isInside(p)) {
        return '#';   // outside the map counts as wall
    }
    return map[p.row][p.col];
}

// The player can walk on anything except walls and locked doors.
bool Level::isWalkable(Position p) const {
    if (getTile(p) == '#') {
        return false;
    }
    for (int i = 0; i < (int)doors.size(); i++) {
        if (doors[i].getPosition() == p && doors[i].isLocked()) {
            return false;
        }
    }
    return true;
}

// Guards stay in their own area: they never walk into any doorway.
bool Level::canGuardWalk(Position p) const {
    if (getTile(p) == '#') {
        return false;
    }
    for (int i = 0; i < (int)doors.size(); i++) {
        if (doors[i].getPosition() == p) {
            return false;
        }
    }
    return true;
}

// Walls and locked doors block a guard's line of sight. Open doors do not.
bool Level::blocksSight(Position p) const {
    return !isWalkable(p);
}

Door* Level::findDoorAt(Position p) {
    for (int i = 0; i < (int)doors.size(); i++) {
        if (doors[i].getPosition() == p) {
            return &doors[i];   // POINTER to the door inside the vector
        }
    }
    return nullptr;
}

Item* Level::findItemAt(Position p) {
    for (int i = 0; i < (int)items.size(); i++) {
        if (items[i].getPosition() == p && !items[i].isCollected()) {
            return &items[i];
        }
    }
    return nullptr;
}

const vector<Guard*>& Level::getGuards() const {
    return guards;
}

// POLYMORPHISM in action: guards[i] is a Guard*, but the correct move()
// (PatrolGuard::move or RandomGuard::move) is chosen at run time.
void Level::moveAllGuards() {
    for (int i = 0; i < (int)guards.size(); i++) {
        guards[i]->move(*this);
    }
}

// Tells the player what to do next, based on what they have collected.
string Level::getObjective(const Player& player) const {
    if (player.hasTarget()) {
        return "Escape! Reach the EXIT (E).";
    }
    for (int i = 0; i < (int)doors.size(); i++) {
        if (doors[i].isLocked()) {
            char key = doors[i].getKeySymbol();
            if (!player.hasKey(key)) {
                return string("Find the ") + doors[i].getKeyName() + " ('" + key + "').";
            }
            return string("Walk into door '") + (char)toupper(key) + "' to unlock it with the " +
                   doors[i].getKeyName() + ".";
        }
    }
    return "Steal the " + targetName + " (T).";
}

// Draws the map. We copy it, put every object on top, then print it.
void Level::draw(const Player& player) const {
    vector<string> screen = map;

    // 1. Guard vision: mark floor tiles that guards can see with ':'
    for (int g = 0; g < (int)guards.size(); g++) {
        vector<Position> seen = guards[g]->getVisionTiles(*this);
        for (int i = 0; i < (int)seen.size(); i++) {
            if (screen[seen[i].row][seen[i].col] == '.') {
                screen[seen[i].row][seen[i].col] = ':';
            }
        }
    }

    // 2. Items that are still lying on the floor
    for (int i = 0; i < (int)items.size(); i++) {
        if (!items[i].isCollected()) {
            Position p = items[i].getPosition();
            screen[p.row][p.col] = items[i].getSymbol();
        }
    }

    // 3. Doors
    for (int i = 0; i < (int)doors.size(); i++) {
        Position p = doors[i].getPosition();
        screen[p.row][p.col] = doors[i].getSymbol();
    }

    // 4. Guards (getSymbol() is a virtual function)
    for (int g = 0; g < (int)guards.size(); g++) {
        Position p = guards[g]->getPosition();
        screen[p.row][p.col] = guards[g]->getSymbol();
    }

    // 5. The player on top of everything
    Position p = player.getPosition();
    screen[p.row][p.col] = player.getSymbol();

    // Print with a space after each character so the map looks less squashed.
    for (int r = 0; r < (int)screen.size(); r++) {
        cout << "  ";
        for (int c = 0; c < (int)screen[r].length(); c++) {
            cout << screen[r][c] << ' ';
        }
        cout << "\n";
    }
}
