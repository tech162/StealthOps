/* =========================================================
   StealthOps - script.js
   Browser version of the game. It uses the SAME maps, the
   SAME classes and the SAME rule numbers as the C++ version
   (see src/ folder), rewritten in JavaScript so it can run
   in a web browser.
   ========================================================= */

// ---------------------------------------------------------
// 1. GAME RULES (same numbers as Game.cpp)
// ---------------------------------------------------------
const RULES = {
    maxHealth: 100,
    maxAlert: 100,
    maxNoise: 10,
    walkNoise: 1,        // noise for a normal step
    runNoise: 4,         // noise for running (2 steps)
    creakNoise: 3,       // extra noise on a creaky floor
    noiseFade: 2,        // noise drops by 2 before each action
    hearingLimit: 5,     // guards hear you at noise 5 or more...
    hearingRadius: 6,    // ...if they are within 6 steps
    alertDecay: 2,       // alert drops by 2 on a quiet turn
    caughtAlert: 10,
    caughtPenalty: 50,
    medkitHeal: 30,
    keycardPoints: 100,
    medkitPoints: 25,
    targetPoints: 500,
    exitBonus: 300,
    ghostBonus: 200
};

const KEY_NAMES = { a: "Red Keycard", b: "Blue Keycard", c: "Gold Keycard" };

// ---------------------------------------------------------
// 2. LEVEL DATA (same maps as Level.cpp)
//   #  wall   .  floor   ~  creaky floor   @  start   E  exit
//   a b c  keycards     A B C  locked doors   T  target   +  medkit
//   H  guard walking left/right   V  guard walking up/down   R  roaming guard
// ---------------------------------------------------------
const LEVELS = [
    {
        title: "The Security Wing",
        targetName: "Security Override Chip",
        briefing: "The outer Security Wing guards the override chip that controls the inner locks. " +
                  "Grab the Red Keycard, slip past the patrol in the main hall, take the chip and leave through the exit.",
        visionRange: 3, spotAlert: 15, hearAlert: 5, catchDamage: 25, startingAlert: 0,
        map: [
            "#####################",
            "#@...#.........#....#",
            "#....#.........#..T.#",
            "#..a.#....H..#.A....#",
            "#....#.........#....#",
            "##.#####.#######....#",
            "#.......~.......#####",
            "#..........+........#",
            "#...................#",
            "#..................E#",
            "#####################"
        ]
    },
    {
        title: "The Research Laboratory",
        targetName: "NIGHTFALL Research Drive",
        briefing: "The override chip got you into the labs. The research drive holds the vault access codes. " +
                  "Two keycards stand between you and the server room, and a roaming guard walks the corridor.",
        visionRange: 3, spotAlert: 20, hearAlert: 10, catchDamage: 25, startingAlert: 10,
        map: [
            "#########################",
            "#@...#.........#........#",
            "#....#.........#....T...#",
            "#....#.........#........#",
            "#....#..b.H....#........#",
            "#....#.........#........#",
            "##.#####A#########B######",
            "#...........~...........#",
            "#..R................V...#",
            "#.......~...............#",
            "##.##...................#",
            "#.a+#................E..#",
            "#########################"
        ]
    },
    {
        title: "The Vault",
        targetName: "NIGHTFALL Core",
        briefing: "This is it. The NIGHTFALL Core sits in the vault behind three locked doors. " +
                  "Guards see further here and the alarm is already twitchy. Take the core and make it back to the exit near where you started.",
        visionRange: 4, spotAlert: 25, hearAlert: 10, catchDamage: 35, startingAlert: 20,
        map: [
            "###########################",
            "#.....#..........#........#",
            "#..b..#...H......#...T....#",
            "#.....#..........#........#",
            "#..+..#......c...#...R....#",
            "#.....#.....H....#........#",
            "#.....#..........#........#",
            "###A########B#########C####",
            "#.........~.......~.......#",
            "#.......H.................#",
            "#.........................#",
            "#.......~.........####.####",
            "#.E...............#...a...#",
            "#@.......+........#....H..#",
            "###########################"
        ]
    }
];

