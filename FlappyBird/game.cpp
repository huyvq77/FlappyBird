#include "game.h"
#include "texture_manager.h"
#include <SDL_image.h>
#include <iostream>
#include <fstream>
#include <cstdlib>
using namespace std;

int score = 0;
int highScore = 0;
int lastPassedPipe = -1;
float birdY = 350;
float velocityY = 0;
bool gameOver = false;
bool birdFalling = false;
SDL_Texture* numberTextures[10];
vector<PipePair> pipes;
Button tryAgainButton;
Button exitButton;

bool checkCollision(int birdX, int birdY, int birdWidth, int birdHeight,
                    int pipeX, int pipeY, int pipeWidth, int pipeHeight) {
    if (birdX + birdWidth < pipeX || birdX > pipeX + pipeWidth ||
            birdY + birdHeight < pipeY || birdY > pipeY + pipeHeight) {
        return false;
    }
    return true;
}

bool init(SDL_Window*& window, SDL_Renderer*& renderer) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        cout << "SDL Init Error: " << SDL_GetError() << endl;
        return false;
    }
    window = SDL_CreateWindow("Flappy Bird",
                              SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        cout << "Window Error: " << SDL_GetError() << endl;
        return false;
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        cout << "Renderer Error: " << SDL_GetError() << endl;
        return false;
    }
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        cout << "SDL_image Error: " << IMG_GetError() << endl;
        return false;
    }
    srand(SDL_GetTicks());
    return true;
}

void loadNumberTextures(SDL_Renderer* renderer) {
    for (int i = 0; i < 10; ++i) {
        string path = "assets/" + to_string(i) + ".png";
        numberTextures[i] = loadTexture(path, renderer);
    }
}

void initButtons(SDL_Renderer* renderer) {
    tryAgainButton.normalTexture = loadTexture("assets/replay.png", renderer);
    tryAgainButton.hoverTexture = loadTexture("assets/replay_2.png", renderer);
    exitButton.normalTexture = loadTexture("assets/exit.png", renderer);
    exitButton.hoverTexture = loadTexture("assets/exit_2.png", renderer);

    int buttonY = SCREEN_HEIGHT / 2 + 50;
    int spacing = 30;
    int totalWidth = 2 * 120 + spacing;
    int startX = (SCREEN_WIDTH - totalWidth) / 2;

    tryAgainButton.rect = { startX, buttonY, 120, 50 };
    exitButton.rect = { startX + 150, buttonY, 120, 50 };
}

void loadHighScore() {
    ifstream file("highscore.txt");
    if (file.is_open()) file >> highScore;
}

void saveHighScore() {
    ofstream file("highscore.txt");
    if (file.is_open()) file << highScore;
}

void renderScore(SDL_Renderer* renderer) {
    string s = to_string(score);
    int digitW = 24, digitH = 36, totalW = digitW * s.size();
    int startX = (SCREEN_WIDTH - totalW) / 2;
    for (size_t i = 0; i < s.size(); ++i) {
        int digit = s[i] - '0';
        SDL_Rect dest = { startX + static_cast<int>(i) * digitW, 50, digitW, digitH };
        SDL_RenderCopy(renderer, numberTextures[digit], NULL, &dest);
    }
}

void renderHighScore(SDL_Renderer* renderer) {
    string s = to_string(highScore);
    int digitW = 24, digitH = 36, totalW = digitW * s.size();
    int startX = (SCREEN_WIDTH - totalW) / 2;
    int y = SCREEN_HEIGHT / 2 + 270;
    for (size_t i = 0; i < s.size(); ++i) {
        int digit = s[i] - '0';
        SDL_Rect dest = { startX + static_cast<int>(i) * digitW, y, digitW, digitH };
        SDL_RenderCopy(renderer, numberTextures[digit], NULL, &dest);
    }
}

void renderNumber(SDL_Renderer* renderer, int number, int x, int y) {
    string s = to_string(number);
    int digitW = 24, digitH = 36, totalW = digitW * s.size();
    int startX = x - totalW / 2;
    for (size_t i = 0; i < s.size(); ++i) {
        int digit = s[i] - '0';
        SDL_Rect dest = { startX + static_cast<int>(i) * digitW, y, digitW, digitH };
        SDL_RenderCopy(renderer, numberTextures[digit], NULL, &dest);
    }
}

void resetGame(int baseY) {
    gameOver = false;
    birdFalling = false;
    birdY = 350;
    velocityY = 0;
    score = 0;
    lastPassedPipe = -1;
    pipes.clear();
    for (int i = 0; i < 3; ++i) {
        PipePair p;
        p.x = SCREEN_WIDTH + i * pipeDistance;
        p.gapY = rand() % (baseY - GAP - 100) + 50;
        pipes.push_back(p);
    }
}

