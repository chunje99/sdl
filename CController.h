#ifndef _CController_H_
#define _CController_H_

#include <iostream>
#include <functional>
#include <SDL.h>
#include "CSurface.h"
#include "CEvent.h"
#include "CButton.h"

#include <glog/logging.h>
#include <gflags/gflags.h>

//==============================================================================
class CController : public CEvent
{
private:
    SDL_Window *m_screen;
    SDL_Renderer *m_renderer;

    int m_windowW;
    int m_windowH;


public:
    CController();

    CButton m_playButton;
    CButton m_stopButton;
    CButton m_play2Button;
    CButton m_stop2Button;
    CButton m_cogButton;
    CButton m_switchButton;

public:
    bool OnInit(SDL_Window *screen, SDL_Renderer *renderer);
    bool OnLoad();
    bool OnDraw();
    void OnEvent(SDL_Event *Event);
};

//==============================================================================
#endif
