// Game.h
// The Game class controls everything: the main menu, the levels,
// each turn of play, the alert system, and saving/loading.

#ifndef GAME_H
#define GAME_H

#include <string>
#include <vector>
#include "Player.h"
#include "Level.h"

// What happened when a level ended.
enum LevelResult { LEVEL_WON, LEVEL_LOST, LEVEL_QUIT };

// What happened when the player tried to take one step.
enum StepResult { STEP_BLOCKED, STEP_MOVED, STEP_STOPPED };

class Game {
private:
    Player player;
    Level* level;               // the level being played (created with new, deleted when done)

    int alert;                  // 0 to 100. At 100 the facility locks down = mission failed.
    bool spottedThisLevel;      // used for the "Ghost" bonus
    bool levelComplete;
    std::string loseReason;

    int levelStartHealth;       // stats at the start of the level (used to retry / save)
    int levelStartScore;
    int bestScore;

    std::vector<std::string> messages;   // the mission log shown under the map

    static const int TOTAL_LEVELS = 3;
    static const int MAX_ALERT = 100;

    // Menu screens
    void showTitle();
    void showInstructions();
    void showLevelSelect();
    void continueSavedGame();

    // Playing
    void playCampaign(int firstLevel, int startHealth, int startScore);
    LevelResult playLevel(int levelNumber);
    bool takeTurn(char key);                 // returns true if something important happened
    StepResult tryStep(Position direction);
    void collectItem(Item& item);
    bool endTurn(int actionNoise);
    bool checkCaught();
    void drawScreen();
    void addMessage(std::string text);

    // File handling
    void saveCheckpoint(int levelNumber);
    bool loadCheckpoint(int& levelNumber, int& health, int& score);
    void deleteSave();
    void loadBestScore();
    void saveBestScore();

public:
    Game();
    ~Game();
    void run();   // shows the main menu until the player chooses Exit
};

#endif