// Directions written as {row, col} steps.
const UP = { row: -1, col: 0 };
const DOWN = { row: 1, col: 0 };
const LEFT = { row: 0, col: -1 };
const RIGHT = { row: 0, col: 1 };
const DIRECTIONS = [UP, DOWN, LEFT, RIGHT];

function addPos(a, b) { return { row: a.row + b.row, col: a.col + b.col }; }
function samePos(a, b) { return a.row === b.row && a.col === b.col; }
function distance(a, b) { return Math.abs(a.row - b.row) + Math.abs(a.col - b.col); }

// ---------------------------------------------------------
// 3. CLASSES (same design as the C++ classes)
// ---------------------------------------------------------

class Character {
    constructor(name, position) {
        this.name = name;
        this.position = position;
    }
}

class Player extends Character {
    constructor() {
        super("Agent Shadow", { row: 0, col: 0 });
        this.health = RULES.maxHealth;
        this.score = 0;
        this.noise = 0;
        this.inventory = [];   // list of collected Item objects
    }
    prepareForLevel(start) {
        this.position = start;
        this.inventory = [];
        this.noise = 0;
    }
    restoreStats(health, score) {
        this.health = Math.max(1, Math.min(RULES.maxHealth, health));
        this.score = Math.max(0, score);
    }
    isAlive() { return this.health > 0; }
    takeDamage(amount) { this.health = Math.max(0, this.health - amount); }
    heal(amount) { this.health = Math.min(RULES.maxHealth, this.health + amount); }
    addScore(points) { this.score += points; }
    loseScore(points) { this.score = Math.max(0, this.score - points); }
    updateNoise(actionNoise) {
        this.noise = Math.max(0, this.noise - RULES.noiseFade);
        this.noise = Math.min(RULES.maxNoise, this.noise + actionNoise);
    }
    hasKey(symbol) { return this.inventory.some(item => item.type === "keycard" && item.symbol === symbol); }
    hasTarget() { return this.inventory.some(item => item.type === "target"); }
}

class Item {
    constructor(name, type, position, symbol, points) {
        this.name = name;
        this.type = type;        // "keycard", "medkit" or "target"
        this.position = position;
        this.symbol = symbol;
        this.points = points;
        this.collected = false;
    }
}

class Door {
    constructor(position, keySymbol) {
        this.position = position;
        this.keySymbol = keySymbol;
        this.keyName = KEY_NAMES[keySymbol];
        this.locked = true;
    }
    letter() { return this.keySymbol.toUpperCase(); }
    tryUnlock(player) {
        if (player.hasKey(this.keySymbol)) {
            this.locked = false;
            return true;
        }
        return false;
    }
}

class Guard extends Character {
    constructor(name, position, facing, visionRange) {
        super(name, position);
        this.facing = facing;
        this.visionRange = visionRange;
    }
    // Tiles in a straight line in front of the guard. Walls and locked doors block the view.
    getVisionTiles(level) {
        const tiles = [];
        let current = this.position;
        for (let step = 1; step <= this.visionRange; step++) {
            current = addPos(current, this.facing);
            if (!level.isInside(current) || level.blocksSight(current)) break;
            tiles.push(current);
        }
        return tiles;
    }
    canSee(target, level) { return this.getVisionTiles(level).some(tile => samePos(tile, target)); }
    isCloseTo(target) { return distance(this.position, target) <= 1; }
    move(level) { /* each kind of guard replaces this */ }
}

// Walks forward, turns around when blocked.
class PatrolGuard extends Guard {
    constructor(position, facing, visionRange) {
        super("Patrol Guard", position, facing, visionRange);
    }
    move(level) {
        let next = addPos(this.position, this.facing);
        if (level.canGuardWalk(next)) {
            this.position = next;
            return;
        }
        this.facing = { row: -this.facing.row, col: -this.facing.col };
        next = addPos(this.position, this.facing);
        if (level.canGuardWalk(next)) this.position = next;
    }
}

