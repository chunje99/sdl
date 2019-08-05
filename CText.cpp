#include "CText.h"

CText::CText()
{
    m_font = NULL;
    m_ttfSurface = NULL;
    m_ttfTexture = NULL;
}

CText::~CText()
{
    TTF_CloseFont(m_font);
    SDL_FreeSurface(m_ttfSurface);
    SDL_DestroyTexture(m_ttfTexture);
}

bool CText::OnInit(SDL_Window* screen, SDL_Renderer* renderer, const char* text)
{
	m_screen = screen;
	m_renderer = renderer;
    m_font = TTF_OpenFont("font/NanumGothic.ttf", 18);
    if (m_font == nullptr)
    {
        LOG(ERROR) << "TTF_OpenFont ERROR" << TTF_GetError();
        return false;
    }
    m_color = {255, 255, 255};
	return SetText(text);
}

bool CText::SetText(const char* text)
{
	if(m_ttfSurface)
		SDL_FreeSurface(m_ttfSurface);
	if(m_ttfTexture)
		SDL_DestroyTexture(m_ttfTexture);

	m_ttfSurface = TTF_RenderText_Solid(m_font, text, m_color);
    m_ttfTexture = SDL_CreateTextureFromSurface(m_renderer, m_ttfSurface);
	return true;
}

bool CText::OnDraw()
{
    CSurface::OnDraw(m_renderer, m_ttfTexture,
                    m_rect.x,
                    m_rect.y, m_rect.w, m_rect.h);
	return true;
}

bool CText::SetRect(SDL_Rect rect)
{
	m_rect.x = rect.x;
	m_rect.y = rect.y;
	m_rect.w = rect.w;
	m_rect.h = rect.h;
}
