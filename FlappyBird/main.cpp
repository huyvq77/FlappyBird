#include "utils.h"
#include "texture_manager.h"
#include "game.h"
#include "audio_manager.h"

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <iostream>

using namespace std;

int main(int argc, char* argv[]) {
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;

    if (!init(window, renderer)) return -1;

    if (!initAudio()) return -1;
    loadSounds();

    if (TTF_Init() == -1) {
        cout << "TTF_Init Error: " << TTF_GetError() << endl;
        return -1;
    }

    TTF_Font* font = TTF_OpenFont("assets/PressStart2P-Regular.ttf", 24);
    if (!font) {
        cout << "Font load error: " << TTF_GetError() << endl;
        return -1;
    }

    SDL_Color white = {255, 255, 255};
    SDL_Surface* scoreSur = TTF_RenderText_Blended(font, "SCORE", white);
    SDL_Surface* highSur = TTF_RenderText_Blended(font, "HIGH SCORE", white);
    SDL_Texture* scoreLogoTex = SDL_CreateTextureFromSurface(renderer, scoreSur);
    SDL_Texture* highScoreLogoTex = SDL_CreateTextureFromSurface(renderer, highSur);
    SDL_FreeSurface(scoreSur);
    SDL_FreeSurface(highSur);

    loadHighScore();

    SDL_Texture* bgTexture = loadTexture("assets/background.png", renderer);
    SDL_Texture* birdTexture = loadTexture("assets/bird.png", renderer);
    SDL_Texture* pipeTexture = loadTexture("assets/pipe.png", renderer);
    SDL_Texture* baseTexture = loadTexture("assets/base.png", renderer);
    SDL_Texture* gameOverTexture = loadTexture("assets/gameover.png", renderer);

    initButtons(renderer);
    loadNumberTextures(renderer);

    if (!bgTexture || !birdTexture || !pipeTexture || !baseTexture || !gameOverTexture ||
            !tryAgainButton.normalTexture || !tryAgainButton.hoverTexture ||
            !exitButton.normalTexture || !exitButton.hoverTexture ||
            !scoreLogoTex || !highScoreLogoTex) return -1;

    int baseHeight = 112;
    int baseY = SCREEN_HEIGHT - baseHeight;
    const int birdWidth = 50;
    const int birdHeight = 40;

    resetGame(baseY);
    stopBGM();
    playBGM();

    bool quit = false;
    SDL_Event e;

    bool gameStarted = false;
    bool transitioning = false;
    float birdX = SCREEN_WIDTH / 2.0f;
    const float birdTargetX = 100.0f;
    const float birdTransitionSpeed = 5.0f;

    float birdJumpOffset = 0;
    bool birdGoingUp = true;
    const float jumpAmplitude = 5.0f;
    const float jumpSpeed = 0.2f;

    float pressAlpha = 255;
    bool fadingOut = true;
    const float fadeSpeed = 5.0f;

    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            } else if (e.type == SDL_MOUSEMOTION) {
                int mouseX = e.motion.x;
                int mouseY = e.motion.y;
                SDL_Point mousePoint = { mouseX, mouseY };
                tryAgainButton.isHovered = SDL_PointInRect(&mousePoint, &tryAgainButton.rect);
                exitButton.isHovered = SDL_PointInRect(&mousePoint, &exitButton.rect);
            } else if (e.type == SDL_MOUSEBUTTONDOWN && gameOver && !birdFalling) {
                int mouseX = e.button.x;
                int mouseY = e.button.y;
                SDL_Point mousePoint = { mouseX, mouseY };
                if (SDL_PointInRect(&mousePoint, &tryAgainButton.rect)) {
                    resetGame(baseY);
                    playBGM();
                } else if (SDL_PointInRect(&mousePoint, &exitButton.rect)) {
                    quit = true;
                }
            }  else if (e.type == SDL_KEYDOWN || e.type == SDL_MOUSEBUTTONDOWN) {
                if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_m) {
                    toggleMusic();
                }


                else if (!gameStarted && !transitioning) {
                    transitioning = true;
                }
                else if (
                    (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_SPACE && e.key.repeat == 0) ||
                    (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT)
                ) {
                    if (!gameOver && gameStarted) {
                        velocityY = jumpStrength;
                        playFlapSound();
                    }
                }
            }
        }

        if (transitioning) {
            birdX -= birdTransitionSpeed;
            if (birdX <= birdTargetX) {
                birdX = birdTargetX;
                transitioning = false;
                gameStarted = true;
            }
        }

        if (gameStarted && !gameOver) {
            velocityY += gravity;
            birdY += velocityY;
            if (birdY + birdHeight > baseY) {
                birdY = baseY - birdHeight;
                gameOver = true;
                birdFalling = true;
                stopBGM();
                playGameOverSound();
            } else if (birdY < 0) {
                birdY = 0;
                gameOver = true;
                birdFalling = true;
                stopBGM();
                playGameOverSound();
            }
            for (size_t i = 0; i < pipes.size(); ++i) {
                pipes[i].x -= 3;
                if (checkCollision(birdX, birdY, birdWidth, birdHeight, pipes[i].x, 0, pipeWidth, pipes[i].gapY) ||
                        checkCollision(birdX, birdY, birdWidth, birdHeight, pipes[i].x, pipes[i].gapY + GAP, pipeWidth, baseY - (pipes[i].gapY + GAP))) {
                    gameOver = true;
                    birdFalling = true;
                    stopBGM();
                    playGameOverSound();
                }
                if (pipes[i].x + pipeWidth < birdX && lastPassedPipe != static_cast<int>(i)) {
                    score++;
                    lastPassedPipe = static_cast<int>(i);
                    playPointSound();
                }
                if (pipes[i].x + pipeWidth < 0) {
                    int lastPipeX = pipes[(i + pipes.size() - 1) % pipes.size()].x;
                    pipes[i].x = lastPipeX + pipeDistance;
                    pipes[i].gapY = rand() % (baseY - GAP - 100) + 50;
                }
            }
        } else if (birdFalling) {
            velocityY += gravity;
            birdY += velocityY;
            if (birdY + birdHeight >= baseY) {
                birdY = baseY - birdHeight;
                birdFalling = false;

                if (score > highScore) {
                    highScore = score;
                    saveHighScore();
                }
            }
        }

        if (!gameStarted && !transitioning) {

            if (birdGoingUp) {
                birdJumpOffset -= jumpSpeed;
                if (birdJumpOffset <= -jumpAmplitude) birdGoingUp = false;
            } else {
                birdJumpOffset += jumpSpeed;
                if (birdJumpOffset >= jumpAmplitude) birdGoingUp = true;
            }

            if (fadingOut) {
                pressAlpha -= fadeSpeed;
                if (pressAlpha <= 100) fadingOut = false;
            } else {
                pressAlpha += fadeSpeed;
                if (pressAlpha >= 255) fadingOut = true;
            }
        }



        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, bgTexture, NULL, NULL);

        if (!gameStarted && !transitioning) {



            SDL_Rect birdRect = { static_cast<int>(SCREEN_WIDTH / 2 - birdWidth / 2), static_cast<int>(birdY + birdJumpOffset), birdWidth, birdHeight };
            SDL_RenderCopy(renderer, birdTexture, NULL, &birdRect);


            SDL_Color color = {255, 255, 255, static_cast<Uint8>(pressAlpha)};
            SDL_Surface* pressSurf = TTF_RenderText_Blended(font, "[Press SPACE or CLICK]", color);
            SDL_Texture* pressTex = SDL_CreateTextureFromSurface(renderer, pressSurf);
            SDL_SetTextureAlphaMod(pressTex, static_cast<Uint8>(pressAlpha));

            SDL_FreeSurface(pressSurf);
            int pw, ph;
            SDL_QueryTexture(pressTex, NULL, NULL, &pw, &ph);
            SDL_Rect pressRect = { SCREEN_WIDTH / 2 - pw / 2, static_cast<int>(birdY + birdHeight + 20 + birdJumpOffset), pw, ph };
            SDL_RenderCopy(renderer, pressTex, NULL, &pressRect);
            SDL_DestroyTexture(pressTex);
        } else if (gameOver && !birdFalling) {

            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 150);
            SDL_Rect overlay = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
            SDL_RenderFillRect(renderer, &overlay);

            SDL_Rect gameOverRect = { SCREEN_WIDTH/2 - 192, SCREEN_HEIGHT/2 - 100, 384, 84 };
            SDL_RenderCopy(renderer, gameOverTexture, NULL, &gameOverRect);

            SDL_Texture* tryAgainTex = tryAgainButton.isHovered ? tryAgainButton.hoverTexture : tryAgainButton.normalTexture;
            SDL_Texture* exitTex = exitButton.isHovered ? exitButton.hoverTexture : exitButton.normalTexture;

            SDL_RenderCopy(renderer, tryAgainTex, NULL, &tryAgainButton.rect);
            SDL_RenderCopy(renderer, exitTex, NULL, &exitButton.rect);

            SDL_Rect scoreLogoRect = { SCREEN_WIDTH / 2 - 75, SCREEN_HEIGHT / 2 - 250, 150, 50 };
            SDL_RenderCopy(renderer, scoreLogoTex, NULL, &scoreLogoRect);
            renderNumber(renderer, score, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - 190);

            renderHighScore(renderer);
            SDL_Rect highScoreLogoRect = { SCREEN_WIDTH / 2 - 75, SCREEN_HEIGHT / 2 + 205, 150, 50 };
            SDL_RenderCopy(renderer, highScoreLogoTex, NULL, &highScoreLogoRect);
        } else {

            for (const auto& pipe : pipes) {
                SDL_Rect pipeBottomRect = { pipe.x, pipe.gapY + GAP, pipeWidth, baseY - (pipe.gapY + GAP) };
                SDL_RenderCopy(renderer, pipeTexture, NULL, &pipeBottomRect);
                SDL_Rect pipeTopRect = { pipe.x, 0, pipeWidth, pipe.gapY };
                SDL_RenderCopyEx(renderer, pipeTexture, NULL, &pipeTopRect, 0, NULL, SDL_FLIP_VERTICAL);
            }

            SDL_Rect birdRect = { static_cast<int>(birdX), static_cast<int>(birdY), birdWidth, birdHeight };
            SDL_RenderCopy(renderer, birdTexture, NULL, &birdRect);
            SDL_Rect baseRect = { 0, baseY, SCREEN_WIDTH, baseHeight };
            SDL_RenderCopy(renderer, baseTexture, NULL, &baseRect);

            if (!gameOver) renderScore(renderer);
        }



        SDL_Color color = {255, 255, 255};
        const char* musicStatusText = "[M]UNMUTE";
        if (musicEnabled) {
            musicStatusText = "[M]MUTE";
        }
        SDL_Surface* musicSurf = TTF_RenderText_Blended(font, musicStatusText, color);
        SDL_Texture* musicTex = SDL_CreateTextureFromSurface(renderer, musicSurf);
        SDL_FreeSurface(musicSurf);
        int mw, mh;
        SDL_QueryTexture(musicTex, NULL, NULL, &mw, &mh);
        SDL_Rect musicRect = { SCREEN_WIDTH - mw - 20, 20, mw, mh };
        SDL_RenderCopy(renderer, musicTex, NULL, &musicRect);
        SDL_DestroyTexture(musicTex);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    SDL_DestroyTexture(bgTexture);
    SDL_DestroyTexture(birdTexture);
    SDL_DestroyTexture(pipeTexture);
    SDL_DestroyTexture(baseTexture);
    SDL_DestroyTexture(gameOverTexture);
    SDL_DestroyTexture(tryAgainButton.normalTexture);
    SDL_DestroyTexture(tryAgainButton.hoverTexture);
    SDL_DestroyTexture(exitButton.normalTexture);
    SDL_DestroyTexture(exitButton.hoverTexture);
    SDL_DestroyTexture(scoreLogoTex);
    SDL_DestroyTexture(highScoreLogoTex);

    for (int i = 0; i < 10; ++i) SDL_DestroyTexture(numberTextures[i]);

    TTF_CloseFont(font);
    TTF_Quit();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    cleanAudio();
    IMG_Quit();
    SDL_Quit();
    return 0;
}