// 1 time in 4 it just looks around, otherwise it walks a random open direction.
class RandomGuard extends Guard {
    constructor(position, visionRange) {
        super("Roaming Guard", position, RIGHT, visionRange);
    }
    move(level) {
        if (Math.floor(Math.random() * 4) === 0) {
            this.facing = DIRECTIONS[Math.floor(Math.random() * 4)];
            return;
        }
        const open = DIRECTIONS.filter(dir => level.canGuardWalk(addPos(this.position, dir)));
        if (open.length === 0) return;
        this.facing = open[Math.floor(Math.random() * open.length)];
        this.position = addPos(this.position, this.facing);
    }
}

class Level {
    constructor(number) {
        const data = LEVELS[number - 1];
        this.number = number;
        this.title = data.title;
        this.targetName = data.targetName;
        this.briefing = data.briefing;
        this.spotAlert = data.spotAlert;
        this.hearAlert = data.hearAlert;
        this.catchDamage = data.catchDamage;
        this.startingAlert = data.startingAlert;
        this.items = [];
        this.doors = [];
        this.guards = [];
        this.map = [];

        // Read the map. Special letters become objects and their tile becomes floor.
        for (let r = 0; r < data.map.length; r++) {
            let row = "";
            for (let c = 0; c < data.map[r].length; c++) {
                const tile = data.map[r][c];
                const here = { row: r, col: c };
                let keep = false;
                if (tile === "@") this.playerStart = here;
                else if ("abc".includes(tile)) this.items.push(new Item(KEY_NAMES[tile], "keycard", here, tile, RULES.keycardPoints));
                else if ("ABC".includes(tile)) this.doors.push(new Door(here, tile.toLowerCase()));
                else if (tile === "T") this.items.push(new Item(data.targetName, "target", here, "T", RULES.targetPoints));
                else if (tile === "+") this.items.push(new Item("Medkit", "medkit", here, "+", RULES.medkitPoints));
                else if (tile === "H") this.guards.push(new PatrolGuard(here, RIGHT, data.visionRange));
                else if (tile === "V") this.guards.push(new PatrolGuard(here, DOWN, data.visionRange));
                else if (tile === "R") this.guards.push(new RandomGuard(here, data.visionRange));
                else keep = true;
                row += keep ? tile : ".";
            }
            this.map.push(row);
        }
    }
    isInside(p) { return p.row >= 0 && p.row < this.map.length && p.col >= 0 && p.col < this.map[p.row].length; }
    getTile(p) { return this.isInside(p) ? this.map[p.row][p.col] : "#"; }
    findDoorAt(p) { return this.doors.find(door => samePos(door.position, p)) || null; }
    findItemAt(p) { return this.items.find(item => !item.collected && samePos(item.position, p)) || null; }
    isWalkable(p) {
        if (this.getTile(p) === "#") return false;
        const door = this.findDoorAt(p);
        return !(door && door.locked);
    }
    canGuardWalk(p) { return this.getTile(p) !== "#" && !this.findDoorAt(p); }
    blocksSight(p) { return !this.isWalkable(p); }
    moveAllGuards() { this.guards.forEach(guard => guard.move(this)); }   // polymorphism
    getObjective(player) {
        if (player.hasTarget()) return "Escape! Reach the EXIT.";
        for (const door of this.doors) {
            if (door.locked) {
                if (!player.hasKey(door.keySymbol)) return "Find the " + door.keyName + ".";
                return "Walk into door " + door.letter() + " to unlock it with the " + door.keyName + ".";
            }
        }
        return "Steal the " + this.targetName + ".";
    }
}

// ---------------------------------------------------------
// 4. THE GAME (same turn order as Game.cpp)
// ---------------------------------------------------------
class Game {
    constructor() {
        this.player = new Player();
        this.level = null;
        this.alert = 0;
        this.state = "menu";          // menu, briefing, playing, paused, over
        this.messages = [];
        this.lastInputTime = 0;
    }

    // Start (or continue) a run from a level with given health and score.
    startCampaign(levelNumber, health, score) {
        this.player.restoreStats(health, score);
        this.startLevel(levelNumber);
    }

