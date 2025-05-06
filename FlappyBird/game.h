#pragma once

#include <SDL.h>
#include <vector>
#include "utils.h"

using namespace std;

bool init(SDL_Window*& window, SDL_Renderer*& renderer);
void loadNumberTextures(SDL_Renderer* renderer);
void initButtons(SDL_Renderer* renderer);
void loadHighScore();
void saveHighScore();
void renderScore(SDL_Renderer* renderer);
void renderHighScore(SDL_Renderer* renderer);
void renderNumber(SDL_Renderer* renderer, int number, int x, int y);
void resetGame(int baseY);
bool checkCollision(int birdX, int birdY, int birdWidth, int birdHeight, int pipeX, int pipeY, int pipeWidth, int pipeHeight);

extern int score;
extern int highScore;
extern int lastPassedPipe;
extern float birdY;
extern float velocityY;
extern bool gameOver;
extern bool birdFalling;
extern SDL_Texture* numberTextures[10];
extern vector<PipePair> pipes;
extern Button tryAgainButton;
extern Button exitButton;

