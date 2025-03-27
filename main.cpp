#include "include/SDL.h"
#include "include/SDL_image.h"
#include "include/SDL_mixer.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <ctime>

const int TILE_SIZE = 45;
const int MAP_WIDTH = 17;
const int MAP_HEIGHT = 15;
const int WINDOW_WIDTH = 1091;
const int WINDOW_HEIGHT = 675;

enum TileType {
    FLOOR = 0,
    BRICK = 1,
    WALL = 2
};

enum PowerUpType {
    EXTRA_BOMB,
    INCREASE_RANGE,
    INCREASE_SPEED,
    EXTRA_LIFE,
    SHIELD,
    BOMB_LAUNCHER,
    INVINCIBILITY
};

enum GameState {
    MENU,
    HOW_TO_PLAY,
    POWER_UPS,
    SELECT_MODE,
    GAME_SETTINGS,
    PLAYING,
    INTRO_SEQUENCE,
    PAUSE, // Add new game state for pause screen
    YOU_WIN, // Add new game state for win screen
    YOU_LOSE // Add new game state for lose screen
};

enum Difficulty {
    EASY,
    NORMAL,
    HARD
};

struct Player {
    SDL_Texture* texture;
    SDL_Rect rect;
    int x, y;
    bool alive;
    int bombCount;
    int explosionRange;
    int speed;
    int lives;
    bool shield;
    bool invincible;
};

struct Bomb {
    SDL_Texture* texture;
    SDL_Rect rect;
    int x, y;
    Uint32 startTime;
    bool exploded;
};

struct Bot : public Player {
    void update(const std::vector<std::vector<int>>& map, const std::vector<Bomb>& bombs, const Player& target) {
        // Simple AI logic to chase the target player and avoid bombs
        if (alive) {
            // Avoid bombs
            for (const auto& bomb : bombs) {
                if (!bomb.exploded && abs(x - bomb.x) <= 1 && abs(y - bomb.y) <= 1) {
                    if (x < bomb.x) x--;
                    else if (x > bomb.x) x++;
                    if (y < bomb.y) y--;
                    else if (y > bomb.y) y++;
                    return;
                }
            }

            // Chase the target player
            if (x < target.x) x++;
            else if (x > target.x) x--;
            if (y < target.y) y++;
            else if (y > target.y) y--;
        }
    }

    bool shouldPlaceBomb(const std::vector<std::vector<int>>& map, const Player& target) {
        // Simple logic to place a bomb if the target is within range
        return alive && abs(x - target.x) <= 1 && abs(y - target.y) <= 1;
    }
};

struct PowerUp {
    PowerUpType type;
    SDL_Texture* texture;
    SDL_Rect rect;
    int x, y;
    bool active;
};

struct GameSettings {
    int players;
    std::string arena;
    Difficulty difficulty;
};

void loadMap(const std::string& filename, std::vector<std::vector<int>>& map) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open map file: " << filename << std::endl;
        return;
    }

    map.resize(MAP_HEIGHT, std::vector<int>(MAP_WIDTH));
    for (int y = 0; y < MAP_HEIGHT; ++y) {
        for (int x = 0; x < MAP_WIDTH; ++x) {
            file >> map[y][x];
        }
    }
}

SDL_Texture* loadTexture(SDL_Renderer* renderer, const std::string& path) {
    SDL_Texture* texture = IMG_LoadTexture(renderer, path.c_str());
    if (!texture) {
        std::cerr << "Failed to load texture: " << path << " " << IMG_GetError() << std::endl;
    }
    return texture;
}

void handlePlayerDeath(Player& player, SDL_Texture* floorTexture) {
    if (player.shield) {
        player.shield = false;
    } else if (player.invincible) {
        player.invincible = false;
    } else {
        player.alive = false;
        player.texture = floorTexture;
    }
}

void drawMap(SDL_Renderer* renderer, const std::vector<std::vector<int>>& map, SDL_Texture* floorTexture, std::vector<PowerUp>& powerUps) {
    SDL_Rect destRect = { 0, 0, TILE_SIZE, TILE_SIZE };
    for (int y = 0; y < MAP_HEIGHT; ++y) {
        for (int x = 0; x < MAP_WIDTH; ++x) {
            if (map[y][x] == FLOOR) {
                destRect.x = 326 + x * TILE_SIZE;
                destRect.y = y * TILE_SIZE;
                SDL_RenderCopy(renderer, floorTexture, nullptr, &destRect);
            }
            // Add more conditions if needed for other tile types
        }
    }

    // Draw power-ups
    for (auto& powerUp : powerUps) {
        if (powerUp.active) {
            SDL_RenderCopy(renderer, powerUp.texture, nullptr, &powerUp.rect);
        }
    }
}