    startLevel(number) {
        this.level = new Level(number);
        this.player.prepareForLevel(this.level.playerStart);
        this.alert = this.level.startingAlert;
        this.spottedThisLevel = false;
        this.levelComplete = false;
        this.messages = [];
        this.levelStartHealth = this.player.health;
        this.levelStartScore = this.player.score;
        saveCheckpoint(number, this.player.health, this.player.score);   // auto-save

        showScreen("game-screen");
        buildNoiseMeter();
        this.addMessage("Mission started. Keep off the amber tiles the guards can see.");
        this.render();

        this.state = "briefing";
        showModal({
            eyebrow: "MISSION " + number + " OF 3",
            title: this.level.title,
            body: "<p>" + this.level.briefing + "</p>" +
                  "<table><tr><td>Target</td><td>" + this.level.targetName + "</td></tr>" +
                  "<tr><td>Guards on duty</td><td>" + this.level.guards.length + "</td></tr>" +
                  "<tr><td>Starting alert</td><td>" + this.alert + "%</td></tr></table>",
            buttons: [{ text: "Begin", primary: true, action: () => { hideModal(); this.state = "playing"; } }]
        });
    }

    restartLevel() {
        this.player.restoreStats(this.levelStartHealth, this.levelStartScore);
        this.startLevel(this.level.number);
    }

    // One key press = one turn.
    takeTurn(direction, running) {
        let actionNoise = 0;

        if (direction) {
            const steps = running ? 2 : 1;
            let turnUsed = false;
            for (let s = 0; s < steps; s++) {
                const result = this.tryStep(direction);
                if (result === "blocked") break;
                turnUsed = true;
                if (result === "stopped") break;
                if (this.level.getTile(this.player.position) === "~") {
                    actionNoise += RULES.creakNoise;
                    this.addMessage("CREAK! The floor is noisy here.", "warn");
                }
                if (this.levelComplete) break;
            }
            if (!turnUsed) { this.render(); return; }
            actionNoise += running ? RULES.runNoise : RULES.walkNoise;
        }

        this.endTurn(actionNoise);
        this.render();
        this.checkLevelEnd();
    }

    tryStep(direction) {
        const next = addPos(this.player.position, direction);
        if (this.level.getTile(next) === "#") {
            this.addMessage("You bump into a wall.");
            return "blocked";
        }
        const door = this.level.findDoorAt(next);
        if (door && door.locked) {
            if (door.tryUnlock(this.player)) {
                this.addMessage("Door " + door.letter() + " unlocked with the " + door.keyName + ". Walk through!", "good");
                return "stopped";
            }
            this.addMessage("Locked. You need the " + door.keyName + ".", "warn");
            return "blocked";
        }

        this.player.position = next;
        const item = this.level.findItemAt(next);
        if (item) this.collectItem(item);

        if (this.level.getTile(next) === "E") {
            if (this.player.hasTarget()) this.levelComplete = true;
            else this.addMessage("This is the exit, but you don't have the " + this.level.targetName + " yet.", "warn");
        }
        return "moved";
    }

    collectItem(item) {
        item.collected = true;
        this.player.addScore(item.points);
        if (item.type === "medkit") {
            this.player.heal(RULES.medkitHeal);
            this.addMessage("Medkit used: +" + RULES.medkitHeal + " health.", "good");
        } else {
            this.player.inventory.push(item);
            this.addMessage("Picked up the " + item.name + " (+" + item.points + " points).", "good");
            if (item.type === "target") this.addMessage("You have the " + item.name + "! Now get to the EXIT.", "good");
        }
    }

