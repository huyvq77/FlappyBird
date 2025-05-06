#pragma once
#include <string>
#include <SDL.h>
using namespace std;

SDL_Texture* loadTexture(const string& path, SDL_Renderer* renderer);
