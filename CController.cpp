#include "CController.h"

CController::CController()
{
}

bool CController::OnInit(CApp* capp, SDL_Window *screen, SDL_Renderer *renderer)
{
    m_capp = capp;
    m_screen = screen;
    m_renderer = renderer;
    SDL_GetWindowSize(m_screen, &m_windowW, &m_windowH);
    SDL_Rect tRect;
    int pngSize = 100;
    tRect.x = 0;
    tRect.y = m_windowH - pngSize;
    tRect.w = pngSize;
    tRect.h = pngSize;

    if(!m_rewindButton.OnInit(m_screen, m_renderer, "Rewind"))
    {
        LOG(ERROR) << "Button Error Rewind";
        return false;
    }
    m_rewindButton.SetRect(&tRect);

    tRect.x += pngSize;
    if(!m_playButton.OnInit(m_screen, m_renderer, "Play"))
    {
        LOG(ERROR) << "Button Error Play";
        return false;
    }
    m_playButton.SetRect(&tRect);
    m_playButton.SetVisible(false);

    if(!m_pauseButton.OnInit(m_screen, m_renderer, "Pause"))
    {
        LOG(ERROR) << "Button Error Pause";
        return false;
    }
    m_pauseButton.SetRect(&tRect);

    tRect.x += pngSize;
    if(!m_ffButton.OnInit(m_screen, m_renderer, "FastForward"))
    {
        LOG(ERROR) << "Button Error FastForward";
        return false;
    }
    m_ffButton.SetRect(&tRect);

    tRect.x += pngSize;
    if(!m_stopButton.OnInit(m_screen, m_renderer, "Stop"))
    {
        LOG(ERROR) << "Button Error Stop";
        return false;
    }
    m_stopButton.SetRect(&tRect);

    tRect.x += pngSize;
    if(!m_cogButton.OnInit(m_screen, m_renderer, "Cog"))
    {
        LOG(ERROR) << "Button Error Cog";
        return false;
    }
    m_cogButton.SetRect(&tRect);
    
    tRect.x += pngSize;
    if(!m_switchButton.OnInit(m_screen, m_renderer, "Switch"))
    {
        LOG(ERROR) << "Button Error Switch";
        return false;
    }
    m_switchButton.SetRect(&tRect);
    if(!m_posButton.OnInit(m_screen, m_renderer, "Possion"))
    {
        LOG(ERROR) << "Button Error Possion";
        return false;
    }
    tRect.x = 0;
    tRect.y = m_windowH - pngSize*2;
    tRect.w = pngSize;
    tRect.h = pngSize;
    m_posButton.SetRect(&tRect);
    m_posButton.SetMovableX(true);

    return true;
}

bool CController::OnLoad()
{
    if (!m_rewindButton.OnLoad("images/icons8-rewind-100.png"))
        return false;
    if (!m_playButton.OnLoad("images/icons8-play-100.png"))
        return false;
    if (!m_stopButton.OnLoad("images/icons8-stop-100.png"))
        return false;
    if (!m_ffButton.OnLoad("images/icons8-fast-forward-100.png"))
        return false;
    if (!m_pauseButton.OnLoad("images/icons8-pause-100.png"))
        return false;
    if (!m_cogButton.OnLoad("images/icons8-edit-100.png"))
        return false;
    if (!m_switchButton.OnLoad("images/icons8-shutdown-100.png"))
        return false;
    if (!m_posButton.OnLoad("images/icons8-round-100.png"))
        return false;
    return true;
}

bool CController::OnDraw()
{
    m_rewindButton.OnDraw();
    m_playButton.OnDraw();
    m_stopButton.OnDraw();
    m_ffButton.OnDraw();
    m_pauseButton.OnDraw();
    m_cogButton.OnDraw();
    m_switchButton.OnDraw();
    SDL_Rect tRect;
    int pngSize = 100;
    if( m_capp->GetCurPos() > 0 )
        tRect.x = (m_windowW-pngSize) * m_capp->GetCurPos() / 100;
    tRect.y = m_windowH - pngSize*2;
    tRect.w = pngSize;
    tRect.h = pngSize;
    m_posButton.SetRect(&tRect);
    m_posButton.OnDraw();

    return true;
}

void CController::OnEvent(SDL_Event *Event)
{
    m_rewindButton.OnEvent(Event);
    m_playButton.OnEvent(Event);
    m_stopButton.OnEvent(Event);
    m_ffButton.OnEvent(Event);
    m_pauseButton.OnEvent(Event);
    m_cogButton.OnEvent(Event);
    m_switchButton.OnEvent(Event);
    m_posButton.OnEvent(Event);
}