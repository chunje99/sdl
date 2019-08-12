#ifndef SDL_CGAME_H
#define SDL_CGAME_H

#include "SDL.h"
#include "SDL_image.h"
#include "CEvent.h"
#include <iostream>
#include <vector>
#include <glog/logging.h>
#include <gflags/gflags.h>

class ColliderComponent;

class CGame : CEvent
{
public:
    CGame();
    ~CGame();
    void init( const char* titl, int width, int height, bool fullscreen);

    void handleEvents();
    void update();
    bool running() { return isRunning; }
    void render();
    void clean();

    static SDL_Renderer* renderer;
    static SDL_Event event;
    static std::vector<ColliderComponent*> colliders;

private:
    void OnEvent(SDL_Event *Event);
    void OnExit();

    bool isRunning = false;
    int cnt = 0;
    SDL_Window * window;

};
#endif