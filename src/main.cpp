// main.cpp
// Starting point of StealthOps. It only creates the Game object and runs it.

#include "Game.h"
#include <cstdlib>   // srand()
#include <ctime>     // time()

int main() {
    // Seed the random number generator once, so the roaming guards
    // move differently every time you play.
    srand((unsigned int)time(0));

    Game game;     // OBJECT of the Game class
    game.run();    // show the main menu and play
    return 0;
}
