
#ifndef _CButton_H_
#define _CButton_H_

#include <iostream>
#include <SDL.h>
#include "CSurface.h"
#include "CEvent.h"
#include <functional>

#include <glog/logging.h>
#include <gflags/gflags.h>

//==============================================================================
class CButton: public CEvent
{
private:
    SDL_Window *m_screen;
    SDL_Renderer *m_renderer;
    SDL_Texture *m_texture;

    SDL_Rect m_rect;
    std::string m_name;
    bool m_isMouseOver;
    std::function<void(void)> m_onLButtonClick;
    bool m_isLDown;
    bool m_movableX;
    bool m_movableY;
    bool m_movableW;
    bool m_movableH;
    bool m_isVisible;

public:
    CButton();
    bool OnInit(SDL_Window *screen, SDL_Renderer *renderer, const char* name);
    bool OnLoad(char* filePath);
    bool OnDraw();
    void SetRect(SDL_Rect* rect);
    void OnEvent(SDL_Event *Event);
    std::string GetName(){return m_name;};
    void SetMovableX(bool tf);
    void SetMovableY(bool tf);
    void SetMovableW(bool tf);
    void SetMovableH(bool tf);
    void SetVisible(bool tf);

private:
    void OnMouseMove(int mX, int mY, int relX, int relY, bool Left, bool Right, bool Middle);
    void OnMouseOver();
    void OnMouseOut();
    void OnLButtonDown(int mX, int mY);
    void OnLButtonUp(int mX, int mY);
};

//==============================================================================
#endif
