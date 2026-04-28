#include "SDL3/SDL_events.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_keycode.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_timer.h"
#include "SDL3/SDL_video.h"
#include <SDL3/SDL.h>

/*
 * Class prototypes
 */

class PPlayer
{
  public:
    static constexpr int kPlayerVel = 10;
    static constexpr int kPlayerWidth = 50;
    static constexpr int kPlayerHeight = 100;

    PPlayer();
    void setPosition(int x, int y);
    void render();
    void handleEvent(SDL_Event &e, int playerID);
    void collide();
    void move();
    
    const SDL_FRect& getCollider() const;
  private:
    int mPositionX;
    int mPositionY;
    
    int mVelY;
    SDL_FRect mCollisionBox;
};

class PBall
{
  public:
    static constexpr int kBallWidth = 20;

    PBall();
    void render();
    void move(const SDL_FRect &boxCollider1, const SDL_FRect &boxCollider2);
    bool collide(const SDL_FRect &boxCollider1, const SDL_FRect &boxCollider2);
    void setPosition(int x, int y);

  private:
    int mVelY;
    int mVelX;
    SDL_FRect mCollisionBox;
};

SDL_Window *gWindow;

SDL_Renderer *gRenderer;

int const windowWidth = 1080;
int const windowHeight = 720;

int gPlayer1Points;
int gPlayer2Points;

bool gPointSomething = false;

bool Init();
bool loadMedia();
void close();
Uint64 frameStartTime{0};

int main(int argc, char *argv[])
{
    int exitCode{0};
    if (Init() == false)
    {
        SDL_Log("Could not initialize program!");
        exitCode = 1;
    }
    else
    {
        bool quit{false};
        SDL_Event e;

        PBall ball;
        PPlayer player1;
        PPlayer player2;

        ball.setPosition(windowWidth / 2 - ball.kBallWidth, windowHeight / 2 - ball.kBallWidth);
        player2.setPosition(windowWidth - 50, 0);
        SDL_zero(e);

        while (quit == false)
        {
            if (gPointSomething)
            {
                ball.setPosition(windowWidth / 2 - ball.kBallWidth, windowHeight / 2 - ball.kBallWidth);
                player1.setPosition(0, 0);
                player2.setPosition(windowWidth - 50, 0);
                SDL_zero(e);
                gPointSomething = false;
            }
            frameStartTime = SDL_GetTicksNS();

            while (SDL_PollEvent(&e) == true)
            {
                if (e.type == SDL_EVENT_QUIT)
                {
                    quit = true;
                }
                else
                {
                    player1.handleEvent(e, 1);
                    player2.handleEvent(e, 2);
                }
            }

            ball.move(player1.getCollider(), player2.getCollider());

            player1.move();
            player1.collide();

            player2.move();
            player2.collide();

            SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
            SDL_RenderClear(gRenderer);
            SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);

            player1.render();
            player2.render();
            ball.render();
            SDL_RenderPresent(gRenderer);

            Uint64 endFrameTime = SDL_GetTicksNS() - frameStartTime;

            constexpr Uint64 nsPerFrame = 1000000000 / 60;
            if (endFrameTime < nsPerFrame)
            {
                SDL_DelayNS(nsPerFrame - endFrameTime);
                endFrameTime = SDL_GetTicksNS() - frameStartTime;
            }
        }
        close();
    }

    return exitCode;
}

bool Init()
{
    bool sucess{true};
    if (SDL_Init(SDL_INIT_VIDEO) == false)
    {
        SDL_Log("SDL Could not initalize!");
        sucess = false;
    }
    else
    {
        if (SDL_CreateWindowAndRenderer("pong", windowWidth, windowHeight, 0, &gWindow, &gRenderer) == false)
        {
            SDL_Log("Window cold not  be created");
            sucess = false;
        };
    }
    return sucess;
}

void close()
{
    SDL_DestroyRenderer(gRenderer);
    gRenderer = nullptr;

    SDL_DestroyWindow(gWindow);
    gWindow = nullptr;

    SDL_Quit();
}

/*
 * Class implementations
 */

