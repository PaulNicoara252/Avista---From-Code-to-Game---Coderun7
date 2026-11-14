#pragma once
#include <SDL3/SDL.h>

void drawCircle(SDL_Renderer* renderer, float cx, float cy, float radius);
bool checkCircleRectCollision(float cx, float cy, float r, float rx, float ry, float rw, float rh);
bool checkRectRectCollision(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2);