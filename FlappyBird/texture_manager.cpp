#include "texture_manager.h"
#include <SDL_image.h>
#include <iostream>
using namespace std;

SDL_Texture* loadTexture(const string& path, SDL_Renderer* renderer) {
	SDL_Surface* surface = IMG_Load(path.c_str());
	if (!surface) {
		cout << "Load Image Error: " << IMG_GetError() << "\n";
		return NULL;
	}
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);
	return texture;
}
