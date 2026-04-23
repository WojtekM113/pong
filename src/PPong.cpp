#include "SDL3/SDL_events.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_keycode.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_video.h"
#include <SDL3/SDL.h>

/*
 * Class prototypes
 */

class PPlayer
{
  public:
    static constexpr int kPlayerVel = 10;

    PPlayer();
    void setPosition(int x, int y);
    void render();
    void handleEvent(SDL_Event &e);
    void collide();
    void move();

  private:
    int mPositionX;
    int mPositionY;
    int mVelY;
    SDL_FRect mCollisionBox;
};

SDL_Window *gWindow;

SDL_Renderer *gRenderer;

int const windowWidth = 1080;
int const windowHeight = 720;

bool Init();
bool loadMedia();
void close();

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

        PPlayer player1;

        SDL_zero(e);

        while (quit == false)
        {
            while (SDL_PollEvent(&e) == true)
            {
                if (e.type == SDL_EVENT_QUIT)
                {
                    quit = true;
                }
                else
                {
                    player1.handleEvent(e);
                }
            }
            
            player1.move();
            
            SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
            SDL_RenderClear(gRenderer);
            SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
            player1.render();
            SDL_RenderPresent(gRenderer);
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

PPlayer::PPlayer() : mPositionX{0}, mPositionY{0}, mCollisionBox{0, 0, 50, 100}, mVelY{0} {};
void PPlayer::handleEvent(SDL_Event &e)
{
    if (e.type == SDL_EVENT_KEY_DOWN && e.key.repeat == 0)
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
    else if (e.type == SDL_EVENT_KEY_UP && e.key.repeat == 0)
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
}
void PPlayer::render()
{
    SDL_RenderFillRect(gRenderer, &mCollisionBox);
}

void PPlayer::move()
{
    mPositionX += mVelY;
    mCollisionBox.y = mPositionX;
}