void handleBombExplosion(Bomb& bomb, std::vector<Player>& players, std::vector<Bot>& bots, std::vector<std::vector<int>>& map, std::vector<PowerUp>& powerUps, Mix_Chunk* explosionSound, SDL_Texture* floorTexture) {
    if (SDL_GetTicks() - bomb.startTime >= 3000 && !bomb.exploded) {
        bomb.exploded = true;
        Mix_PlayChannel(-1, explosionSound, 0);

        // Check for player collision with explosion
        for (auto& player : players) {
            if (player.alive && player.x == bomb.x && player.y == bomb.y) {
                handlePlayerDeath(player, floorTexture);
            }
        }

        // Check for bot collision with explosion
        for (auto& bot : bots) {
            if (bot.alive && bot.x == bomb.x && bot.y == bomb.y) {
                handlePlayerDeath(bot, floorTexture);
            }
        }

        // Destroy bricks and possibly spawn power-ups
        if (map[bomb.y][bomb.x] == BRICK) {
            map[bomb.y][bomb.x] = FLOOR;
            if (rand() % 2 == 0) { // 50% chance to spawn a power-up
                PowerUp powerUp;
                powerUp.type = static_cast<PowerUpType>(rand() % 7);
                powerUp.texture = floorTexture; // Replace with actual power-up texture
                powerUp.rect = { 326 + bomb.x * TILE_SIZE, bomb.y * TILE_SIZE, TILE_SIZE, TILE_SIZE };
                powerUp.x = bomb.x;
                powerUp.y = bomb.y;
                powerUp.active = true;
                powerUps.push_back(powerUp);
            }
        }
    }
}

void applyPowerUp(Player& player, PowerUp& powerUp) {
    switch (powerUp.type) {
        case EXTRA_BOMB:
            player.bombCount++;
            break;
        case INCREASE_RANGE:
            player.explosionRange++;
            break;
        case INCREASE_SPEED:
            player.speed++;
            break;
        case EXTRA_LIFE:
            player.lives++;
            break;
        case SHIELD:
            player.shield = true;
            break;
        case BOMB_LAUNCHER:
            // Implement bomb launcher functionality
            break;
        case INVINCIBILITY:
            player.invincible = true;
            break;
    }
    powerUp.active = false;
}

void drawMenu(SDL_Renderer* renderer, SDL_Texture* menuTexture, SDL_Texture* startButtonTexture, SDL_Texture* howToPlayButtonTexture, bool startHover, bool howToPlayHover) {
    SDL_RenderCopy(renderer, menuTexture, nullptr, nullptr);

    SDL_Rect startButtonRect = { 400, 200, 300, 100 };
    if (startHover) {
        SDL_SetTextureColorMod(startButtonTexture, 200, 200, 200); // Lighten texture on hover
    } else {
        SDL_SetTextureColorMod(startButtonTexture, 255, 255, 255); // Normal texture
    }
    SDL_RenderCopy(renderer, startButtonTexture, nullptr, &startButtonRect);

    SDL_Rect howToPlayButtonRect = { 400, 350, 300, 100 };
    if (howToPlayHover) {
        SDL_SetTextureColorMod(howToPlayButtonTexture, 200, 200, 200); // Lighten texture on hover
    } else {
        SDL_SetTextureColorMod(howToPlayButtonTexture, 255, 255, 255); // Normal texture
    }
    SDL_RenderCopy(renderer, howToPlayButtonTexture, nullptr, &howToPlayButtonRect);
}

void drawHowToPlay(SDL_Renderer* renderer, SDL_Texture* howToPlayTexture, SDL_Texture* backButtonTexture, SDL_Texture* powerUpsButtonTexture, bool backHover, bool powerUpsHover) {
    SDL_RenderCopy(renderer, howToPlayTexture, nullptr, nullptr);

    SDL_Rect backButtonRect = { 50, 50, 200, 100 };
    if (backHover) {
        SDL_SetTextureColorMod(backButtonTexture, 200, 200, 200); // Lighten texture on hover
    } else {
        SDL_SetTextureColorMod(backButtonTexture, 255, 255, 255); // Normal texture
    }
    SDL_RenderCopy(renderer, backButtonTexture, nullptr, &backButtonRect);

    SDL_Rect powerUpsButtonRect = { 400, 500, 300, 100 };
    if (powerUpsHover) {
        SDL_SetTextureColorMod(powerUpsButtonTexture, 200, 200, 200); // Lighten texture on hover
    } else {
        SDL_SetTextureColorMod(powerUpsButtonTexture, 255, 255, 255); // Normal texture
    }
    SDL_RenderCopy(renderer, powerUpsButtonTexture, nullptr, &powerUpsButtonRect);
}

