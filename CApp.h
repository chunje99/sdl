//==============================================================================
// SDL Tutorial 1
//==============================================================================
#ifndef _CAPP_H_
#define _CAPP_H_

#include <iostream>
#include <SDL.h>

#include "CSurface.h"
#include "CEvent.h"

//==============================================================================
class CApp : public CEvent
{
private:
    bool Running;

    SDL_Surface *Surf_Display;
    SDL_Surface *Surf_Test;

    SDL_Window *screen;
    SDL_Renderer *renderer;
    SDL_Texture *tex;
    int X;
    int Y;

public:
    CApp();

    int OnExecute();

public:
    bool OnInit();

    void OnEvent(SDL_Event *Event);

    void OnLoop();

    void OnRender();

    void OnCleanup();

    void OnExit();

	void OnKeyDown(SDL_Keycode sym, Uint16 mod, SDL_Scancode scancode);

	void OnKeyUp(SDL_Keycode sym, Uint16 mod, SDL_Scancode scancode);

	void OnMouseMove(int mX, int mY, int relX, int relY, bool Left, bool Right, bool Middle);
};
    //==============================================================================

#endif
