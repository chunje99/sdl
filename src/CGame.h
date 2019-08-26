#ifndef SDL_CGAME_H
#define SDL_CGAME_H

#include "SDL.h"
#include "SDL_image.h"
#include "CEvent.h"
#include <iostream>
#include <vector>
#include <glog/logging.h>
#include <gflags/gflags.h>

class AssetManager;
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
    static bool isRunning;
    static SDL_Rect camera;
    static AssetManager* assets;
    enum groupLabels : std::size_t
    {
        groupMap,
        groupPlayers,
        groupColliders,
        groupProjectiles

    };

private:
    void OnEvent(SDL_Event *Event);
    void OnExit();

    int cnt = 0;
    SDL_Window * window;

};
#endif