void drawPowerUps(SDL_Renderer* renderer, SDL_Texture* powerUpsTexture, SDL_Texture* backButtonTexture, SDL_Texture* controlsButtonTexture, bool backHover, bool controlsHover) {
    SDL_RenderCopy(renderer, powerUpsTexture, nullptr, nullptr);

    SDL_Rect backButtonRect = { 50, 50, 200, 100 };
    if (backHover) {
        SDL_SetTextureColorMod(backButtonTexture, 200, 200, 200); // Lighten texture on hover
    } else {
        SDL_SetTextureColorMod(backButtonTexture, 255, 255, 255); // Normal texture
    }
    SDL_RenderCopy(renderer, backButtonTexture, nullptr, &backButtonRect);

    SDL_Rect controlsButtonRect = { 400, 500, 300, 100 };
    if (controlsHover) {
        SDL_SetTextureColorMod(controlsButtonTexture, 200, 200, 200); // Lighten texture on hover
    } else {
        SDL_SetTextureColorMod(controlsButtonTexture, 255, 255, 255); // Normal texture
    }
    SDL_RenderCopy(renderer, controlsButtonTexture, nullptr, &controlsButtonRect);
}

void drawSelectMode(SDL_Renderer* renderer, SDL_Texture* selectModeTexture, SDL_Texture* backButtonTexture, SDL_Texture* nextButtonTexture, bool backHover, bool nextHover) {
    SDL_RenderCopy(renderer, selectModeTexture, nullptr, nullptr);

    SDL_Rect backButtonRect = { 50, 50, 200, 100 };
    if (backHover) {
        SDL_SetTextureColorMod(backButtonTexture, 200, 200, 200); // Lighten texture on hover
    } else {
        SDL_SetTextureColorMod(backButtonTexture, 255, 255, 255); // Normal texture
    }
    SDL_RenderCopy(renderer, backButtonTexture, nullptr, &backButtonRect);

    SDL_Rect nextButtonRect = { 800, 500, 200, 100 };
    if (nextHover) {
        SDL_SetTextureColorMod(nextButtonTexture, 200, 200, 200); // Lighten texture on hover
    } else {
        SDL_SetTextureColorMod(nextButtonTexture, 255, 255, 255); // Normal texture
    }
    SDL_RenderCopy(renderer, nextButtonTexture, nullptr, &nextButtonRect);
}

void drawGameSettings(SDL_Renderer* renderer, SDL_Texture* gameSettingsTexture, SDL_Texture* backButtonTexture, SDL_Texture* startButtonTexture, bool backHover, bool startHover, const GameSettings& settings, SDL_Texture* player1Textures[3], SDL_Texture* player2Textures[3], SDL_Texture* christmasTextures[3], SDL_Texture* forestTextures[3], SDL_Texture* easyTextures[3], SDL_Texture* normalTextures[3], SDL_Texture* hardTextures[3], int selectedPlayer, int selectedArena, int selectedDifficulty) {
    SDL_RenderCopy(renderer, gameSettingsTexture, nullptr, nullptr);

    SDL_Rect backButtonRect = { 50, 50, 200, 100 };
    if (backHover) {
        SDL_SetTextureColorMod(backButtonTexture, 200, 200, 200); // Lighten texture on hover
    } else {
        SDL_SetTextureColorMod(backButtonTexture, 255, 255, 255); // Normal texture
    }
    SDL_RenderCopy(renderer, backButtonTexture, nullptr, &backButtonRect);

    SDL_Rect startButtonRect = { 800, 500, 200, 100 };
    if (startHover) {
        SDL_SetTextureColorMod(startButtonTexture, 200, 200, 200); // Lighten texture on hover
    } else {
        SDL_SetTextureColorMod(startButtonTexture, 255, 255, 255); // Normal texture
    }
    SDL_RenderCopy(renderer, startButtonTexture, nullptr, &startButtonRect);

    // Draw player buttons
    SDL_Rect player1Rect = { 100, 200, 100, 100 };
    SDL_Rect player2Rect = { 250, 200, 100, 100 };
    SDL_RenderCopy(renderer, player1Textures[selectedPlayer == 1 ? 2 : 0], nullptr, &player1Rect);
    SDL_RenderCopy(renderer, player2Textures[selectedPlayer == 2 ? 2 : 0], nullptr, &player2Rect);

    // Draw arena buttons
    SDL_Rect christmasRect = { 100, 350, 100, 100 };
    SDL_Rect forestRect = { 250, 350, 100, 100 };
    SDL_RenderCopy(renderer, christmasTextures[selectedArena == 1 ? 2 : 0], nullptr, &christmasRect);
    SDL_RenderCopy(renderer, forestTextures[selectedArena == 2 ? 2 : 0], nullptr, &forestRect);

    // Draw difficulty buttons
    SDL_Rect easyRect = { 100, 500, 100, 100 };
    SDL_Rect normalRect = { 250, 500, 100, 100 };
    SDL_Rect hardRect = { 400, 500, 100, 100 };
    SDL_RenderCopy(renderer, easyTextures[selectedDifficulty == EASY ? 2 : 0], nullptr, &easyRect);
    SDL_RenderCopy(renderer, normalTextures[selectedDifficulty == NORMAL ? 2 : 0], nullptr, &normalRect);
    SDL_RenderCopy(renderer, hardTextures[selectedDifficulty == HARD ? 2 : 0], nullptr, &hardRect);
}

