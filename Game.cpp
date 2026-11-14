#include "Game.h"
#include "Constants.h"
#include "Utils.h"
#include <vector>
#include <cmath>
#include <cstdlib>

// structura inamic
struct Enemy {
    float x, y; 
    float speedY; //viteza pe y
    float timeAlive; 
    float fireTimer;
    bool hasFiredFirst;
    bool active; // stare inamic
};

//structura proiectil
struct Projectile {
    float x, y; 
    float vx, vy; //vitezele pe x si y
    bool isPlayer; //true daca e tras de jucator sau false daca e tras de inamic
};

//variabile de stare
static float shipX = 0.0f, shipY = 0.0f; 
static float shipVelX = 0.0f, shipVelY = 0.0f;
static float mouseRelX = 0.0f, mouseRelY = 0.0f;
static float lastMouseDirX = 0.0f, lastMouseDirY = 0.0f; 
static float mouseMoveKeepAlive = 0.0f;

static bool isFiring = false; //daca tragem continuu
static float playerFireTimer = 0.0f; //rata de foc, cronometru

static bool gameOver = false; //flag daca s  a incheiat jocul
static bool gameWon = false; //flag pentru victorie
static float scrollOffset = 0.0f;

static Enemy enemies[NUM_SLICES]; //vectorial de pozitie pentru cei 3 inamici pe coloane
static float globalRespawnTimer = 0.0f; //cronometru pentru generarea inamicilor
static std::vector<Projectile> projectiles; //proiectilele de pe ecran

// Functii interne

//functie pt generarea de inamic
static void spawnEnemyInSlice(int sliceIndex, float windowWidth) 
{
    //marja pentru banda de miscare(de scroll)
    float margin = 20.0f;
    float playableWidth = windowWidth - 2.0f * margin;
    float sliceWidth = playableWidth / NUM_SLICES;

    //calculam limitele
    float minX = margin + sliceIndex * sliceWidth + SHIP_SIZE / 2.0f;
    float maxX = margin + (sliceIndex + 1) * sliceWidth - SHIP_SIZE / 2.0f;

    //poz aleatorie pe axa x
    float rand01 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    enemies[sliceIndex].x = minX + rand01 * (maxX - minX);
    enemies[sliceIndex].y = SHIP_SIZE / 2.0f; 

    //viteza verticala aleatorie
    float randSpeed = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    enemies[sliceIndex].speedY = 50.0f + randSpeed * 100.0f;

    //initializarea starilor inamici
    enemies[sliceIndex].timeAlive = 0.0f;
    enemies[sliceIndex].fireTimer = 0.0f;
    enemies[sliceIndex].hasFiredFirst = false;
    enemies[sliceIndex].active = true;
}

// functia de tragere a 3 proiectile
static void enemyFire(float ex, float ey) 
{
    //calcularea unghiului
    float dx = shipX - ex;
    float dy = shipY - ey;
    float baseAngle = std::atan2(dy, dx);

    // +15 si -15 grade
    float angles[3] = 
    {
        baseAngle,
        baseAngle + 15.0f * (3.14159265f / 180.0f),
        baseAngle - 15.0f * (3.14159265f / 180.0f)
    };

    for (int i = 0; i < 3; i++) {
        Projectile p;
        p.x = ex; p.y = ey;
        p.vx = std::cos(angles[i]) * ENEMY_PROJ_SPEED;
        p.vy = std::sin(angles[i]) * ENEMY_PROJ_SPEED;
        p.isPlayer = false; 
        projectiles.push_back(p);
    }
}

// Functii publice ( in Game.h)

//returneaza true daca jocul s-a terminat
bool isGameOver() 
{
    return gameOver;
}

// starea de tragere a jucatorului
void setPlayerFiring(bool firing) 
{
    if (firing && !isFiring && !gameOver) 
    {
        
        Projectile p;
        p.x = shipX; p.y = shipY;
        p.vx = 0.0f; p.vy = -PLAYER_PROJ_SPEED;
        p.isPlayer = true; //proiectil jucator 
        projectiles.push_back(p);
        playerFireTimer = 0.0f;
    }
    isFiring = firing;
}


