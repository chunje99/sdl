#ifndef CAPP_TEXT
#define CAPP_TEXT

#include <iostream>
#include <SDL.h>
#include <SDL_ttf.h>
#include "CEvent.h"
#include "CSurface.h"

#include <glog/logging.h>
#include <gflags/gflags.h>

class CText : public CEvent
{
public:
	CText();
	~CText();
	bool OnInit(SDL_Window* screen, SDL_Renderer* renderer, const char* text);
	bool SetText(const char* text);
	bool SetRect(SDL_Rect rect);
	bool OnDraw();

private:
    SDL_Surface *m_ttfSurface;
    SDL_Window *m_screen;
    SDL_Renderer *m_renderer;
    SDL_Texture *m_ttfTexture;
    TTF_Font *m_font;
	SDL_Color m_color;
	SDL_Rect m_rect;
};
#endif