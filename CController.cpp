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
    tRect.x = 0;
    tRect.y = m_windowH - 64;
    tRect.w = 64;
    tRect.h = 64;

    if(!m_playButton.OnInit(m_screen, m_renderer, "Play"))
        return false;
    m_playButton.SetRect(&tRect);

    tRect.x += 64;
    if(!m_stopButton.OnInit(m_screen, m_renderer, "Stop"))
        return false;
    m_stopButton.SetRect(&tRect);

    tRect.x += 64;
    if(!m_play2Button.OnInit(m_screen, m_renderer, "Play2"))
        return false;
    m_play2Button.SetRect(&tRect);

    tRect.x += 64;
    if(!m_stop2Button.OnInit(m_screen, m_renderer, "Stop2"))
        return false;
    m_stop2Button.SetRect(&tRect);

    tRect.x += 64;
    if(!m_cogButton.OnInit(m_screen, m_renderer, "Cog"))
        return false;
    m_cogButton.SetRect(&tRect);
    
    tRect.x += 64;
    if(!m_switchButton.OnInit(m_screen, m_renderer, "Switch"))
        return false;
    m_switchButton.SetRect(&tRect);

    return true;
}

bool CController::OnLoad()
{
    if (!m_playButton.OnLoad("/home/ho80/devel/src/IcoMoon-Free/PNG/64px/019-play.png"))
        return false;
    if (!m_stopButton.OnLoad("/home/ho80/devel/src/IcoMoon-Free/PNG/64px/280-stop.png"))
        return false;
    if (!m_play2Button.OnLoad("/home/ho80/devel/src/IcoMoon-Free/PNG/64px/278-play2.png"))
        return false;
    if (!m_stop2Button.OnLoad("/home/ho80/devel/src/IcoMoon-Free/PNG/64px/287-stop2.png"))
        return false;
    if (!m_cogButton.OnLoad("/home/ho80/devel/src/IcoMoon-Free/PNG/64px/149-cog.png"))
        return false;
    if (!m_switchButton.OnLoad("/home/ho80/devel/src/IcoMoon-Free/PNG/64px/183-switch.png"))
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