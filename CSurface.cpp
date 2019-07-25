//==============================================================================
#include "CSurface.h"

//==============================================================================
CSurface::CSurface() {
}

//==============================================================================
SDL_Surface* CSurface::OnLoad(SDL_Window *window, char* File) {
	SDL_Surface* Surf_Temp = NULL;
	SDL_Surface* Surf_Return = NULL;

	if((Surf_Temp = IMG_Load(File)) == NULL) {
		return NULL;
	}

	Surf_Return = SDL_ConvertSurfaceFormat(Surf_Temp, SDL_GetWindowPixelFormat(window), 0);
	//Surf_Return = SDL_DisplayFormat(Surf_Temp);
	SDL_FreeSurface(Surf_Temp);

	return Surf_Return;
}

//==============================================================================
bool CSurface::OnDraw(SDL_Surface* Surf_Dest, SDL_Surface* Surf_Src, int X, int Y) {
	if(Surf_Dest == NULL || Surf_Src == NULL) {
		return false;
	}

	SDL_Rect DestR;

	DestR.x = X;
	DestR.y = Y;

	SDL_BlitSurface(Surf_Src, NULL, Surf_Dest, &DestR);

	return true;
}

//------------------------------------------------------------------------------
bool CSurface::OnDraw(SDL_Surface* Surf_Dest, SDL_Surface* Surf_Src, int X, int Y, int X2, int Y2, int W, int H) {
	if(Surf_Dest == NULL || Surf_Src == NULL) {
		return false;
	}

	SDL_Rect DestR;

	DestR.x = X;
	DestR.y = Y;

	SDL_Rect SrcR;

	SrcR.x = X2;
	SrcR.y = Y2;
	SrcR.w = W;
	SrcR.h = H;

	SDL_BlitSurface(Surf_Src, &SrcR, Surf_Dest, &DestR);

	return true;
}

bool CSurface::OnDraw(SDL_Renderer *renderer, SDL_Texture* tex, SDL_Surface *Surf_Src, int X, int Y)
{
	if(renderer == NULL || tex == NULL || Surf_Src == NULL) {
		return false;
	}

	SDL_Rect DestR, SrcR;

	DestR.x = X;
	DestR.y = Y;

	SrcR.x = X;
	SrcR.y = Y;
	SrcR.w = Surf_Src->w;
	SrcR.h = Surf_Src->h;

    SDL_RenderClear(renderer);
    //Draw the texture
    SDL_RenderCopy(renderer, tex, NULL, NULL);
    //Update the screen
    SDL_RenderPresent(renderer);

	return true;
}

bool CSurface::OnDraw(SDL_Renderer *renderer, SDL_Texture* tex, int X, int Y)
{
	if(renderer == NULL || tex == NULL ) {
		return false;
	}

	SDL_Rect DestR, SrcR;

	DestR.x = X;
	DestR.y = Y;

	SrcR.x = X;
	SrcR.y = Y;

    SDL_RenderClear(renderer);
    //Draw the texture
    SDL_RenderCopy(renderer, tex, NULL, NULL);
    //Update the screen
    SDL_RenderPresent(renderer);

	return true;
}