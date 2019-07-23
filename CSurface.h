//==============================================================================
// Surface functions
//==============================================================================
#ifndef _CSURFACE_H_
    #define _CSURFACE_H_

#include <SDL.h>
#include <SDL_image.h>

//==============================================================================
class CSurface {
	public:
		CSurface();

	public:
		static SDL_Surface* OnLoad(SDL_Window *window, char* File);

		static bool OnDraw(SDL_Surface* Surf_Dest, SDL_Surface* Surf_Src, int X, int Y);

		static bool OnDraw(SDL_Surface* Surf_Dest, SDL_Surface* Surf_Src, int X, int Y, int X2, int Y2, int W, int H);

		static bool OnDraw(SDL_Renderer* renderer, SDL_Texture* tex, SDL_Surface* Surf_Src, int X, int Y);
};

//==============================================================================

#endif