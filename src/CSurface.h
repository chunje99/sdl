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
		static SDL_Surface* OnLoad(SDL_Window *window, SDL_Renderer* renderer, char* File);
		static bool OnDraw(SDL_Surface* Surf_Dest, SDL_Surface* Surf_Src, int X, int Y);
		static bool OnDrawPlayer(SDL_Renderer* renderer, SDL_Texture* tex, int X, int Y, int W, int H);
		static bool OnDraw(SDL_Renderer* renderer, SDL_Texture* tex, int X, int Y, int W, int H);
		static bool OnDraw(SDL_Renderer* renderer, SDL_Texture* tex, SDL_Surface* Surf_Src, int X, int Y);
		static bool Transparent(SDL_Surface* Surf_Dest, int R, int G, int B);


		static SDL_Texture * Load(SDL_Renderer* renderer, const char* File);
		static bool Draw(SDL_Renderer* renderer, SDL_Texture* tex, SDL_Rect srcRect, SDL_Rect destRect);
};

//==============================================================================

#endif