// Game.cpp
// The main game flow: menus, levels, turns, alert system, save/load.

#include "Game.h"
#include <iostream>
#include <fstream>
#include <cstdio>    // remove()
#include <cstdlib>
#include <cctype>

using namespace std;

// Files created next to StealthOps.exe
const string SAVE_FILE = "savegame.txt";
const string SCORE_FILE = "highscore.txt";

// Game rule numbers (kept together so they are easy to find and change)
const int WALK_NOISE = 1;        // noise for a normal step
const int RUN_NOISE = 4;         // noise for running (2 steps)
const int CREAK_NOISE = 3;       // extra noise for stepping on a creaky floor '~'
const int HEARING_LIMIT = 5;     // guards hear you when noise is 5 or more...
const int HEARING_RADIUS = 6;    // ...and they are within 6 steps
const int ALERT_DECAY = 2;       // alert drops by 2 every quiet turn
const int CAUGHT_ALERT = 10;     // extra alert when caught
const int CAUGHT_PENALTY = 50;   // score lost when caught
const int MEDKIT_HEAL = 30;
const int EXIT_BONUS = 300;      // reaching the exit with the target
const int GHOST_BONUS = 200;     // finishing a level without ever being spotted

// ---------------------------------------------------------------
// Small helper functions
// ---------------------------------------------------------------

// Clears the console window.
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    cout << "\033[2J\033[H";
#endif
}

// Reads one line typed by the player.
string readInput() {
    string line;
    if (!getline(cin, line)) {
        // Only happens if the input is closed (for example Ctrl+Z). Nothing more to read.
        cout << "\nInput closed. Goodbye, Agent.\n";
        exit(0);
    }
    return line;
}

// Returns the first non-space character typed, or ' ' if the line was empty.
char readChoice() {
    string line = readInput();
    for (int i = 0; i < (int)line.length(); i++) {
        if (line[i] != ' ') {
            return line[i];
        }
    }
    return ' ';
}

void waitForEnter() {
    cout << "\nPress Enter to continue...";
    readInput();
}

// ---------------------------------------------------------------
// Constructor / destructor
// ---------------------------------------------------------------

Game::Game() {
    level = nullptr;
    alert = 0;
    spottedThisLevel = false;
    levelComplete = false;
    levelStartHealth = Player::MAX_HEALTH;
    levelStartScore = 0;
    bestScore = 0;
}

Game::~Game() {
    delete level;   // deleting nullptr is safe, so no check is needed
}

// ---------------------------------------------------------------
// Main menu
// ---------------------------------------------------------------

void Game::run() {
    loadBestScore();

    while (true) {
        showTitle();
        cout << "   1. Start New Mission\n";
        cout << "   2. Continue Saved Mission\n";
        cout << "   3. Select Level\n";
        cout << "   4. Instructions\n";
        cout << "   5. Exit\n\n";
        cout << "   Choose an option (1-5): ";

        char choice = readChoice();
        if (choice == '1') {
            playCampaign(1, Player::MAX_HEALTH, 0);
        } else if (choice == '2') {
            continueSavedGame();
        } else if (choice == '3') {
            showLevelSelect();
        } else if (choice == '4') {
            showInstructions();
        } else if (choice == '5') {
            cout << "\n   Mission aborted. See you next time, Agent.\n";
            return;
        }
        // Any other key simply shows the menu again.
    }
}