void addMouseMotion(float xrel, float yrel)
{
    mouseRelX += xrel;
    mouseRelY += yrel;
}


void clearMouseMotion() 
{
    mouseRelX = 0.0f;
    mouseRelY = 0.0f;
}

// repulsor
void useRepulsor()
{

    for (size_t i = 0; i < projectiles.size(); i++) 
    {
        if (!projectiles[i].isPlayer) 
        {
            float dx = projectiles[i].x - shipX;
            float dy = projectiles[i].y - shipY;
            float len = std::sqrt(dx * dx + dy * dy);
            if (len > 0.0001f) {
                dx /= len;
                dy /= len;
                projectiles[i].vx += dx * 10.0f;
                projectiles[i].vy += dy * 10.0f;
            }
        }
    }
}

// resetare la apasarea tastei r
void resetGame(float windowWidth, float windowHeight) 
{
    shipX = windowWidth / 2.0f;
    shipY = windowHeight - SHIP_SIZE;
    shipVelX = 0.0f;
    shipVelY = 0.0f;
    lastMouseDirX = 0.0f;
    lastMouseDirY = 0.0f;
    mouseMoveKeepAlive = 0.0f;
    isFiring = false;
    playerFireTimer = 0.0f;
    gameOver = false;
    gameWon = false;
    globalRespawnTimer = 0.0f;
    projectiles.clear();

    //generam inamic per linie
    for (int i = 0; i < NUM_SLICES; i++) 
    {
        spawnEnemyInSlice(i, windowWidth);
    }
}

