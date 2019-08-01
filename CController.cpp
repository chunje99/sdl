#include "CController.h"

CController::CController()
{
}

bool CController::OnInit(SDL_Window *screen, SDL_Renderer *renderer)
{
    m_screen = screen;
    m_renderer = renderer;
    SDL_GetWindowSize(m_screen, &m_windowW, &m_windowH);
    SDL_Rect tRect;
    int pngSize = 100;
    tRect.x = 0;
    tRect.y = m_windowH - pngSize;
    tRect.w = pngSize;
    tRect.h = pngSize;

    if(!m_playButton.OnInit(m_screen, m_renderer, "Play"))
    {
        LOG(ERROR) << "Button Error Play";
        return false;
    }
    m_playButton.SetRect(&tRect);

    tRect.x += pngSize;
    if(!m_stopButton.OnInit(m_screen, m_renderer, "Stop"))
    {
        LOG(ERROR) << "Button Error Stop";
        return false;
    }
    m_stopButton.SetRect(&tRect);

    tRect.x += pngSize;
    if(!m_play2Button.OnInit(m_screen, m_renderer, "Play2"))
    {
        LOG(ERROR) << "Button Error Play2";
        return false;
    }
    m_play2Button.SetRect(&tRect);

    tRect.x += pngSize;
    if(!m_stop2Button.OnInit(m_screen, m_renderer, "Stop2"))
    {
        LOG(ERROR) << "Button Error Stop2";
        return false;
    }
    m_stop2Button.SetRect(&tRect);

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

    return true;
}

bool CController::OnLoad()
{
    if (!m_playButton.OnLoad("images/icons8-play-100.png"))
        return false;
    if (!m_stopButton.OnLoad("images/icons8-stop-100.png"))
        return false;
    if (!m_play2Button.OnLoad("images/icons8-play-100.png"))
        return false;
    if (!m_stop2Button.OnLoad("images/icons8-stop-100.png"))
        return false;
    if (!m_cogButton.OnLoad("images/icons8-edit-100.png"))
        return false;
    if (!m_switchButton.OnLoad("images/icons8-shutdown-100.png"))
        return false;
    return true;
}

bool CController::OnDraw()
{
    m_playButton.OnDraw();
    m_stopButton.OnDraw();
    m_play2Button.OnDraw();
    m_stop2Button.OnDraw();
    m_cogButton.OnDraw();
    m_switchButton.OnDraw();

    return true;
}

void CController::OnEvent(SDL_Event *Event)
{
    m_playButton.OnEvent(Event);
    m_stopButton.OnEvent(Event);
    m_play2Button.OnEvent(Event);
    m_stop2Button.OnEvent(Event);
    m_cogButton.OnEvent(Event);
    m_switchButton.OnEvent(Event);
}