void playIntroSequence(SDL_Renderer* renderer, SDL_Texture* introTextures[3]) {
    for (int i = 0; i < 3; ++i) {
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, introTextures[i], nullptr, nullptr);
        SDL_RenderPresent(renderer);
        SDL_Delay(2000); // Display each image for 2 seconds
    }
}

void updateGameSettings(GameSettings& settings, std::vector<Bot>& bots) {
    if (settings.players == 1) {
        bots.resize(3); // 1 player mode includes 3 AI bots
    } else if (settings.players == 2) {
        bots.resize(2); // 2 player mode includes 2 AI bots
    }

    // Set difficulty for each bot
    for (auto& bot : bots) {
        switch (settings.difficulty) {
            case EASY:
                bot.speed = 1;
                break;
            case NORMAL:
                bot.speed = 2;
                break;
            case HARD:
                bot.speed = 3;
                break;
        }
    }
}

void drawPauseScreen(SDL_Renderer* renderer, SDL_Texture* pauseTexture, SDL_Texture* mainMenuButtonTexture, SDL_Texture* resumeButtonTexture, bool mainMenuHover, bool resumeHover) {
    SDL_RenderCopy(renderer, pauseTexture, nullptr, nullptr);

    SDL_Rect mainMenuButtonRect = { 400, 200, 300, 100 };
    if (mainMenuHover) {
        SDL_SetTextureColorMod(mainMenuButtonTexture, 200, 200, 200); // Lighten texture on hover
    } else {
        SDL_SetTextureColorMod(mainMenuButtonTexture, 255, 255, 255); // Normal texture
    }
    SDL_RenderCopy(renderer, mainMenuButtonTexture, nullptr, &mainMenuButtonRect);

    SDL_Rect resumeButtonRect = { 400, 350, 300, 100 };
    if (resumeHover) {
        SDL_SetTextureColorMod(resumeButtonTexture, 200, 200, 200); // Lighten texture on hover
    } else {
        SDL_SetTextureColorMod(resumeButtonTexture, 255, 255, 255); // Normal texture
    }
    SDL_RenderCopy(renderer, resumeButtonTexture, nullptr, &resumeButtonRect);
}

void drawWinScreen(SDL_Renderer* renderer, SDL_Texture* winTexture) {
    SDL_RenderCopy(renderer, winTexture, nullptr, nullptr);
}

void drawLoseScreen(SDL_Renderer* renderer, SDL_Texture* loseTexture) {
    SDL_RenderCopy(renderer, loseTexture, nullptr, nullptr);
}

Mix_Chunk* clickSound = nullptr;

Mix_Music* menuMusic = nullptr;
Mix_Music* gameMusic = nullptr;
Mix_Music* winMusic = nullptr;
Mix_Music* loseMusic = nullptr;

