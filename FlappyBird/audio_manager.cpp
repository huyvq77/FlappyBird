#include "audio_manager.h"
#include <iostream>
using namespace std;

Mix_Music* bgm = NULL;
Mix_Chunk* flapSound = NULL;
Mix_Chunk* gameOverSound = NULL;
Mix_Chunk* pointSound = NULL;
bool musicEnabled = true;

bool initAudio() {
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        cout << "SDL_mixer Error: " << Mix_GetError() << endl;
        return false;
    }
    return true;
}

void loadSounds() {
    bgm = Mix_LoadMUS("assets/bgm.mp3");
    if (!bgm) cout << "Failed to load background music: " << Mix_GetError() << endl;

    flapSound = Mix_LoadWAV("assets/flap.wav");
    if (!flapSound) cout << "Failed to load flap sound: " << Mix_GetError() << endl;

    gameOverSound = Mix_LoadWAV("assets/gameover.wav");
    if (!gameOverSound) cout << "Failed to load game over sound: " << Mix_GetError() << endl;

    pointSound = Mix_LoadWAV("assets/point.wav");
    if (!pointSound) cout << "Failed to load point sound: " << Mix_GetError() << endl;
}

void playFlapSound() {
    if (flapSound) Mix_PlayChannel(-1, flapSound, 0);
}

void playGameOverSound() {
    if (gameOverSound) Mix_PlayChannel(-1, gameOverSound, 0);
}

void playPointSound() {
    if (pointSound) Mix_PlayChannel(-1, pointSound, 0);
}

void playBGM() {
    if (musicEnabled && bgm) {
        Mix_PlayMusic(bgm, -1);
    }
}

void stopBGM() {
    if (Mix_PlayingMusic()) Mix_HaltMusic();
}

void toggleMusic() {
    musicEnabled = !musicEnabled;
    if (musicEnabled) {
        playBGM();
    } else {
        stopBGM();
    }
}

void cleanAudio() {
    Mix_FreeMusic(bgm);
    Mix_FreeChunk(flapSound);
    Mix_FreeChunk(gameOverSound);
    Mix_FreeChunk(pointSound);
    Mix_CloseAudio();
}
