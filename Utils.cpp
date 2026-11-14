#include "Utils.h"
#include <cmath>

//functie pentru desenarea unui cerc plin SDL3
void drawCircle(SDL_Renderer* renderer, float cx, float cy, float radius) 
{
    int r = static_cast<int>(radius);
    for (int dy = -r; dy <= r; dy++)
    {
        for (int dx = -r; dx <= r; dx++) 
        {
            //verificam ecuatia cercului: x^2 + y^2 <= r^2
            if (dx * dx + dy * dy <= r * r) 
            {
                //desenare cerc
                SDL_RenderPoint(renderer, cx + dx, cy + dy);
            }
        }
    }
}

//verificam daca un dreptunghi s a lovit de un proiectil 
bool checkCircleRectCollision(float cx, float cy, float r, float rx, float ry, float rw, float rh)
{
    // Calculam marginile dreptunghiului
    float left = rx - rw / 2.0f;
    float right = rx + rw / 2.0f;
    float top = ry - rh / 2.0f;
    float bottom = ry + rh / 2.0f;

    float closestX = cx;
    if (cx < left) closestX = left;
    else if (cx > right) closestX = right;

    float closestY = cy;
    if (cy < top) closestY = top;
    else if (cy > bottom) closestY = bottom;

    float distX = cx - closestX;
    float distY = cy - closestY;
    //coliziune
    return (distX * distX + distY * distY) <= (r * r);
}

bool checkRectRectCollision(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2) 
{
    return (std::abs(x1 - x2) * 2.0f < (w1 + w2)) &&
        (std::abs(y1 - y2) * 2.0f < (h1 + h2));
}