int main(int argc, char* argv[]) {
    srand(static_cast<unsigned int>(time(0)));

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return -1;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cerr << "Failed to initialize SDL_image: " << IMG_GetError() << std::endl;
        return -1;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "Failed to initialize SDL_mixer: " << Mix_GetError() << std::endl;
        return -1;
    }

    SDL_Window* window = SDL_CreateWindow("Bomb It", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
        return -1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Failed to create renderer: " << SDL_GetError() << std::endl;
        return -1;
    }

    // Load textures
    SDL_Texture* leftPanelTexture = loadTexture(renderer, "assets/image/screens/left_panel.png");
    SDL_Texture* christmasFloorTexture = loadTexture(renderer, "assets/image/christmas_floor.png");
    SDL_Texture* forestFloorTexture = loadTexture(renderer, "assets/image/forest_floor.png");
    SDL_Texture* playerTextures[4] = {
        loadTexture(renderer, "assets/image/players/player1.png"),
        loadTexture(renderer, "assets/image/players/player2.png"),
        loadTexture(renderer, "assets/image/players/player3.png"),
        loadTexture(renderer, "assets/image/players/player4.png")
    };
    SDL_Texture* bombTexture = loadTexture(renderer, "assets/image/bomb.png");
    SDL_Texture* explosionTexture = loadTexture(renderer, "assets/image/explosion.png");

    // Load menu textures
    SDL_Texture* menuTexture = loadTexture(renderer, "assets/image/screens/menu.png");
    SDL_Texture* startButtonTexture = loadTexture(renderer, "assets/image/buttons/start_button.png");
    SDL_Texture* howToPlayButtonTexture = loadTexture(renderer, "assets/image/buttons/how_to_play_button.png");
    SDL_Texture* howToPlayTexture = loadTexture(renderer, "assets/image/screens/how_to_play.png");
    SDL_Texture* backButtonTexture = loadTexture(renderer, "assets/image/buttons/back_button.png");
    SDL_Texture* powerUpsButtonTexture = loadTexture(renderer, "assets/image/buttons/power_ups_button.png");
    SDL_Texture* powerUpsTexture = loadTexture(renderer, "assets/image/screens/power_ups.png");
    SDL_Texture* controlsButtonTexture = loadTexture(renderer, "assets/image/buttons/controls_button.png");
    SDL_Texture* selectModeTexture = loadTexture(renderer, "assets/image/screens/select_mode.png");
    SDL_Texture* nextButtonTexture = loadTexture(renderer, "assets/image/buttons/next_button.png");
    SDL_Texture* gameSettingsTexture = loadTexture(renderer, "assets/image/screens/game_settings.png");

    // Load intro textures
    SDL_Texture* introTextures[3] = {
        loadTexture(renderer, "assets/image/intro/intro1.png"),
        loadTexture(renderer, "assets/image/intro/intro2.png"),
        loadTexture(renderer, "assets/image/intro/intro3.png")
    };

    // Load player button textures
    SDL_Texture* player1Textures[3] = {
        loadTexture(renderer, "assets/image/player1_normal.png"),
        loadTexture(renderer, "assets/image/player1_hover.png"),
        loadTexture(renderer, "assets/image/player1_click.png")
    };
    SDL_Texture* player2Textures[3] = {
        loadTexture(renderer, "assets/image/player2_normal.png"),
        loadTexture(renderer, "assets/image/player2_hover.png"),
        loadTexture(renderer, "assets/image/player2_click.png")
    };

    // Load arena button textures
    SDL_Texture* christmasTextures[3] = {
        loadTexture(renderer, "assets/image/christmas_normal.png"),
        loadTexture(renderer, "assets/image/christmas_hover.png"),
        loadTexture(renderer, "assets/image/christmas_click.png")
    };
    SDL_Texture* forestTextures[3] = {
        loadTexture(renderer, "assets/image/forest_normal.png"),
        loadTexture(renderer, "assets/image/forest_hover.png"),
        loadTexture(renderer, "assets/image/forest_click.png")
    };

    // Load difficulty button textures
    SDL_Texture* easyTextures[3] = {
        loadTexture(renderer, "assets/image/easy_normal.png"),
        loadTexture(renderer, "assets/image/easy_hover.png"),
        loadTexture(renderer, "assets/image/easy_click.png")
    };
    SDL_Texture* normalTextures[3] = {
        loadTexture(renderer, "assets/image/normal_normal.png"),
        loadTexture(renderer, "assets/image/normal_hover.png"),
        loadTexture(renderer, "assets/image/normal_click.png")
    };
    SDL_Texture* hardTextures[3] = {
        loadTexture(renderer, "assets/image/hard_normal.png"),
        loadTexture(renderer, "assets/image/hard_hover.png"),
        loadTexture(renderer, "assets/image/hard_click.png")
    };

    // Load sound
    Mix_Chunk* explosionSound = Mix_LoadWAV("assets/sound/explosion.mp3");
    if (!explosionSound) {
        std::cerr << "Failed to load sound: " << Mix_GetError() << std::endl;
        return -1;
    }

    // Load click sound
    clickSound = Mix_LoadWAV("assets/sound/click.mp3");
    if (!clickSound) {
        std::cerr << "Failed to load click sound: " << Mix_GetError() << std::endl;
        return -1;
    }

    // Load background music
    menuMusic = Mix_LoadMUS("assets/sound/menu_music.mp3");
    if (!menuMusic) {
        std::cerr << "Failed to load menu music: " << Mix_GetError() << std::endl;
        return -1;
    }

    gameMusic = Mix_LoadMUS("assets/sound/game_music.mp3");
    if (!gameMusic) {
        std::cerr << "Failed to load game music: " << Mix_GetError() << std::endl;
        return -1;
    }

    winMusic = Mix_LoadMUS("assets/sound/win_music.mp3");
    if (!winMusic) {
        std::cerr << "Failed to load win music: " << Mix_GetError() << std::endl;
        return -1;
    }

    loseMusic = Mix_LoadMUS("assets/sound/lose_music.mp3");
    if (!loseMusic) {
        std::cerr << "Failed to load lose music: " << Mix_GetError() << std::endl;
        return -1;
    }

    // Play menu music initially
    Mix_PlayMusic(menuMusic, -1);

    // Load maps
    std::vector<std::vector<int>> christmasMap, forestMap;
    loadMap("christmas_map.txt", christmasMap);
    loadMap("forest_map.txt", forestMap);

    // Initialize players and bots
    std::vector<Player> players(2);
    std::vector<Bot> bots;
    for (int i = 0; i < 2; ++i) {
        players[i].texture = playerTextures[i];
        players[i].rect = { 0, 0, TILE_SIZE, TILE_SIZE };
        players[i].alive = true;
        players[i].bombCount = 1;
        players[i].explosionRange = 1;
        players[i].speed = 1;
        players[i].lives = 1;
        players[i].shield = false;
        players[i].invincible = false;
    }

    GameSettings settings = {1, "christmas", EASY};

    // Update game settings based on selected mode
    updateGameSettings(settings, bots);

    // Set initial positions for players and bots
    players[0].x = 0; players[0].y = 0;
    players[1].x = MAP_WIDTH - 1; players[1].y = 0;
    for (int i = 0; i < bots.size(); ++i) {
        bots[i].texture = playerTextures[i + 2];
        bots[i].rect = { 0, 0, TILE_SIZE, TILE_SIZE };
        bots[i].alive = true;
        bots[i].bombCount = 1;
        bots[i].explosionRange = 1;
        bots[i].speed = 1;
        bots[i].lives = 1;
        bots[i].shield = false;
        bots[i].invincible = false;
        bots[i].x = (i % 2 == 0) ? 0 : MAP_WIDTH - 1;
        bots[i].y = MAP_HEIGHT - 1;
    }

    // Choose map to display (for example, CHRISTMAS map)
    std::vector<std::vector<int>>* currentMap = &christmasMap;
    SDL_Texture* currentFloorTexture = christmasFloorTexture;

    std::vector<Bomb> bombs;
    std::vector<PowerUp> powerUps;

    int selectedPlayer = 1;
    int selectedArena = 1;
    int selectedDifficulty = EASY;

    SDL_Texture* pauseTexture = loadTexture(renderer, "pause.png");
    SDL_Texture* mainMenuButtonTexture = loadTexture(renderer, "assets/image/buttons/main_menu_button.png");
    SDL_Texture* resumeButtonTexture = loadTexture(renderer, "assets/image/buttons/resume_button.png");

    SDL_Texture* winTexture = loadTexture(renderer, "you_win.png");
    SDL_Texture* loseTexture = loadTexture(renderer, "you_lose.png");

    GameState gameState = MENU;

    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }

            if (gameState == MENU) {
                int x, y;
                SDL_GetMouseState(&x, &y);
                bool startHover = (x >= 400 && x <= 700 && y >= 200 && y <= 300);
                bool howToPlayHover = (x >= 400 && x <= 700 && y >= 350 && y <= 450);

                if (event.type == SDL_MOUSEBUTTONDOWN) {
                    Mix_PlayChannel(-1, clickSound, 0); // Play click sound
                    if (startHover) {
                        gameState = INTRO_SEQUENCE;
                    } else if (howToPlayHover) {
                        gameState = HOW_TO_PLAY;
                    }
                }

                drawMenu(renderer, menuTexture, startButtonTexture, howToPlayButtonTexture, startHover, howToPlayHover);
            } else if (gameState == INTRO_SEQUENCE) {
                playIntroSequence(renderer, introTextures);
                gameState = SELECT_MODE;
            } else if (gameState == HOW_TO_PLAY) {
                int x, y;
                SDL_GetMouseState(&x, &y);
                bool backHover = (x >= 50 && x <= 250 && y >= 50 && y <= 150);
                bool powerUpsHover = (x >= 400 && x <= 700 && y >= 500 && y <= 600);

                if (event.type == SDL_MOUSEBUTTONDOWN) {
                    Mix_PlayChannel(-1, clickSound, 0); // Play click sound
                    if (backHover) {
                        gameState = MENU;
                    } else if (powerUpsHover) {
                        gameState = POWER_UPS;
                    }
                }

                drawHowToPlay(renderer, howToPlayTexture, backButtonTexture, powerUpsButtonTexture, backHover, powerUpsHover);
            } else if (gameState == POWER_UPS) {
                int x, y;
                SDL_GetMouseState(&x, &y);
                bool backHover = (x >= 50 && x <= 250 && y >= 50 && y <= 150);
                bool controlsHover = (x >= 400 && x <= 700 && y >= 500 && y <= 600);

                if (event.type == SDL_MOUSEBUTTONDOWN) {
                    Mix_PlayChannel(-1, clickSound, 0); // Play click sound
                    if (backHover) {
                        gameState = MENU;
                    } else if (controlsHover) {
                        gameState = HOW_TO_PLAY;
                    }
                }

                drawPowerUps(renderer, powerUpsTexture, backButtonTexture, controlsButtonTexture, backHover, controlsHover);
            } else if (gameState == SELECT_MODE) {
                int x, y;
                SDL_GetMouseState(&x, &y);
                bool backHover = (x >= 50 && x <= 250 && y >= 50 && y <= 150);
                bool nextHover = (x >= 800 && x <= 1000 && y >= 500 && y <= 600);

                if (event.type == SDL_MOUSEBUTTONDOWN) {
                    Mix_PlayChannel(-1, clickSound, 0); // Play click sound
                    if (backHover) {
                        gameState = MENU;
                    } else if (nextHover) {
                        gameState = GAME_SETTINGS;
                    }
                }

                drawSelectMode(renderer, selectModeTexture, backButtonTexture, nextButtonTexture, backHover, nextHover);
            } else if (gameState == GAME_SETTINGS) {
                int x, y;
                SDL_GetMouseState(&x, &y);
                bool backHover = (x >= 50 && x <= 250 && y >= 50 && y <= 150);
                bool startHover = (x >= 800 && x <= 1000 && y >= 500 && y <= 600);

                if (event.type == SDL_MOUSEBUTTONDOWN) {
                    Mix_PlayChannel(-1, clickSound, 0); // Play click sound
                    if (backHover) {
                        gameState = SELECT_MODE;
                    } else if (startHover) {
                        gameState = PLAYING;
                    }
                }

                drawGameSettings(renderer, gameSettingsTexture, backButtonTexture, startButtonTexture, backHover, startHover, settings, player1Textures, player2Textures, christmasTextures, forestTextures, easyTextures, normalTextures, hardTextures, selectedPlayer, selectedArena, selectedDifficulty);
            } else if (gameState == PLAYING) {
                // Switch to game music if not already playing
                if (Mix_PlayingMusic() == 0 || Mix_GetMusicType(NULL) != MUS_MP3) {
                    Mix_HaltMusic();
                    Mix_PlayMusic(gameMusic, -1);
                }

                if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_p) {
                    Mix_PlayChannel(-1, clickSound, 0); // Play click sound
                    gameState = PAUSE; // Switch to pause screen when 'P' is pressed
                }

                // Check game conditions to determine the winner
                int alivePlayers = 0;
                int aliveBots = 0;
                for (const auto& player : players) {
                    if (player.alive) alivePlayers++;
                }
                for (const auto& bot : bots) {
                    if (bot.alive) aliveBots++;
                }

                if (alivePlayers + aliveBots == 1) {
                    if (alivePlayers == 1) {
                        gameState = YOU_WIN; // Player wins
                    } else {
                        gameState = YOU_LOSE; // Bot wins
                    }
                }

                drawMap(renderer, *currentMap, currentFloorTexture, powerUps);

                // Draw bombs
                for (auto& bomb : bombs) {
                    if (!bomb.exploded) {
                        SDL_RenderCopy(renderer, bomb.texture, nullptr, &bomb.rect);
                    } else {
                        SDL_Rect explosionRect = { 326 + bomb.x * TILE_SIZE, bomb.y * TILE_SIZE, TILE_SIZE, TILE_SIZE };
                        SDL_RenderCopy(renderer, explosionTexture, nullptr, &explosionRect);
                    }
                }

                // Draw players
                for (auto& player : players) {
                    if (player.alive) {
                        player.rect.x = 326 + player.x * TILE_SIZE;
                        player.rect.y = player.y * TILE_SIZE;
                        SDL_RenderCopy(renderer, player.texture, nullptr, &player.rect);
                    } else {
                        handlePlayerDeath(player, currentFloorTexture);
                    }
                }

                // Draw bots
                for (auto& bot : bots) {
                    if (bot.alive) {
                        bot.rect.x = 326 + bot.x * TILE_SIZE;
                        bot.rect.y = bot.y * TILE_SIZE;
                        SDL_RenderCopy(renderer, bot.texture, nullptr, &bot.rect);
                    } else {
                        handlePlayerDeath(bot, currentFloorTexture);
                    }
                }

                // Handle bomb explosions
                for (auto& bomb : bombs) {
                    handleBombExplosion(bomb, players, bots, *currentMap, powerUps, explosionSound, currentFloorTexture);
                }

                SDL_RenderPresent(renderer);
            } else if (gameState == PAUSE) {
                int x, y;
                SDL_GetMouseState(&x, &y);
                bool mainMenuHover = (x >= 400 && x <= 700 && y >= 200 && y <= 300);
                bool resumeHover = (x >= 400 && x <= 700 && y >= 350 && y <= 450);

                if (event.type == SDL_MOUSEBUTTONDOWN) {
                    Mix_PlayChannel(-1, clickSound, 0); // Play click sound
                    if (mainMenuHover) {
                        gameState = MENU; // Go back to main menu
                    } else if (resumeHover) {
                        gameState = PLAYING; // Resume the game
                    }
                }

                drawPauseScreen(renderer, pauseTexture, mainMenuButtonTexture, resumeButtonTexture, mainMenuHover, resumeHover);
            } else if (gameState == YOU_WIN || gameState == YOU_LOSE) {
                // Switch back to menu music if not already playing
                if (Mix_PlayingMusic() == 0 || Mix_GetMusicType(NULL) != MUS_MP3) {
                    Mix_HaltMusic();
                    Mix_PlayMusic(menuMusic, -1);
                }

                if (event.type == SDL_MOUSEBUTTONDOWN) {
                    Mix_PlayChannel(-1, clickSound, 0); // Play click sound
                    gameState = MENU; // Go back to main menu on click
                }

                if (gameState == YOU_WIN) {
                    // Switch to win music if not already playing
                    if (Mix_PlayingMusic() == 0 || Mix_GetMusicType(NULL) != MUS_MP3) {
                        Mix_HaltMusic();
                        Mix_PlayMusic(winMusic, -1);
                    }
                    drawWinScreen(renderer, winTexture);
                } else {
                    // Switch to lose music if not already playing
                    if (Mix_PlayingMusic() == 0 || Mix_GetMusicType(NULL) != MUS_MP3) {
                        Mix_HaltMusic();
                        Mix_PlayMusic(loseMusic, -1);
                    }
                    drawLoseScreen(renderer, loseTexture);
                }
            }
        }

        // Ensure this block is properly closed
        // ...existing code...
    }

    // Cleanup
    for (int i = 0; i < 4; ++i) {
        SDL_DestroyTexture(playerTextures[i]);
    }
    SDL_DestroyTexture(leftPanelTexture);
    SDL_DestroyTexture(christmasFloorTexture);
    SDL_DestroyTexture(forestFloorTexture);
    SDL_DestroyTexture(bombTexture);
    SDL_DestroyTexture(explosionTexture);
    SDL_DestroyTexture(menuTexture);
    SDL_DestroyTexture(startButtonTexture);
    SDL_DestroyTexture(howToPlayButtonTexture);
    SDL_DestroyTexture(howToPlayTexture);
    SDL_DestroyTexture(backButtonTexture);
    SDL_DestroyTexture(powerUpsButtonTexture);
    SDL_DestroyTexture(powerUpsTexture);
    SDL_DestroyTexture(controlsButtonTexture);
    SDL_DestroyTexture(selectModeTexture);
    SDL_DestroyTexture(nextButtonTexture);
    for (int i = 0; i < 3; ++i) {
        SDL_DestroyTexture(introTextures[i]);
    }
    Mix_FreeChunk(explosionSound);
    Mix_FreeChunk(clickSound);
    Mix_FreeMusic(menuMusic);
    Mix_FreeMusic(gameMusic);
    Mix_FreeMusic(winMusic);
    Mix_FreeMusic(loseMusic);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    Mix_CloseAudio();
    IMG_Quit();
    SDL_Quit();

    return 0; // Ensure the function ends with a return statement
} // Ensure this closing brace matches the opening brace of main
