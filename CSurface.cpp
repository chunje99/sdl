//==============================================================================
#include "CSurface.h"

//==============================================================================
CSurface::CSurface() {
}

//==============================================================================
SDL_Surface* CSurface::OnLoad(SDL_Window *window, SDL_Renderer* renderer, char* File) {
	SDL_Surface* Surf_Temp = NULL;
	SDL_Surface* Surf_Return = NULL;

	if((Surf_Temp = IMG_Load(File)) == NULL) {
		return NULL;
	}

	//Surf_Return = SDL_ConvertSurfaceFormat(Surf_Temp, SDL_GetWindowPixelFormat(window), 0);
	//Surf_Return = SDL_ConvertSurfaceFormat(Surf_Temp, SDL_PIXELFORMAT_RGBA8888, 0);
	Surf_Return = SDL_ConvertSurfaceFormat(Surf_Temp, SDL_PIXELFORMAT_RGBA8888, 0);
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

    //SDL_RenderClear(renderer);
    //Draw the texture
    SDL_RenderCopy(renderer, tex, NULL, &SrcR);
    //Update the screen
    //SDL_RenderPresent(renderer);

	return true;
}

bool CSurface::OnDrawPlayer(SDL_Renderer *renderer, SDL_Texture* tex, int X, int Y, int W, int H)
{
	if(renderer == NULL || tex == NULL ) {
		return false;
	}

	SDL_Rect DestR, SrcR;

	DestR.x = X;
	DestR.y = Y;
	DestR.w = W;
	DestR.h = H;

	SrcR.x = 0;
	SrcR.y = 0;
	SrcR.w = 200;
	SrcR.h = 200;

    //SDL_RenderClear(renderer);
    //Draw the texture
    SDL_RenderCopy(renderer, tex, NULL, &DestR);

	return true;
}

bool CSurface::Transparent(SDL_Surface *Surf_Dest, int R, int G, int B)
{
	if (Surf_Dest == NULL)
	{
		return false;
	}

	SDL_SetColorKey(Surf_Dest, SDL_TRUE | SDL_RLEACCEL, SDL_MapRGB(Surf_Dest->format, R, G, B));

	return true;
}

bool CSurface::OnDraw(SDL_Renderer *renderer, SDL_Texture* tex, int X, int Y, int W, int H)
{
	if(renderer == NULL || tex == NULL ) {
		return false;
	}

	SDL_Rect DestR;

	DestR.x = X;
	DestR.y = Y;
	DestR.w = W;
	DestR.h = H;

    SDL_RenderCopy(renderer, tex, NULL, &DestR);

	return true;
}