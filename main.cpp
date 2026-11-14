#include <iostream>
#include <ctime>
#include <SDL3/SDL.h>
#include "Constants.h"
#include "Game.h"

//var globale
SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
SDL_Event currentEvent;
SDL_Color backgroundColor = { 255, 255, 255, 255 };
bool quit = false;
float displayScale = 1.0f;
Uint64 lastTicksNS = 0;


//functie pentru initializarea bibliotecii SDL 
bool initWindow() 
{
    if (!SDL_Init(SDL_INIT_VIDEO)) 
    {
        SDL_Log("SDL initialization failed: %s\n", SDL_GetError());
        return false;
    }

    displayScale = SDL_GetDisplayContentScale(1);
    //preluam scala ecranului
    if (displayScale <= 0.0f) 
    {
        displayScale = 1.0f;
    }

    if (!SDL_CreateWindowAndRenderer("SDL Space Shooter", 
        static_cast<int>(displayScale * WINDOW_WIDTH), 
        static_cast<int>(displayScale * WINDOW_HEIGHT), 
        SDL_WINDOW_RESIZABLE, &window, &renderer)) 
    {
        SDL_Log("Failed to create window and renderer: %s\n", SDL_GetError());
        return false;
    }
    //set scala de randare globala
    SDL_SetRenderScale(renderer, displayScale, displayScale);
    
    //pentru o miscare sincronizata cu rata monitorilui
    SDL_SetRenderVSync(renderer, 1);
    SDL_SetWindowRelativeMouseMode(window, true);
    return true;
}
// Functie pentru procesarea evenimentelor
void processEvents(float curW, float curH) 
{
    clearMouseMotion();
    // s a inchis fereastra
    while (SDL_PollEvent(&currentEvent)) 
    {
        if (currentEvent.type == SDL_EVENT_QUIT) 
        {
            quit = true;
        }

        if (currentEvent.type == SDL_EVENT_MOUSE_BUTTON_DOWN && !isGameOver()) 
        {
            if (currentEvent.button.button == SDL_BUTTON_LEFT) 
            {
                setPlayerFiring(true);
            } else if (currentEvent.button.button == SDL_BUTTON_RIGHT) 
            {
                useRepulsor();
            }
        }

        if (currentEvent.type == SDL_EVENT_MOUSE_BUTTON_UP) 
        {
            if (currentEvent.button.button == SDL_BUTTON_LEFT) 
            {
                setPlayerFiring(false);
            }
        }

        if (currentEvent.type == SDL_EVENT_MOUSE_MOTION)
        {
            addMouseMotion(currentEvent.motion.xrel, currentEvent.motion.yrel);
        }

        if (currentEvent.type == SDL_EVENT_KEY_DOWN) 
        {
            if (currentEvent.key.key == SDLK_R && isGameOver()) 
            {
                resetGame(curW, curH);
            } else if (currentEvent.key.key == SDLK_ESCAPE) 
            {
                quit = true;
            }
        }
    }
}

void drawFrame(float curW, float curH) 
{
    SDL_SetRenderDrawColor(renderer, backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
    SDL_RenderClear(renderer);
    drawGame(renderer, curW, curH);
    SDL_RenderPresent(renderer);
}
void cleanup() 
{
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
}

int main() 
{
    srand(static_cast<unsigned int>(time(nullptr)));

    if (!initWindow())
    {
        std::cout << "Failed to initialize" << std::endl;
        return -1;
    }

    int pixelW, pixelH;
    SDL_GetWindowSizeInPixels(window, &pixelW, &pixelH);
    resetGame(pixelW / displayScale, pixelH / displayScale);

    SDL_zero(currentEvent);
    lastTicksNS = SDL_GetTicksNS();

    while (!quit) 
    {
        Uint64 currentTicksNS = SDL_GetTicksNS();
        float dt = static_cast<float>(currentTicksNS - lastTicksNS) / 1000000000.0f;
        lastTicksNS = currentTicksNS;

        if (dt > 0.05f) dt = 0.05f;

        SDL_GetWindowSizeInPixels(window, &pixelW, &pixelH);
        float curW = pixelW / displayScale;
        float curH = pixelH / displayScale;

        processEvents(curW, curH);
        updateGame(dt, curW, curH);
        drawFrame(curW, curH);
    }

    cleanup();
    return 0;
}