    endTurn(actionNoise) {
        this.player.updateNoise(actionNoise);
        if (this.levelComplete) return;

        if (this.checkCaught()) return;   // walked next to a guard
        this.level.moveAllGuards();
        if (this.checkCaught()) return;   // a guard walked next to you

        let importantEvent = false;
        const pos = this.player.position;

        const seenBy = this.level.guards.find(guard => guard.canSee(pos, this.level));
        if (seenBy) {
            this.alert += this.level.spotAlert;
            this.spottedThisLevel = true;
            this.addMessage("SPOTTED by a " + seenBy.name + "! Alert +" + this.level.spotAlert + "%.", "bad");
            flashMap("flash-warn");
            importantEvent = true;
        }

        if (this.player.noise >= RULES.hearingLimit) {
            const heardBy = this.level.guards.find(guard => distance(guard.position, pos) <= RULES.hearingRadius);
            if (heardBy) {
                this.alert += this.level.hearAlert;
                this.addMessage("A guard HEARD you! Alert +" + this.level.hearAlert + "%.", "warn");
                flashMap("flash-warn");
                importantEvent = true;
            }
        }

        if (!importantEvent) this.alert = Math.max(0, this.alert - RULES.alertDecay);
        this.alert = Math.min(RULES.maxAlert, this.alert);
    }

    checkCaught() {
        const guard = this.level.guards.find(g => g.isCloseTo(this.player.position));
        if (!guard) return false;

        this.player.takeDamage(this.level.catchDamage);
        this.player.loseScore(RULES.caughtPenalty);
        this.alert = Math.min(RULES.maxAlert, this.alert + RULES.caughtAlert);
        this.spottedThisLevel = true;
        this.addMessage("CAUGHT by a " + guard.name + "! -" + this.level.catchDamage + " health, -" + RULES.caughtPenalty + " points.", "bad");
        flashMap("flash-bad");

        if (this.player.isAlive()) {
            this.player.position = this.level.playerStart;
            this.player.noise = 0;
            this.addMessage("You break free and retreat to the entry point.");
        } else {
            this.loseReason = "Your health reached 0. The guards captured you.";
        }
        return true;
    }

    // Decide whether the level was won or lost after a turn.
    checkLevelEnd() {
        if (this.levelComplete) { this.winLevel(); return; }
        if (!this.player.isAlive()) { this.loseLevel(this.loseReason); return; }
        if (this.alert >= RULES.maxAlert) this.loseLevel("The alert reached 100%. The facility is in full lockdown.");
    }

    winLevel() {
        this.state = "over";
        this.player.addScore(RULES.exitBonus);
        const ghost = !this.spottedThisLevel;
        if (ghost) this.player.addScore(RULES.ghostBonus);
        this.render();

        const finishedNumber = this.level.number;
        const rows = "<table>" +
            "<tr><td>Escape bonus</td><td>+" + RULES.exitBonus + "</td></tr>" +
            "<tr><td>Ghost bonus</td><td>" + (ghost ? "+" + RULES.ghostBonus + " (never spotted)" : "none") + "</td></tr>" +
            "<tr><td>Health left</td><td>" + this.player.health + "</td></tr>" +
            "<tr><td>Total score</td><td>" + this.player.score + "</td></tr></table>";

        if (finishedNumber < LEVELS.length) {
            showModal({
                eyebrow: "LEVEL " + finishedNumber + " COMPLETE",
                title: "You escaped with the " + this.level.targetName,
                body: rows,
                buttons: [{ text: "Next mission", primary: true, action: () => { hideModal(); this.startLevel(finishedNumber + 1); } }]
            });
            return;
        }

        // Finished level 3: victory!
        let best = loadBestScore();
        const newBest = this.player.score > best;
        if (newBest) saveBestScore(this.player.score);
        deleteSave();
        showModal({
            eyebrow: "MISSION ACCOMPLISHED",
            title: "The NIGHTFALL Core is yours",
            body: "<p>Obsidian Dynamics never even knew you were there.</p>" + rows +
                  (newBest ? "<p><strong>New best score!</strong></p>" : ""),
            buttons: [{ text: "Main menu", primary: true, action: () => { hideModal(); goToMenu(); } }]
        });
    }

    loseLevel(reason) {
        this.state = "over";
        this.render();
        showModal({
            eyebrow: "MISSION FAILED",
            title: "Game over",
            body: "<p>" + reason + "</p><p>Retry starts this level again with the health and score you had when it began.</p>",
            bad: true,
            buttons: [
                { text: "Retry level", primary: true, action: () => { hideModal(); this.restartLevel(); } },
                { text: "Main menu", action: () => { hideModal(); goToMenu(); } }
            ]
        });
    }

