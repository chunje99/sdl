//==============================================================================
#include "config.h"
#include "CGame.h"

//==============================================================================

int main(int argc, char *argv[])
{
    google::InitGoogleLogging(argv[0]);
#if defined OS_LINUX
    gflags::ParseCommandLineFlags(&argc, &argv, true);
#elif defined OS_MAC
    google::ParseCommandLineFlags(&argc, &argv, true);
#endif

    const int FPS = 60;
    const int frameDelay = 1000 / FPS;

    Uint32 frameStart;
    int frameTime;

    CGame *game = new CGame();
    game->init("Game Window", 800, 640, 0);
    while (game->running())
    {
        frameStart = SDL_GetTicks();
        game->handleEvents();
        game->update();
        game->render();
        frameTime = SDL_GetTicks() - frameStart;
        if (frameTime < frameDelay)
        {
            SDL_Delay(frameDelay - frameTime);
        }
    }
    delete game;
}

//==============================================================================