void Game::showTitle() {
    clearScreen();
    cout << R"ART(
   ____  _             _ _   _      ___
  / ___|| |_ ___  __ _| | |_| |__  / _ \ _ __  ___
  \___ \| __/ _ \/ _` | | __| '_ \| | | | '_ \/ __|
   ___) | ||  __/ (_| | | |_| | | | |_| | |_) \__ \
  |____/ \__\___|\__,_|_|\__|_| |_|\___/| .__/|___/
                                        |_|
)ART";
    cout << "        A Stealth-Based Heist Game\n";
    cout << "  ================================================\n";
    cout << "  Year 2031. Obsidian Dynamics has built NIGHTFALL,\n";
    cout << "  a weapon core locked deep inside its facility.\n";
    cout << "  You are Agent SHADOW. Get in, steal it, get out.\n";
    cout << "  ================================================\n";
    cout << "  Best score: " << bestScore << "\n\n";
}

void Game::showInstructions() {
    clearScreen();
    cout << "=================== HOW TO PLAY ===================\n\n";
    cout << " The game is turn-based. Type one or more keys and press Enter.\n";
    cout << " Example: typing  ddd  then Enter moves you 3 steps right.\n\n";
    cout << " CONTROLS\n";
    cout << "   w a s d   walk one step (up, left, down, right)  - quiet\n";
    cout << "   W A S D   run two steps (capital letters)       - LOUD\n";
    cout << "   x         wait one turn (let a guard walk past)\n";
    cout << "   h         show this help\n";
    cout << "   q         quit to the main menu\n\n";
    cout << " MAP\n";
    cout << "   @ you        G guard       : tiles a guard can see\n";
    cout << "   a b c keycards             A B C locked doors   / open door\n";
    cout << "   T target     E exit        + medkit   ~ creaky floor   # wall\n\n";
    cout << " RULES\n";
    cout << "   - Walk into a locked door while carrying its keycard to open it.\n";
    cout << "   - Guards look straight ahead in the direction they walk.\n";
    cout << "     Standing on a ':' tile means you are SPOTTED -> alert rises.\n";
    cout << "   - Standing next to a guard means you are CAUGHT -> you lose health\n";
    cout << "     and are sent back to the entry point.\n";
    cout << "   - Running and creaky floors make noise. Noise 5+ lets nearby\n";
    cout << "     guards hear you -> alert rises.\n";
    cout << "   - Alert 100% = lockdown (mission failed). Health 0 = captured.\n";
    cout << "   - Alert slowly drops on every quiet turn.\n\n";
    cout << " SCORE\n";
    cout << "   Keycard +100   Medkit +25   Target +500   Escape +300\n";
    cout << "   Ghost bonus +200 if you finish a level without being spotted.\n";
    cout << "   Getting caught costs 50 points.\n\n";
    cout << " SAVING\n";
    cout << "   The game saves automatically at the start of every level.\n";
    cout << "   Choose 'Continue Saved Mission' to restart from that level.\n";
    waitForEnter();
}

void Game::showLevelSelect() {
    clearScreen();
    cout << "================= SELECT LEVEL =================\n\n";
    cout << "   1. The Security Wing       (easy)\n";
    cout << "   2. The Research Laboratory (medium)\n";
    cout << "   3. The Vault               (hard)\n";
    cout << "   0. Back\n\n";
    cout << "   Choose a level: ";

    char choice = readChoice();
    if (choice >= '1' && choice <= '3') {
        int levelNumber = choice - '0';
        playCampaign(levelNumber, Player::MAX_HEALTH, 0);
    }
}

void Game::continueSavedGame() {
    int levelNumber, health, score;
    clearScreen();
    if (loadCheckpoint(levelNumber, health, score)) {
        cout << "Saved mission found!\n";
        cout << "  Level : " << levelNumber << "\n";
        cout << "  Health: " << health << "\n";
        cout << "  Score : " << score << "\n";
        waitForEnter();
        playCampaign(levelNumber, health, score);
    } else {
        cout << "No saved mission found. Start a new mission first.\n";
        waitForEnter();
    }
}

// ---------------------------------------------------------------
// Playing the levels
// ---------------------------------------------------------------

// Plays from firstLevel up to level 3. Handles win, lose and retry.
void Game::playCampaign(int firstLevel, int startHealth, int startScore) {
    player.restoreStats(startHealth, startScore);
    int levelNumber = firstLevel;

    while (levelNumber <= TOTAL_LEVELS) {
        LevelResult result = playLevel(levelNumber);

        if (result == LEVEL_QUIT) {
            return;   // back to main menu (the level-start save is kept)
        }

        if (result == LEVEL_LOST) {
            drawScreen();
            cout << "\n  ############  MISSION FAILED  ############\n";
            cout << "  " << loseReason << "\n\n";
            cout << "  r = retry this level    m = main menu : ";
            char choice = ' ';
            while (choice != 'r' && choice != 'm') {
                choice = (char)tolower(readChoice());
            }
            if (choice == 'm') {
                return;
            }
            // Retry: go back to the health and score we had when the level began.
            player.restoreStats(levelStartHealth, levelStartScore);
            continue;
        }

        // LEVEL_WON: add the bonuses and show a summary.
        player.addScore(EXIT_BONUS);
        if (!spottedThisLevel) {
            player.addScore(GHOST_BONUS);
        }

        clearScreen();
        cout << "\n  ==========  LEVEL " << levelNumber << " COMPLETE  ==========\n\n";
        cout << "  You escaped with the " << level->getTargetName() << ".\n\n";
        cout << "  Escape bonus : +" << EXIT_BONUS << "\n";
        if (!spottedThisLevel) {
            cout << "  Ghost bonus  : +" << GHOST_BONUS << "  (never spotted!)\n";
        } else {
            cout << "  Ghost bonus  : none (you were spotted)\n";
        }
        cout << "  Health left  : " << player.getHealth() << "\n";
        cout << "  Total score  : " << player.getScore() << "\n";
        waitForEnter();
        levelNumber++;
    }

    // All three levels finished: VICTORY
    clearScreen();
    cout << "\n  ****************************************************\n";
    cout << "  *              MISSION ACCOMPLISHED                *\n";
    cout << "  ****************************************************\n\n";
    cout << "  The NIGHTFALL Core is in Phantom Unit hands.\n";
    cout << "  Obsidian Dynamics never even knew you were there.\n\n";
    cout << "  Final score: " << player.getScore() << "\n";
    if (player.getScore() > bestScore) {
        bestScore = player.getScore();
        saveBestScore();
        cout << "  NEW BEST SCORE!\n";
    }
    deleteSave();   // the mission is over, nothing left to continue
    waitForEnter();
}

// Plays one level. Returns LEVEL_WON, LEVEL_LOST or LEVEL_QUIT.
LevelResult Game::playLevel(int levelNumber) {
    // Remove the old level (if any) and build the new one.
    delete level;
    level = new Level(levelNumber);

    player.prepareForLevel(level->getPlayerStart());
    alert = level->getStartingAlert();
    spottedThisLevel = false;
    levelComplete = false;
    loseReason = "";
    messages.clear();

    levelStartHealth = player.getHealth();
    levelStartScore = player.getScore();
    saveCheckpoint(levelNumber);   // automatic save at the start of every level

    // Mission briefing
    clearScreen();
    cout << "\n  ===== MISSION " << levelNumber << " of " << TOTAL_LEVELS << ": "
         << level->getTitle() << " =====\n\n";
    cout << level->getBriefing() << "\n\n";
    cout << "  Target: " << level->getTargetName() << "\n";
    cout << "  Guards on duty: " << Guard::getGuardCount() << "\n";
    cout << "  Starting alert: " << alert << "%\n";
    waitForEnter();

    addMessage("Mission started. Keep off the ':' tiles the guards can see.");

    while (true) {
        drawScreen();
        cout << "\n  Your move(s): ";
        string input = readInput();

        for (int i = 0; i < (int)input.length(); i++) {
            char key = input[i];

            if (key == 'q' || key == 'Q') {
                return LEVEL_QUIT;
            }
            if (key == 'h' || key == 'H') {
                showInstructions();
                break;
            }

            bool importantEvent = takeTurn(key);

            if (levelComplete) {
                return LEVEL_WON;
            }
            if (!player.isAlive()) {
                return LEVEL_LOST;
            }
            if (alert >= MAX_ALERT) {
                loseReason = "The alert reached 100%. The facility is in full lockdown.";
                return LEVEL_LOST;
            }
            if (importantEvent) {
                break;   // stop the remaining keys so the player can react
            }
        }
    }
}

// One key = one turn. Returns true if something important happened.
bool Game::takeTurn(char key) {
    Position direction(0, 0);
    bool running = false;
    bool waiting = false;

    switch (key) {
        case 'w': direction = Position(-1, 0); break;
        case 's': direction = Position(1, 0);  break;
        case 'a': direction = Position(0, -1); break;
        case 'd': direction = Position(0, 1);  break;
        case 'W': direction = Position(-1, 0); running = true; break;
        case 'S': direction = Position(1, 0);  running = true; break;
        case 'A': direction = Position(0, -1); running = true; break;
        case 'D': direction = Position(0, 1);  running = true; break;
        case 'x':
        case 'X': waiting = true; break;
        case ' ': return false;   // ignore spaces
        default:
            addMessage(string("Unknown key '") + key + "'. Press h for help.");
            return true;
    }

    int actionNoise = 0;

    if (!waiting) {
        int steps = 1;
        if (running) steps = 2;

        bool turnUsed = false;
        for (int s = 0; s < steps; s++) {
            StepResult result = tryStep(direction);
            if (result == STEP_BLOCKED) {
                break;
            }
            turnUsed = true;
            if (result == STEP_STOPPED) {
                break;   // a door was unlocked, the player stays in place
            }
            if (level->getTile(player.getPosition()) == '~') {
                actionNoise += CREAK_NOISE;
                addMessage("CREAK! The floor is noisy here.");
            }
            if (levelComplete) {
                break;
            }
        }

        if (!turnUsed) {
            return true;   // bumped into a wall or locked door: no turn passes
        }

        if (running) {
            actionNoise += RUN_NOISE;
        } else {
            actionNoise += WALK_NOISE;
        }
    }

    return endTurn(actionNoise);
}

// Tries to move the player one tile.
StepResult Game::tryStep(Position direction) {
    Position next = player.getPosition() + direction;

    if (level->getTile(next) == '#') {
        addMessage("You bump into a wall.");
        return STEP_BLOCKED;
    }

    // Locked door?
    Door* door = level->findDoorAt(next);
    if (door != nullptr && door->isLocked()) {
        if (door->tryUnlock(player)) {
            addMessage(string("Door ") + (char)toupper(door->getKeySymbol()) +
                       " unlocked with the " + door->getKeyName() + ". Walk through!");
            return STEP_STOPPED;
        }
        addMessage("Locked. You need the " + door->getKeyName() + ".");
        return STEP_BLOCKED;
    }

    player.setPosition(next);

    // Pick up anything lying here.
    Item* item = level->findItemAt(next);
    if (item != nullptr) {
        collectItem(*item);
    }

    // Exit?
    if (level->getTile(next) == 'E') {
        if (player.hasTarget()) {
            levelComplete = true;
        } else {
            addMessage("This is the exit, but you don't have the " + level->getTargetName() + " yet.");
        }
    }
    return STEP_MOVED;
}

void Game::collectItem(Item& item) {
    item.collect();
    player.addScore(item);   // overloaded addScore(const Item&)

    if (item.getType() == MEDKIT) {
        player.heal(MEDKIT_HEAL);
        addMessage("Medkit used: +" + to_string(MEDKIT_HEAL) + " health.");
    } else {
        player.addToInventory(item);
        addMessage("Picked up the " + item.getName() + " (+" + to_string(item.getPoints()) + " points).");
        if (item.getType() == TARGET) {
            addMessage("You have the " + item.getName() + "! Now get to the EXIT (E).");
        }
    }
}

// Everything that happens after the player acts: noise, guards, detection.
bool Game::endTurn(int actionNoise) {
    player.updateNoise(actionNoise);

    if (levelComplete) {
        return true;
    }

    // 1. Did the player walk right next to a guard?
    if (checkCaught()) {
        return true;
    }

    // 2. Guards take their turn.
    level->moveAllGuards();

    // 3. Did a guard walk into the player?
    if (checkCaught()) {
        return true;
    }

    bool importantEvent = false;
    const vector<Guard*>& guards = level->getGuards();

    // 4. Can any guard SEE the player?
    for (int i = 0; i < (int)guards.size(); i++) {
        if (guards[i]->canSee(player.getPosition(), *level)) {
            alert += level->getSpotAlert();
            spottedThisLevel = true;
            addMessage("SPOTTED by a " + guards[i]->getName() + "! Alert +" +
                       to_string(level->getSpotAlert()) + "%.");
            importantEvent = true;
            break;   // count it once per turn
        }
    }

    // 5. Can any guard HEAR the player?
    if (player.getNoise() >= HEARING_LIMIT) {
        for (int i = 0; i < (int)guards.size(); i++) {
            if (distanceBetween(guards[i]->getPosition(), player.getPosition()) <= HEARING_RADIUS) {
                alert += level->getHearAlert();
                addMessage("A guard HEARD you! Alert +" + to_string(level->getHearAlert()) + "%.");
                importantEvent = true;
                break;
            }
        }
    }

    // 6. A quiet turn lets the alert calm down a little.
    if (!importantEvent) {
        alert -= ALERT_DECAY;
        if (alert < 0) alert = 0;
    }
    if (alert > MAX_ALERT) alert = MAX_ALERT;

    return importantEvent;
}

// If any guard is on or next to the player's tile, the player is caught.
bool Game::checkCaught() {
    const vector<Guard*>& guards = level->getGuards();

    for (int i = 0; i < (int)guards.size(); i++) {
        if (guards[i]->isCloseTo(player.getPosition())) {
            player.takeDamage(level->getCatchDamage());
            player.loseScore(CAUGHT_PENALTY);
            alert += CAUGHT_ALERT;
            if (alert > MAX_ALERT) alert = MAX_ALERT;
            spottedThisLevel = true;

            addMessage("CAUGHT by a " + guards[i]->getName() + "! -" +
                       to_string(level->getCatchDamage()) + " health, -" +
                       to_string(CAUGHT_PENALTY) + " points.");

            if (player.isAlive()) {
                player.setPosition(level->getPlayerStart());
                player.resetNoise();
                addMessage("You break free and retreat to the entry point.");
            } else {
                loseReason = "Your health reached 0. The guards captured you.";
            }
            return true;
        }
    }
    return false;
}

// Draws the whole game screen: title, map, legend, status and messages.
void Game::drawScreen() {
    clearScreen();
    cout << "  === LEVEL " << level->getNumber() << "/" << TOTAL_LEVELS << ": "
         << level->getTitle() << " ===   Guards on duty: " << Guard::getGuardCount() << "\n\n";

    level->draw(player);

    cout << "\n  @ you  G guard  : guard can see  a/b/c keycard  A/B/C locked door\n";
    cout << "  / open door  T target  E exit  + medkit  ~ creaky floor\n\n";

    cout << "  " << player << "\n";   // uses the friend operator<<
    cout << "  Alert:  " << makeBar(alert, MAX_ALERT, 10) << " " << alert << "%";
    if (alert >= 70) {
        cout << "   !! DANGER - LOCKDOWN AT 100% !!";
    }
    cout << "\n";
    cout << "  Inventory: " << player.getInventoryText() << "\n";
    cout << "  Objective: " << level->getObjective(player) << "\n\n";

    cout << "  --- Mission log ---\n";
    for (int i = 0; i < (int)messages.size(); i++) {
        cout << "  > " << messages[i] << "\n";
    }
    cout << "\n  w/a/s/d walk | W/A/S/D run (loud) | x wait | h help | q menu\n";
}

// Keeps only the 5 newest messages.
void Game::addMessage(string text) {
    messages.push_back(text);
    if (messages.size() > 5) {
        messages.erase(messages.begin());
    }
}

// ---------------------------------------------------------------
// FILE HANDLING
// ---------------------------------------------------------------

// Writes the level number, health and score to savegame.txt (one per line).
void Game::saveCheckpoint(int levelNumber) {
    ofstream file(SAVE_FILE);
    if (!file) {
        addMessage("Warning: the game could not be saved.");
        return;
    }
    file << levelNumber << "\n";
    file << player.getHealth() << "\n";
    file << player.getScore() << "\n";
    file.close();
}

// Reads savegame.txt. Returns false if there is no valid save.
bool Game::loadCheckpoint(int& levelNumber, int& health, int& score) {
    ifstream file(SAVE_FILE);
    if (!file) {
        return false;   // file does not exist
    }
    levelNumber = 0;
    health = 0;
    score = -1;
    file >> levelNumber >> health >> score;
    file.close();

    if (levelNumber < 1 || levelNumber > TOTAL_LEVELS || health < 1 || score < 0) {
        return false;   // missing or damaged data
    }
    return true;
}

void Game::deleteSave() {
    remove(SAVE_FILE.c_str());
}

void Game::loadBestScore() {
    ifstream file(SCORE_FILE);
    bestScore = 0;
    if (file) {
        file >> bestScore;
        if (bestScore < 0) bestScore = 0;
    }
}

void Game::saveBestScore() {
    ofstream file(SCORE_FILE);
    if (file) {
        file << bestScore << "\n";
    }
}