    addMessage(text, type) {
        this.messages.unshift({ text: text, type: type || "" });   // newest first
        if (this.messages.length > 6) this.messages.pop();
    }

    // Draws the map and all the panels.
    render() {
        const level = this.level;
        const mapEl = document.getElementById("map");
        const cols = level.map[0].length;
        fitTileSize(cols, level.map.length);
        mapEl.style.gridTemplateColumns = "repeat(" + cols + ", var(--tile))";

        // Collect every tile a guard can see.
        const vision = new Set();
        level.guards.forEach(guard => {
            guard.getVisionTiles(level).forEach(t => vision.add(t.row + "," + t.col));
        });

        let html = "";
        for (let r = 0; r < level.map.length; r++) {
            for (let c = 0; c < cols; c++) {
                const p = { row: r, col: c };
                const terrain = level.map[r][c];
                const classes = ["tile"];
                let inner = "";

                if (terrain === "#") classes.push("wall");
                else if (terrain === "~") classes.push("creaky");
                else if (terrain === "E") classes.push("exit");
                if (vision.has(r + "," + c)) classes.push("vision");

                const door = level.findDoorAt(p);
                if (door) {
                    classes.push("door", "door-" + door.keySymbol);
                    if (door.locked) inner = door.letter();
                    else classes.push("door-open");
                }

                const item = level.findItemAt(p);
                if (item) inner = itemHtml(item);

                const guard = level.guards.find(g => samePos(g.position, p));
                if (guard) { classes.push("guard", "dir-" + directionName(guard.facing)); inner = ""; }

                if (samePos(this.player.position, p)) { classes.push("player"); inner = ""; }

                html += '<div class="' + classes.join(" ") + '">' + inner + "</div>";
            }
        }
        mapEl.innerHTML = html;

        // Top bar
        document.getElementById("level-label").textContent = "LEVEL " + level.number + " / " + LEVELS.length;
        document.getElementById("level-title").textContent = level.title;
        document.getElementById("objective-text").textContent = level.getObjective(this.player);

        // Health
        const healthBar = document.getElementById("health-bar");
        healthBar.style.width = this.player.health + "%";
        healthBar.classList.toggle("low", this.player.health <= 35);
        document.getElementById("health-value").textContent = this.player.health;

        // Noise
        document.getElementById("noise-value").textContent = this.player.noise;
        document.querySelectorAll("#noise-meter span").forEach((seg, i) => {
            seg.classList.toggle("on", i < this.player.noise);
            seg.classList.toggle("loud", i >= RULES.hearingLimit - 1);
        });

        // Alert
        const alertBar = document.getElementById("alert-bar");
        alertBar.style.width = this.alert + "%";
        alertBar.classList.toggle("danger", this.alert >= 70);
        document.getElementById("alert-value").textContent = this.alert + "%";

        // Score and inventory
        document.getElementById("score-value").textContent = this.player.score;
        const inv = document.getElementById("inventory-list");
        inv.innerHTML = this.player.inventory.length === 0
            ? '<li class="empty">Empty</li>'
            : this.player.inventory.map(item => "<li>" + itemHtml(item) + item.name + "</li>").join("");

        // Mission log
        document.getElementById("log").innerHTML =
            this.messages.map(m => '<li class="' + m.type + '">' + m.text + "</li>").join("");
    }
}

// ---------------------------------------------------------
// 5. SAVE / LOAD (browser storage instead of a text file)
// ---------------------------------------------------------
const SAVE_KEY = "stealthops_save";
const BEST_KEY = "stealthops_best";

function saveCheckpoint(level, health, score) {
    try { localStorage.setItem(SAVE_KEY, JSON.stringify({ level: level, health: health, score: score })); }
    catch (e) { /* storage blocked: the game still works, it just cannot save */ }
}
function loadCheckpoint() {
    try {
        const data = JSON.parse(localStorage.getItem(SAVE_KEY));
        if (data && data.level >= 1 && data.level <= LEVELS.length && data.health >= 1) return data;
    } catch (e) { }
    return null;
}
function deleteSave() { try { localStorage.removeItem(SAVE_KEY); } catch (e) { } }
function loadBestScore() {
    try { return parseInt(localStorage.getItem(BEST_KEY), 10) || 0; } catch (e) { return 0; }
}
function saveBestScore(score) { try { localStorage.setItem(BEST_KEY, String(score)); } catch (e) { } }

