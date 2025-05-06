#pragma once
#include <SDL.h>
#include <SDL_mixer.h>

bool initAudio();
void loadSounds();
void playFlapSound();
void playGameOverSound();
void playPointSound();
void playBGM();
void stopBGM();
void cleanAudio();

extern bool musicEnabled;
void toggleMusic();