void updateGame(float dt, float windowWidth, float windowHeight) 
{
    if (gameOver) return; 
    scrollOffset += SCROLL_SPEED * dt;
    if (scrollOffset >= windowHeight) scrollOffset -= windowHeight;

    float mouseLen = std::sqrt(mouseRelX * mouseRelX + mouseRelY * mouseRelY);
    if (mouseLen > 0.001f) 
    {
        float strength = mouseLen / 5.0f;
        if (strength > 2.5f) strength = 2.5f;
        if (strength < 1.0f) strength = 1.0f;

        lastMouseDirX = (mouseRelX / mouseLen) * strength;
        lastMouseDirY = (mouseRelY / mouseLen) * strength;
        mouseMoveKeepAlive = 0.04f;
    }

    if (mouseMoveKeepAlive > 0.0f) 
    {
        mouseMoveKeepAlive -= dt;
        shipVelX += lastMouseDirX * SHIP_ACCEL * dt;
        shipVelY += lastMouseDirY * SHIP_ACCEL * dt;

        //limitare la viteza maxima
        float speed = std::sqrt(shipVelX * shipVelX + shipVelY * shipVelY);
        if (speed > SHIP_MAX_SPEED) {
            shipVelX = (shipVelX / speed) * SHIP_MAX_SPEED;
            shipVelY = (shipVelY / speed) * SHIP_MAX_SPEED;
        }
    }
    else {
        //decelerare
        float speed = std::sqrt(shipVelX * shipVelX + shipVelY * shipVelY);
        if (speed > 0.0f) 
        {
            float drop = SHIP_DECEL * dt;
            float newSpeed = speed - drop;
            if (newSpeed < 0.0f) newSpeed = 0.0f;
            shipVelX = (shipVelX / speed) * newSpeed;
            shipVelY = (shipVelY / speed) * newSpeed;
        }
    }

    //actualizam pozitia nac=vei
    shipX += shipVelX * dt;
    shipY += shipVelY * dt;

    float halfShip = SHIP_SIZE / 2.0f;
    float margin = 20.0f;
    if (shipX < margin + halfShip) { shipX = margin + halfShip; shipVelX = 0.0f; }
    if (shipX > windowWidth - margin - halfShip) { shipX = windowWidth - margin - halfShip; shipVelX = 0.0f; }
    if (shipY < halfShip) { shipY = halfShip; shipVelY = 0.0f; }
    if (shipY > windowHeight - halfShip) { shipY = windowHeight - halfShip; shipVelY = 0.0f; }

    if (isFiring) 
    {
        playerFireTimer += dt;
        while (playerFireTimer >= PLAYER_FIRE_RATE) 
        {
            playerFireTimer -= PLAYER_FIRE_RATE;
            Projectile p;
            p.x = shipX; p.y = shipY;
            p.vx = 0.0f; p.vy = -PLAYER_PROJ_SPEED;
            p.isPlayer = true;
            projectiles.push_back(p);
        }
    }

    for (size_t i = 0; i < projectiles.size(); ) 
    {
        projectiles[i].x += projectiles[i].vx * dt;
        projectiles[i].y += projectiles[i].vy * dt;

        if (projectiles[i].x < -PROJ_RADIUS * 2.0f || projectiles[i].x > windowWidth + PROJ_RADIUS * 2.0f ||
            projectiles[i].y < -PROJ_RADIUS * 2.0f || projectiles[i].y > windowHeight + PROJ_RADIUS * 2.0f) {
            projectiles.erase(projectiles.begin() + i);
        }
        else {
            i++;
        }
    }

    for (int i = 0; i < NUM_SLICES; i++) 
    {
        if (enemies[i].active) {
            enemies[i].y += enemies[i].speedY * dt;
            enemies[i].timeAlive += dt;

            if (enemies[i].timeAlive >= 1.0f)
            {
                if (!enemies[i].hasFiredFirst) 
                {
                    enemyFire(enemies[i].x, enemies[i].y);
                    enemies[i].hasFiredFirst = true;
                    enemies[i].fireTimer = 0.0f;
                }
                else 
                {
                    enemies[i].fireTimer += dt;
                    if (enemies[i].fireTimer >= ENEMY_FIRE_RATE) 
                    {
                        enemies[i].fireTimer -= ENEMY_FIRE_RATE;
                        enemyFire(enemies[i].x, enemies[i].y);
                    }
                }
            }

            // Daca un inamic scapa pe jos din ecra reapare sus
            if (enemies[i].y - SHIP_SIZE / 2.0f > windowHeight) 
            {
                spawnEnemyInSlice(i, windowWidth);
            }
        }
    }

    //timer global pentru respawn
    bool anySliceEmpty = false;
    for (int i = 0; i < NUM_SLICES; i++) 
    {
        if (!enemies[i].active) 
        {
            anySliceEmpty = true;
            break;
        }
    }

    if (anySliceEmpty) {
        globalRespawnTimer += dt;
        if (globalRespawnTimer >= 3.0f)
        {
            globalRespawnTimer -= 3.0f;
            for (int i = 0; i < NUM_SLICES; i++) 
            {
                if (!enemies[i].active) 
                {
                    spawnEnemyInSlice(i, windowWidth);
                    break;
                }
            }
        }
    }
    else 
    {
        globalRespawnTimer = 0.0f;
    }

    for (size_t p = 0; p < projectiles.size(); )
    {
        bool projDestroyed = false;
        if (projectiles[p].isPlayer)
        {
            for (int e = 0; e < NUM_SLICES; e++) 
            {
                if (enemies[e].active)
                {
                    if (checkCircleRectCollision(projectiles[p].x, projectiles[p].y, PROJ_RADIUS,
                        enemies[e].x, enemies[e].y, SHIP_SIZE, SHIP_SIZE))
                    {
                        enemies[e].active = false;//distrugerea inamicului
                        projDestroyed = true;

                        //verificam daca am castigat
                        bool anyEnemyActive = false;
                        for (int i = 0; i < NUM_SLICES; i++)
                        {
                            if (enemies[i].active) 
                            {
                                anyEnemyActive = true;
                                break;
                            }
                        }
                        if (!anyEnemyActive)
                        {
                            gameOver = true;
                            gameWon = true;
                            projectiles.clear();
                            return;
                        }
                        break;
                    }
                }
            }
        }

        if (projDestroyed)
        {
            projectiles.erase(projectiles.begin() + p);
        }
        else {
            p++;
        }
    }

    // verificarea coliziunilor fatale
    for (int i = 0; i < NUM_SLICES; i++) 
    {
        if (enemies[i].active && checkRectRectCollision(shipX, shipY, SHIP_SIZE, SHIP_SIZE, enemies[i].x, enemies[i].y, SHIP_SIZE, SHIP_SIZE)) {
            gameOver = true; gameWon = false;
            projectiles.clear();
            for (int j = 0; j < NUM_SLICES; j++) enemies[j].active = false;
            return;
        }
    }
    for (size_t p = 0; p < projectiles.size(); p++) 
    {
        if (!projectiles[p].isPlayer && checkCircleRectCollision(projectiles[p].x, projectiles[p].y, PROJ_RADIUS, shipX, shipY, SHIP_SIZE, SHIP_SIZE)) {
            gameOver = true; gameWon = false;
            projectiles.clear();
            for (int j = 0; j < NUM_SLICES; j++) enemies[j].active = false;
            return;
        }
    }
}

