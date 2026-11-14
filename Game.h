#pragma once
#include <SDL3/SDL.h>

//logica principala
void resetGame(float windowWidth, float windowHeight);
void updateGame(float dt, float windowWidth, float windowHeight);
void drawGame(SDL_Renderer* renderer, float windowWidth, float windowHeight);

//functii pentru preluarea controalelor din main.cpp
void setPlayerFiring(bool firing);
void addMouseMotion(float xrel, float yrel);
void clearMouseMotion();
void useRepulsor();

bool isGameOver();