// ---------------------------------------------------------
// 6. SCREEN HELPERS
// ---------------------------------------------------------
const game = new Game();

function itemHtml(item) {
    if (item.type === "keycard") return '<span class="key key-' + item.symbol + '"></span>';
    if (item.type === "target") return '<span class="target"></span>';
    return '<span class="medkit"></span>';
}

function directionName(dir) {
    if (dir.row === -1) return "up";
    if (dir.row === 1) return "down";
    if (dir.col === -1) return "left";
    return "right";
}

function showScreen(id) {
    document.querySelectorAll(".screen").forEach(s => s.classList.remove("active"));
    document.getElementById(id).classList.add("active");
}

function buildNoiseMeter() {
    const meter = document.getElementById("noise-meter");
    if (meter.children.length === 0) {
        for (let i = 0; i < RULES.maxNoise; i++) meter.appendChild(document.createElement("span"));
    }
}

// Makes the tiles as big as possible while the whole map still fits.
function fitTileSize(cols, rows) {
    const area = document.querySelector(".map-area");
    const availableWidth = Math.max(240, area.clientWidth - 20);
    const availableHeight = Math.max(240, window.innerHeight - 230);
    const size = Math.floor(Math.min(availableWidth / cols, availableHeight / rows, 38)) - 1;
    document.documentElement.style.setProperty("--tile", Math.max(14, size) + "px");
}

function flashMap(className) {
    const mapEl = document.getElementById("map");
    mapEl.classList.add(className);
    setTimeout(() => mapEl.classList.remove(className), 350);
}

function showModal(options) {
    document.getElementById("modal-eyebrow").textContent = options.eyebrow || "";
    document.getElementById("modal-title").textContent = options.title || "";
    document.getElementById("modal-body").innerHTML = options.body || "";
    document.querySelector(".modal").classList.toggle("bad", !!options.bad);
    const box = document.getElementById("modal-buttons");
    box.innerHTML = "";
    options.buttons.forEach(b => {
        const btn = document.createElement("button");
        btn.className = "btn" + (b.primary ? " btn-primary" : "");
        btn.textContent = b.text;
        btn.addEventListener("click", b.action);
        box.appendChild(btn);
    });
    document.getElementById("overlay").classList.remove("hidden");
    const first = box.querySelector("button");
    if (first) first.focus();
}

function hideModal() { document.getElementById("overlay").classList.add("hidden"); }

function goToMenu() {
    game.state = "menu";
    hideModal();
    showScreen("menu-screen");
    refreshMenu();
}

function refreshMenu() {
    document.getElementById("best-score").textContent = loadBestScore();
    const save = loadCheckpoint();
    document.getElementById("btn-continue").disabled = !save;
    document.getElementById("continue-note").textContent = save
        ? "Saved: Level " + save.level + " - health " + save.health + ", score " + save.score
        : "No saved mission yet. Progress saves at the start of every level.";
}

function instructionsHtml() {
    return "<p>The game is <strong>turn-based</strong>: every key press is one turn, then the guards move.</p>" +
        "<ul>" +
        "<li><kbd>W</kbd><kbd>A</kbd><kbd>S</kbd><kbd>D</kbd> or arrow keys: walk one step (quiet)</li>" +
        "<li><kbd>Shift</kbd> + direction (or the Run box): run two steps (loud)</li>" +
        "<li><kbd>Space</kbd> or <kbd>X</kbd>: wait one turn</li>" +
        "<li><kbd>P</kbd> pause, <kbd>R</kbd> restart level, <kbd>Esc</kbd> menu</li>" +
        "</ul>" +
        "<ul>" +
        "<li>Collect keycards and walk into the matching coloured door to unlock it.</li>" +
        "<li>Guards look straight ahead (the little arrow). Amber tiles = they can see you there: alert rises.</li>" +
        "<li>Standing next to a guard = caught: you lose health and go back to the entry point.</li>" +
        "<li>Running and creaky floors make noise. Noise 5+ near a guard = heard: alert rises.</li>" +
        "<li>Alert 100% = lockdown. Health 0 = captured. Alert drops on quiet turns.</li>" +
        "<li>Score: keycard +100, medkit +25, target +500, escape +300, never spotted +200.</li>" +
        "</ul>";
}

