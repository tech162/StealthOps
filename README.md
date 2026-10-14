# StealthOps: A Stealth-Based Heist Game

A turn-based stealth game written in **C++** using object-oriented programming, with a
matching **browser version** built in HTML, CSS and JavaScript.

You play Agent SHADOW. Sneak through three levels of a secure facility, collect keycards,
unlock doors, avoid the guards, steal the target and escape before the alarm reaches 100%.

---

## Contents

1. [Story and objectives](#1-story-and-objectives)
2. [Features](#2-features)
3. [Controls](#3-controls)
4. [How the game works (rules)](#4-how-the-game-works-rules)
5. [Folder structure](#5-folder-structure)
6. [Required software](#6-required-software)
7. [Open the project in VS Code](#7-open-the-project-in-vs-code)
8. [Compile the C++ game on Windows](#8-compile-the-c-game-on-windows)
9. [Run the C++ game and where the output appears](#9-run-the-c-game-and-where-the-output-appears)
10. [Open the browser (HTML) version](#10-open-the-browser-html-version)
11. [Is the frontend connected to the C++ engine?](#11-is-the-frontend-connected-to-the-c-engine)
12. [How to complete all three levels](#12-how-to-complete-all-three-levels)
13. [Save and load](#13-save-and-load)
14. [The C++ classes explained](#14-the-c-classes-explained)
15. [OOP concepts and where they are used](#15-oop-concepts-and-where-they-are-used)
16. [Common errors and solutions](#16-common-errors-and-solutions)
17. [Viva questions with short answers](#17-viva-questions-with-short-answers)
18. [What was tested](#18-what-was-tested)

---

## 1. Story and objectives

> Year 2031. Obsidian Dynamics has built **NIGHTFALL**, a weapon core hidden deep inside
> its research facility. You are **Agent SHADOW** of the Phantom Unit. Get in, steal it,
> get out.

| Level | Name | What you steal | Why you need it |
|---|---|---|---|
| 1 | The Security Wing | Security Override Chip | It controls the inner locks, so it gets you into the labs |
| 2 | The Research Laboratory | NIGHTFALL Research Drive | It holds the access codes for the vault |
| 3 | The Vault | NIGHTFALL Core | The final prize |

In every level you must:

1. Find the keycards.
2. Unlock the matching doors.
3. Pick up the target (T).
4. Reach the exit (E) **with** the target.

You **lose** if your health reaches 0 (captured) or the alert reaches 100% (lockdown).

---

## 2. Features

- Main menu: Start New Mission, Continue Saved Mission, Select Level, Instructions, Exit
- Three levels that get harder (more guards, longer vision, higher starting alert, more damage)
- Walls, rooms, corridors, keycards, locked doors, guards, medkits, creaky floors, target and exit
- Inventory of collected keycards and the target
- Two kinds of guards: **patrol guards** (walk back and forth) and **roaming guards** (random)
- Guards look in the direction they walk; their line of sight is shown on the map
- Alert meter, noise meter, health, score
- Score bonuses for keycards, the target, escaping and finishing a level without being spotted
- Win screen, game-over screen, retry level, next level
- Automatic save at the start of every level, plus a saved best score (file handling)
- A browser version with the same maps and rules, a stealth-themed interface,
  health/alert/noise bars, inventory panel, mission log, pause and restart

---

## 3. Controls

### C++ console version

The console game is **turn-based**. Type one or more keys, then press **Enter**.
For example `ddd` + Enter moves three steps right.

| Key | Action |
|---|---|
| `w` `a` `s` `d` | Walk one step up / left / down / right (quiet) |
| `W` `A` `S` `D` (capital) | Run two steps (loud) |
| `x` | Wait one turn |
| `h` | Show the help screen |
| `q` | Quit to the main menu |

If something important happens (you are spotted, heard or caught, or you bump into a wall),
the rest of the keys you typed are ignored so you can react.

### Browser version

| Key | Action |
|---|---|
| `W` `A` `S` `D` or arrow keys | Walk one step |
| `Shift` + direction | Run two steps (or tick the **Run** box) |
| `Space` or `X` | Wait one turn |
| `P` | Pause / resume |
| `R` | Restart the level |
| `Esc` | Back to the main menu |

You can also click the on-screen arrow buttons below the map.

### Map symbols (console)

| Symbol | Meaning |
|---|---|
| `@` | You |
| `G` | Guard |
| `:` | A tile a guard can see right now |
| `a` `b` `c` | Red, Blue and Gold keycards |
| `A` `B` `C` | Locked doors (A needs a, B needs b, C needs c) |
| `/` | Open door |
| `T` | Target |
| `E` | Exit |
| `+` | Medkit |
| `~` | Creaky floor (noisy) |
| `#` | Wall |

---

## 4. How the game works (rules)

Every action is one **turn**. After you act, the guards move, then the game checks
whether you were seen, heard or caught.

| Rule | Details |
|---|---|
| Vision | A guard sees in a straight line in the direction it is facing (3 tiles in levels 1-2, 4 tiles in level 3). Walls and locked doors block its view. |
| Spotted | If you stand on a tile a guard can see, the alert goes up (+15% / +20% / +25% for levels 1 / 2 / 3). |
| Caught | If you are on or next to a guard (up, down, left or right), you lose health (25 / 25 / 35), lose 50 points, the alert rises 10%, and you are sent back to the entry point. |
| Noise | Walking = 1, running = 4, creaky floor = +3. Old noise fades by 2 each turn. Maximum 10. |
| Heard | If your noise is 5 or more and a guard is within 6 steps, the alert goes up (+5% / +10% / +10%). |
| Calming down | On every turn where you are not spotted or heard, the alert drops by 2%. |
| Lockdown | Alert 100% = mission failed. |
| Medkit | +30 health (maximum 100). |

### Score

| Event | Points |
|---|---|
| Keycard | +100 |
| Medkit | +25 |
| Target | +500 |
| Escape (reach the exit with the target) | +300 |
| Ghost bonus (finish a level without being spotted or caught) | +200 |
| Getting caught | -50 |

A perfect run through all three levels scores roughly 3,600 to 3,700 points.

---

## 5. Folder structure

```
StealthOps/
│
├── src/                  C++ source files (.cpp) - the code
│   ├── main.cpp          Starting point: creates the Game and runs it
│   ├── Character.cpp     Base class for Player and Guard
│   ├── Player.cpp        The player: health, score, noise, inventory
│   ├── Guard.cpp         Guard, PatrolGuard, RandomGuard
│   ├── Item.cpp          Keycards, medkits, target
│   ├── Door.cpp          Locked doors
│   ├── Level.cpp         The three maps and level settings, drawing the map
│   └── Game.cpp          Menus, turns, alert system, save/load
│
├── include/              C++ header files (.h) - the class declarations
│   ├── Character.h       (also contains the Position structure)
│   ├── Player.h
│   ├── Guard.h
│   ├── Item.h
│   ├── Door.h
│   ├── Level.h
│   └── Game.h
│
├── frontend/             Browser version
│   ├── index.html        Page structure (menu, game screen, pop-up window)
│   ├── style.css         Colours, layout, tiles
│   └── script.js         The game in JavaScript (same rules and maps)
│
├── README.md             This file
└── compile.bat           Double-click (or run) to build StealthOps.exe
```

Files created when you build and play (not included in the ZIP):

- `StealthOps.exe` - the compiled game (made by `compile.bat`)
- `savegame.txt` - the automatic save
- `highscore.txt` - your best score

There is no `assets` folder because the game needs no images or sounds: the console version
uses text characters and the browser version draws everything with CSS.

---

## 6. Required software

| Software | Needed for | Free? |
|---|---|---|
| **Visual Studio Code** | Opening and reading the project | Yes |
| **MinGW-w64 (g++ compiler)** | Compiling the C++ game | Yes |
| **A web browser** (Chrome, Edge or Firefox) | Playing the browser version | Yes |
| C/C++ extension for VS Code (optional) | Colour highlighting, error underlines | Yes |

No internet connection, database, account or paid service is needed.

### Check if g++ is already installed

1. Press the Windows key, type `cmd`, press Enter.
2. Type this and press Enter:

   ```
   g++ --version
   ```

3. If you see something like `g++ (Rev..., Built by MSYS2 project) 13.2.0`, you are ready.
   If you see `'g++' is not recognized...`, install it as shown below.

### Install g++ (MinGW-w64 through MSYS2)

1. Download and install **MSYS2** from https://www.msys2.org (keep the default folder `C:\msys64`).
2. When it finishes, an MSYS2 window opens. Type this and press Enter, then press `Y` when asked:

   ```
   pacman -S mingw-w64-ucrt-x86_64-gcc
   ```

3. Add the compiler to PATH so Windows can find it:
   - Press the Windows key, type **environment variables**, open **Edit the system environment variables**.
   - Click **Environment Variables...**
   - Under **User variables**, select **Path** and click **Edit**.
   - Click **New** and type `C:\msys64\ucrt64\bin`
   - Click **OK** on every window.
4. **Close and reopen** cmd / VS Code, then run `g++ --version` again.

---

## 7. Open the project in VS Code

1. Extract `StealthOps.zip`. You get one folder called `StealthOps`.
2. Open VS Code.
3. Click **File > Open Folder...** and choose the `StealthOps` folder (the one that
   contains `src`, `include`, `frontend` and `compile.bat`).
4. Open a terminal inside VS Code: **Terminal > New Terminal**.
   The terminal opens at the bottom and is already inside the `StealthOps` folder.

---

## 8. Compile the C++ game on Windows

"Compiling" turns the `.cpp` text files into a program, `StealthOps.exe`.
You only need to do this once (and again after you change the code).

### Option A: use compile.bat (easiest)

In the VS Code terminal type:

```
.\compile.bat
```

(In PowerShell, which is VS Code's default terminal, you must type `.\` before the file name.
In the classic `cmd` window you can type just `compile.bat`.)

You can also simply double-click `compile.bat` in File Explorer.

When it works you will see:

```
[OK] Build finished: StealthOps.exe is now in this folder.
```

### Option B: type the g++ command yourself

From inside the `StealthOps` folder:

```
g++ -std=c++17 -Wall -Iinclude src/main.cpp src/Character.cpp src/Player.cpp src/Guard.cpp src/Item.cpp src/Door.cpp src/Level.cpp src/Game.cpp -o StealthOps.exe
```

What each part means:

| Part | Meaning |
|---|---|
| `g++` | The C++ compiler |
| `-std=c++17` | Use the C++17 standard |
| `-Wall` | Show helpful warnings |
| `-Iinclude` | Look for header files (`.h`) in the `include` folder |
| `src/....cpp` | All eight source files |
| `-o StealthOps.exe` | Name of the program to create |

The shorter form `g++ -std=c++17 -Iinclude src/*.cpp -o StealthOps.exe` also works with MinGW,
but the full list above is the safest.

---

## 9. Run the C++ game and where the output appears

Compiling does **not** start the game. Run it separately:

```
.\StealthOps.exe
```

(or `StealthOps.exe` in cmd).

| Question | Answer |
|---|---|
| Where is the program? | `StealthOps.exe` is created directly in the `StealthOps` folder, next to `compile.bat`. |
| Where does the game appear? | If you run it from the VS Code terminal, the game is shown **inside that terminal** at the bottom of VS Code. If you double-click `StealthOps.exe` in File Explorer, it opens in **its own black console window**. |
| Where are saves written? | `savegame.txt` and `highscore.txt` appear in the folder the game was started from (the `StealthOps` folder). |

Tip: drag the terminal panel up, or maximise it, so the whole map fits on screen.
The game clears the screen after every turn.

An example of the game screen:

```
  === LEVEL 1/3: The Security Wing ===   Guards on duty: 1

  # # # # # # # # # # # # # # # # # # # # #
  # @ . . . # . . . . . . . . . # . . . . #
  # . . . . # . . . . . . . . . # . . T . #
  # . . a . # : : : G . . . # . A . . . . #
  # . . . . # . . . . . . . . . # . . . . #
  ...
  Health: [##########] 100   Noise: [----------] 0   Score: 0
  Alert:  [----------] 0%
  Inventory: (empty)
  Objective: Find the Red Keycard ('a').
```

---

## 10. Open the browser (HTML) version

The browser version needs **no server and no installation**.

1. Open the `StealthOps\frontend` folder in File Explorer.
2. Double-click `index.html`. It opens in your default browser.
   (Or right-click it > **Open with** > Chrome / Edge / Firefox.)
3. Click **Start Mission**, read the briefing, click **Begin**, and play with the keyboard.

From VS Code you can also right-click `frontend/index.html` in the Explorer panel and choose
**Reveal in File Explorer**, then double-click it.

If the map looks too small or too big, use the browser zoom (`Ctrl` + `+` / `Ctrl` + `-`).

---

## 11. Is the frontend connected to the C++ engine?

**No. They are two separate, complete versions of the same game.**

- The **C++ version** (`src/`, `include/`) is the main project. It is the one that
  demonstrates the OOP concepts and runs in the console.
- The **browser version** (`frontend/`) is a JavaScript re-implementation of the same game:
  the **same three maps**, the **same class design** (Player, Guard, PatrolGuard,
  RandomGuard, Item, Door, Level, Game), the **same rule numbers** and the same turn order.
  It exists to give the game an attractive visual interface.

Why they are not linked: a web page cannot run a Windows `.exe` program by itself.
Connecting them would need a small web server written in C++ (or another language) that the
page talks to over the network. That adds networking code, a server to start and stop, and
more things that can go wrong during a demo. For a 2-3 day college project, two working
versions that follow the same rules are simpler and more reliable.

How to explain it in the viva: *"The game logic is written in C++. To present it with a
graphical interface, we ported the same classes and rules to JavaScript, which browsers can
run. Both versions play identically, but they run separately."*

---

## 12. How to complete all three levels

General tips:

- Watch which way each guard faces. Guards cannot see behind or beside them.
- Never stand right next to a guard, even behind it: that counts as caught.
- Use `x` (wait) to let a guard walk past, then move.
- Walk, don't run. Running is only worth it when no guard is nearby.
- Creaky floors (`~`) add noise. Never step on two noisy things in a row.
- Guards never walk through doorways, so a doorway is a safe place to pause.
- Keycards stay in your inventory until the level ends.

### Level 1: The Security Wing (1 guard)

1. The **Red Keycard** (`a`) is in your starting room. Pick it up.
2. Leave the room through the gap in its bottom wall and walk right along the lower corridor.
3. Enter the main hall through the gap above the creaky floor tile.
4. The guard walks left and right along the hall's middle row. It turns around at the
   pillar just before door **A**, so the space next to the door is safe.
5. Wait until the guard walks away to the left, then move along the bottom row of the
   hall to the right, go up beside the pillar and walk into door **A** to unlock it.
6. Take the **Security Override Chip** (`T`), come back out the same way, go down to the
   lower corridor and walk to the exit **E** in the bottom-right corner.

### Level 2: The Research Laboratory (3 guards)

1. The **Red Keycard** (`a`) is in the small storeroom in the bottom-left corner
   (with a medkit). A **roaming guard** wanders the corridor, so keep an eye on it.
2. Door **A** is in the corridor's top wall and leads into the lab. The **Blue Keycard**
   (`b`) is two steps inside. A guard patrols that exact row: step in when it is walking
   away at the far end, grab the card and step straight back out.
3. Door **B** is further right on the corridor's top wall. A guard patrols up and down
   two columns to its right. Unlock **B** when that guard is at the bottom of its route.
4. The **NIGHTFALL Research Drive** (`T`) is in the server room behind door **B**.
5. Return through door **B** and go to the exit **E** in the bottom-right, timing your
   moves around the up-and-down guard.

### Level 3: The Vault (5 guards)

1. A guard sweeps the whole middle row of the big corridor. Cross that row only right
   behind it, or when it is far away.
2. The **Red Keycard** (`a`) is in the guard post (bottom-right room), right below its
   entrance. A guard walks left and right inside the post. Wait until it is at one end,
   step in, grab the card, step out.
3. Door **A** (top-left) leads to the archive: the **Blue Keycard** (`b`) and a medkit.
   No guards inside.
4. Door **B** (top-middle) leads to the central hall with two patrol guards. The
   **Gold Keycard** (`c`) is in the middle. Slip between the two patrol rows.
5. Door **C** (top-right) leads to the vault with a **roaming guard**. Take the
   **NIGHTFALL Core** (`T`).
6. Walk all the way back to the exit **E**, which is right next to where you started.
   Your health is lower here and every guard hurts more, so use waits instead of risks.

---

## 13. Save and load

### C++ version (file handling)

- When a level starts, the game writes `savegame.txt` with three numbers:

  ```
  2       <- level number
  75      <- health at the start of that level
  1150    <- score at the start of that level
  ```

- **Continue Saved Mission** in the main menu reads this file with `ifstream` and starts that
  level with that health and score.
- When you finish all three levels the save is deleted and, if you beat it, the best score
  is written to `highscore.txt`. The best score is shown on the title screen.
- If the file is missing or damaged, the game says "No saved mission found".

The code is in `Game::saveCheckpoint`, `Game::loadCheckpoint`, `Game::deleteSave`,
`Game::loadBestScore` and `Game::saveBestScore` in `src/Game.cpp`.

## 14. The C++ classes explained

```
Character  (abstract base class)
├── Player
└── Guard  (abstract)
    ├── PatrolGuard
    └── RandomGuard

Item, Door        objects inside a level
Level             owns the map, items, doors and guards
Game              runs everything
```

| Class | File | Purpose | Important functions |
|---|---|---|---|
| `Position` (struct) | `Character.h` | A row and column on the map, also used as a direction | `operator+`, `operator==` |
| `Character` | `Character.h/.cpp` | Common data for anyone who moves: name and position | `getPosition()`, `setPosition()`, pure virtual `getSymbol()` |
| `Player` | `Player.h/.cpp` | Health, score, noise and inventory of Agent SHADOW | `takeDamage()`, `heal()`, `addScore()`, `updateNoise()`, `hasKey()`, `hasTarget()` |
| `Guard` | `Guard.h/.cpp` | Common guard logic: facing direction, line of sight, catching | `getVisionTiles()`, `canSee()`, `isCloseTo()`, pure virtual `move()` |
| `PatrolGuard` | `Guard.h/.cpp` | Walks straight, turns around when blocked | `move()` |
| `RandomGuard` | `Guard.h/.cpp` | Random walk, sometimes stops to look around | `move()` |
| `Item` | `Item.h/.cpp` | A keycard, medkit or target lying on the map | `collect()`, `getPoints()` |
| `Door` | `Door.h/.cpp` | A door that needs a matching keycard | `tryUnlock()`, `getSymbol()` |
| `Level` | `Level.h/.cpp` | Builds a map from text, stores its items/doors/guards and difficulty, draws it | `loadMap()`, `isWalkable()`, `canGuardWalk()`, `blocksSight()`, `moveAllGuards()`, `getObjective()`, `draw()` |
| `Game` | `Game.h/.cpp` | Menus, level flow, turns, alert, win/lose, save/load | `run()`, `playCampaign()`, `playLevel()`, `takeTurn()`, `tryStep()`, `endTurn()`, `checkCaught()` |

### How one turn works (`Game::takeTurn` and `Game::endTurn`)

1. Read the key. Work out the direction (and whether you are running).
2. `tryStep()` moves you one tile, or opens a door if you have the keycard, or stops at a wall.
   Any item on the new tile is collected.
3. Your noise is updated.
4. If you are next to a guard: caught.
5. Every guard moves (`level->moveAllGuards()`).
6. If a guard is now next to you: caught.
7. If a guard can see you: alert goes up.
8. If you are loud and a guard is close: alert goes up.
9. If nothing happened, the alert drops a little.
10. `playLevel()` checks: reached the exit with the target = win; health 0 or alert 100 = lose.

### How the map is stored

Each level is written as text in `Level.cpp`, for example `"#..a.#....H..#.A....#"`.
`Level::loadMap()` reads every character. Letters such as `a`, `A`, `T`, `H` become
`Item`, `Door` or `Guard` objects and their place on the map becomes floor (`.`).
Only walls, floor, creaky floor and the exit stay in the `map` vector.

---

## 15. OOP concepts and where they are used

| Concept | Where | Why it is used |
|---|---|---|
| **Classes and objects** | Every file. `main.cpp` creates `Game game;` | Each part of the game (player, guard, door...) is its own class |
| **Constructors** | `Player::Player()`, `Guard::Guard(...)`, `Level::Level(int)`, `Item::Item(...)`, `Door::Door(...)`. `Player` calls the `Character` constructor in its initialiser list | Every object starts in a valid state (full health, doors locked, level loaded) |
| **Destructor** | `Level::~Level()` deletes the guards; `Guard::~Guard()` lowers the guard count; `Game::~Game()` deletes the level | Frees memory created with `new` |
| **Encapsulation** | `Player` keeps `health`, `score`, `noise`, `inventory` private. Health only changes through `takeDamage()` / `heal()` which keep it between 0 and 100 | Prevents invalid values |
| **Inheritance** | `Player : public Character`, `Guard : public Character`, `PatrolGuard : public Guard`, `RandomGuard : public Guard` | Shared name/position code is written once |
| **Abstract classes / pure virtual functions** | `Character::getSymbol() = 0`, `Guard::move() = 0` | A plain "Character" or plain "Guard" makes no sense in the game, only specific kinds |
| **Virtual functions and polymorphism** | `Level` stores `vector<Guard*> guards`. `Level::moveAllGuards()` calls `guards[i]->move(*this)`. At run time C++ calls `PatrolGuard::move` or `RandomGuard::move` depending on the real object | The level moves all guards with one loop without knowing their type |
| **Function overloading** | `Player::addScore(int points)` and `Player::addScore(const Item& item)` | Add bonus points (`addScore(300)`) or an item's points (`addScore(item)`) with the same name |
| **Operator overloading** | `Position::operator+` (position + direction = next position) and `Position::operator==` (compare positions). Also `operator<<` for `Player` | Movement code reads naturally: `next = position + facing` |
| **Friend function** | `friend ostream& operator<<(ostream&, const Player&)` in `Player.h` | Lets `cout << player;` print the status line by reading private health, noise and score |
| **Static data members** | `static int Guard::guardCount` (increased in the constructor, decreased in the destructor), shown as "Guards on duty". Also `Player::MAX_HEALTH`, `Player::MAX_NOISE`, `Game::TOTAL_LEVELS` | One counter shared by all guards instead of one per guard |
| **Vectors** | `vector<Item> inventory`, `vector<string> map`, `vector<Item> items`, `vector<Door> doors`, `vector<Guard*> guards`, `vector<string> messages` | Lists that grow and shrink as the game runs |
| **Arrays** | `const Position DIRECTIONS[4]` in `Guard.cpp` | The four directions a guard can face |
| **Strings** | Names, map rows, messages, inventory text | Text everywhere |
| **Pointers and dynamic memory** | `new PatrolGuard(...)` / `new RandomGuard(...)` in `Level::loadMap()`, `delete` in `Level::~Level()`. `Game` holds `Level* level` (`new Level(n)` for each level). `findDoorAt()` / `findItemAt()` return pointers or `nullptr` | Needed for polymorphism (a vector of base-class pointers) and to change the current level |
| **File handling** | `ofstream` / `ifstream` in `Game::saveCheckpoint`, `loadCheckpoint`, `loadBestScore`, `saveBestScore` | Save progress and best score between runs |
| **Random numbers** | `srand(time(0))` in `main.cpp`, `rand()` in `RandomGuard::move()` | Roaming guards behave differently each game |
| **Enumerations** | `ItemType`, `LevelResult`, `StepResult` | Readable names instead of magic numbers |

---