//functia de randare 
void drawGame(SDL_Renderer* renderer, float windowWidth, float windowHeight) 
{
    const float marginWidth = 20.0f;
    const float blockWidth = 14.0f;
    const float blockHeight = 20.0f;
    const float blockSpacing = 45.0f;

    float yOffset = std::fmod(scrollOffset, blockSpacing);

    for (float y = -blockSpacing + yOffset; y < windowHeight; y += blockSpacing) 
    {
        SDL_FRect leftBlock = { 0.0f, y, blockWidth, blockHeight };
        SDL_FRect rightBlock = { windowWidth - blockWidth, y, blockWidth, blockHeight };

        SDL_SetRenderDrawColor(renderer, 225, 225, 225, 255);
        SDL_RenderFillRect(renderer, &leftBlock);
        SDL_RenderFillRect(renderer, &rightBlock);

        SDL_SetRenderDrawColor(renderer, 130, 130, 130, 255);
        SDL_RenderRect(renderer, &leftBlock);
        SDL_RenderRect(renderer, &rightBlock);
    }

    auto drawRectOutline = [&](const SDL_FRect& rect) 
        {
        SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
        SDL_RenderRect(renderer, &rect);
        };

    if (!gameOver)
    {
        //inamicii
        for (int i = 0; i < NUM_SLICES; i++) 
        {
            if (enemies[i].active)
            {
                SDL_FRect enemyRect = { enemies[i].x - SHIP_SIZE / 2.0f, enemies[i].y - SHIP_SIZE / 2.0f, SHIP_SIZE, SHIP_SIZE };
                SDL_SetRenderDrawColor(renderer, 255, 153, 0, 255);
                SDL_RenderFillRect(renderer, &enemyRect);
                drawRectOutline(enemyRect);
            }
        }

        //proiectilele
        for (size_t i = 0; i < projectiles.size(); i++) 
        {
            if (projectiles[i].isPlayer)
            {
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            }
            else {
                SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
            }
            drawCircle(renderer, projectiles[i].x, projectiles[i].y, PROJ_RADIUS);
        }
    }

    //nava jucatorului cu culoarea diferita in functie de starea jocului
    if (!gameOver) 
    {
        SDL_SetRenderDrawColor(renderer, 0, 220, 0, 255);
    }
    else if (gameWon) 
    {
        SDL_SetRenderDrawColor(renderer, 0, 102, 255, 255);
    }
    else {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    }

    SDL_FRect shipRect = { shipX - SHIP_SIZE / 2.0f, shipY - SHIP_SIZE / 2.0f, SHIP_SIZE, SHIP_SIZE };
    SDL_RenderFillRect(renderer, &shipRect);
    drawRectOutline(shipRect);
}