// PLAYER IMPLEMENTATION
PPlayer::PPlayer() : mPositionX{0}, mPositionY{0}, mCollisionBox{0, 0, kPlayerWidth, kPlayerHeight}, mVelY{0} {};
void PPlayer::handleEvent(SDL_Event &e, int playerID)
{
    if (playerID == 1)
    {
        if (e.type == SDL_EVENT_KEY_DOWN && e.key.repeat == 0)
        {
            switch (e.key.key)
            {
            case SDLK_W:
                mVelY -= kPlayerVel;
                break;
            case SDLK_S:
                mVelY += kPlayerVel;
                break;
            }
        }
        else if (e.type == SDL_EVENT_KEY_UP && e.key.repeat == 0)
        {
            switch (e.key.key)
            {
            case SDLK_W:
                mVelY += kPlayerVel;
                break;
            case SDLK_S:
                mVelY -= kPlayerVel;
                break;
            }
        }
    }
    else if (playerID == 2)
    {
        if (e.type == SDL_EVENT_KEY_DOWN && e.key.repeat == 0)
        {
            switch (e.key.key)
            {
            case SDLK_UP:
                mVelY -= kPlayerVel;
                break;
            case SDLK_DOWN:
                mVelY += kPlayerVel;
                break;
            }
        }
        else if (e.type == SDL_EVENT_KEY_UP && e.key.repeat == 0)
        {
            switch (e.key.key)
            {
            case SDLK_UP:
                mVelY += kPlayerVel;
                break;
            case SDLK_DOWN:
                mVelY -= kPlayerVel;
                break;
            }
        }
    }
}
void PPlayer::render()
{
    SDL_RenderFillRect(gRenderer, &mCollisionBox);
}

void PPlayer::move()
{
    mCollisionBox.y += mVelY;
}

void PPlayer::setPosition(int x, int y)
{
    mCollisionBox.y = y;
    mCollisionBox.x = x;
}

// This could be in move() function
void PPlayer::collide()
{
    if (mCollisionBox.y + kPlayerHeight > windowHeight)
    {
        mCollisionBox.y -= mVelY;
    }

    if (mCollisionBox.y < 0)
    {
        mCollisionBox.y -= mVelY;
    }
}

const SDL_FRect& PPlayer::getCollider() const
{
    return mCollisionBox;
}

// BALL IMPLEMENTATION
PBall::PBall() : mVelX{5}, mVelY{5}, mCollisionBox{0, 0, kBallWidth, kBallWidth}
{
}

void PBall::render()
{
    SDL_RenderFillRect(gRenderer, &mCollisionBox);
}

void PBall::setPosition(int x, int y)
{
    mCollisionBox.x = x;
    mCollisionBox.y = y;
}

void PBall::move(const SDL_FRect &boxCollider1, const SDL_FRect &boxCollider2)
{

    if (mCollisionBox.y + mCollisionBox.h > windowHeight || collide(boxCollider1, boxCollider2))
    {
        mVelY = -1 * mVelY;
    }
    else if (mCollisionBox.y < 0)
    {
        mVelY = -1 * mVelY;
    }

    mCollisionBox.y += mVelY;

    if (collide(boxCollider1, boxCollider2))
    {
        mVelX = -1 * mVelX;
    }
    
    mCollisionBox.x += mVelX;
    
    if(mCollisionBox.x + mCollisionBox.w > windowWidth)
    {
        gPlayer1Points++;
        gPointSomething = true;
    }
    else if(mCollisionBox.x < 0)
    {
        gPlayer2Points++;
        gPointSomething = true;
    }
}

bool PBall::collide(const SDL_FRect &boxCollider1, const SDL_FRect &boxCollider2)
{

    if (mCollisionBox.x + mCollisionBox.w > boxCollider1.x && mCollisionBox.x < boxCollider1.x + boxCollider1.w &&
        mCollisionBox.y + mCollisionBox.h > boxCollider1.y && mCollisionBox.y < boxCollider1.y + boxCollider1.h)
    {
        return true;
    }
    
    else if (mCollisionBox.x + mCollisionBox.w > boxCollider2.x && mCollisionBox.x < boxCollider2.x + boxCollider2.w &&
        mCollisionBox.y + mCollisionBox.h > boxCollider2.y && mCollisionBox.y < boxCollider2.y + boxCollider2.h)
    {
        return true;
    }
    else
    {
        return false;
    }
}
 