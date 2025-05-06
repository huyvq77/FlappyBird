#pragma once

#include <SDL.h>
#include <vector>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 800;
const int GAP = 170;
const int pipeWidth = 80;
const int pipeDistance = 300;
const float gravity = 0.5f;
const float jumpStrength = -8.0f;

struct PipePair {
	int x;
	int gapY;
};

struct Button {
	SDL_Rect rect;
	bool isHovered;
	SDL_Texture* normalTexture;
	SDL_Texture* hoverTexture;
};
