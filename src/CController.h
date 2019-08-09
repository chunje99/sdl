#ifndef _CController_H_
#define _CController_H_

#include <iostream>
#include <functional>
#include <SDL.h>
#include "CSurface.h"
#include "CEvent.h"
#include "CButton.h"
#include "CApp.h"

#include <glog/logging.h>
#include <gflags/gflags.h>

//==============================================================================
class CApp;
class CController : public CEvent
{
private:
    CApp* m_capp;
    SDL_Window *m_screen;
    SDL_Renderer *m_renderer;

    int m_windowW;
    int m_windowH;


public:
    CController();

    CButton m_rewindButton;
    CButton m_playButton;
    CButton m_pauseButton;
    CButton m_stopButton;
    CButton m_ffButton;
    CButton m_cogButton;
    CButton m_switchButton;
    CButton m_posButton;

public:
    bool OnInit(CApp* capp, SDL_Window *screen, SDL_Renderer *renderer);
    bool OnLoad();
    bool OnDraw();
    void OnEvent(SDL_Event *Event);
};

//==============================================================================
#endif