function showInstructions(onClose) {
    showModal({
        eyebrow: "FIELD MANUAL",
        title: "How to play",
        body: instructionsHtml(),
        buttons: [{ text: "Got it", primary: true, action: onClose }]
    });
}

function togglePause() {
    if (game.state === "playing") {
        game.state = "paused";
        showModal({
            eyebrow: "PAUSED",
            title: "Mission on hold",
            body: "<p>The guards are frozen until you resume.</p>",
            buttons: [
                { text: "Resume", primary: true, action: () => { hideModal(); game.state = "playing"; } },
                { text: "How to play", action: () => showInstructions(togglePauseResume) }
            ]
        });
    } else if (game.state === "paused") {
        hideModal();
        game.state = "playing";
    }
}
function togglePauseResume() { game.state = "playing"; togglePause(); }

// ---------------------------------------------------------
// 7. BUTTONS AND KEYBOARD
// ---------------------------------------------------------
document.getElementById("btn-start").addEventListener("click", () => game.startCampaign(1, RULES.maxHealth, 0));
document.getElementById("btn-continue").addEventListener("click", () => {
    const save = loadCheckpoint();
    if (save) game.startCampaign(save.level, save.health, save.score);
});
document.getElementById("btn-instructions").addEventListener("click", () => showInstructions(hideModal));
document.querySelectorAll(".level-card").forEach(card => {
    card.addEventListener("click", () => game.startCampaign(parseInt(card.dataset.level, 10), RULES.maxHealth, 0));
});
document.getElementById("btn-pause").addEventListener("click", togglePause);
document.getElementById("btn-restart").addEventListener("click", () => {
    if (game.state === "playing" || game.state === "paused") { hideModal(); game.restartLevel(); }
});
document.getElementById("btn-menu").addEventListener("click", goToMenu);

const PAD_DIRECTIONS = { up: UP, down: DOWN, left: LEFT, right: RIGHT };
document.querySelectorAll(".pad-btn").forEach(btn => {
    btn.addEventListener("click", () => {
        btn.blur();   // so Space does not "click" this button again
        if (game.state !== "playing") return;
        const running = document.getElementById("run-toggle").checked;
        game.takeTurn(PAD_DIRECTIONS[btn.dataset.key] || null, running);
    });
});

const KEY_DIRECTIONS = {
    w: UP, arrowup: UP, s: DOWN, arrowdown: DOWN,
    a: LEFT, arrowleft: LEFT, d: RIGHT, arrowright: RIGHT
};

document.addEventListener("keydown", event => {
    const key = event.key.toLowerCase();

    if (key === "escape" && game.state !== "menu") { goToMenu(); return; }
    if (key === "p") { togglePause(); return; }
    if (game.state !== "playing") return;
    if (key === "r") { game.restartLevel(); return; }

    // Holding a key down repeats it; allow at most one turn every 110 ms.
    const now = Date.now();
    if (now - game.lastInputTime < 110) return;

    let handled = true;
    if (KEY_DIRECTIONS[key]) {
        const running = event.shiftKey || document.getElementById("run-toggle").checked;
        game.takeTurn(KEY_DIRECTIONS[key], running);
    } else if (key === " " || key === "x") {
        game.takeTurn(null, false);
    } else {
        handled = false;
    }
    if (handled) {
        game.lastInputTime = now;
        event.preventDefault();   // stop arrow keys / space from scrolling the page
    }
});

window.addEventListener("resize", () => { if (game.level && game.state !== "menu") game.render(); });

refreshMenu();
