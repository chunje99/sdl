#include "CButton.h"

CButton::CButton() : m_screen(NULL),
                     m_renderer(NULL),
                     m_texture(NULL),
                     m_isMouseOver(false)
{
}

bool CButton::OnInit(SDL_Window* screen, SDL_Renderer* renderer, const char* name)
{
    m_screen = screen;
    m_renderer = renderer;
    m_name = name;
}

bool CButton::OnLoad(char* filePath)
{
    SDL_Surface* tmp_surface = NULL;
    if ((tmp_surface = CSurface::OnLoad(m_screen, m_renderer, filePath)) == NULL)
    {
        return false;
    }
    m_texture = SDL_CreateTextureFromSurface(m_renderer, tmp_surface);
	SDL_FreeSurface(tmp_surface);
    return true;
}

bool CButton::OnDraw()
{
    CSurface::OnDraw(m_renderer, m_texture,
                    m_rect.x,
                    m_rect.y, m_rect.w, m_rect.h);
    return true;
}

void CButton::SetRect(SDL_Rect* rect)
{
    m_rect.x = rect->x;
    m_rect.y = rect->y;
    m_rect.w = rect->w;
    m_rect.h = rect->h;
}

void CButton::OnEvent(SDL_Event *Event)
{
    CEvent::OnEvent(Event);
}

void CButton::OnMouseMove(int mX, int mY, int relX, int relY, bool Left, bool Right, bool Middle)
{
    DLOG(INFO) << "CButton::   mX:" << mX << "mY:" << mY << "relX:" << relX << "relY:" << relY;
    if (mX >= m_rect.x && mX <= m_rect.x + m_rect.w &&
        mY >= m_rect.y && mY <= m_rect.y + m_rect.w)
        OnMouseOver();
    else
        OnMouseOut();
}

void CButton::OnMouseOver()
{
    if(!m_isMouseOver)
    {
        DLOG(INFO) << "OnMouseOver:" << m_name;
        m_isMouseOver = true;
        SDL_Event user_event;

        user_event.type = SDL_USEREVENT;
        user_event.user.type = USER_EVNET_TYPE_BUTTON;
        user_event.user.code = USER_EVNET_CODE_MOUSEOVER;
        user_event.user.data1 = this;
        user_event.user.data2 = NULL;
        SDL_PushEvent(&user_event);
    }
}
void CButton::OnMouseOut()
{
    if(m_isMouseOver)
    {
        DLOG(INFO) << "OnMouseOut:" << m_name;
        m_isMouseOver = false;
        SDL_Event user_event;

        user_event.type = SDL_USEREVENT;
        user_event.user.type = USER_EVNET_TYPE_BUTTON;
        user_event.user.code = USER_EVNET_CODE_MOUSEOUT;
        user_event.user.data1 = this;
        user_event.user.data2 = NULL;
        SDL_PushEvent(&user_event);
    }
}

void CButton::OnLButtonDown(int mX, int mY)
{
    if (mX >= m_rect.x && mX <= m_rect.x + m_rect.w &&
        mY >= m_rect.y && mY <= m_rect.y + m_rect.w)
    {
        DLOG(INFO) << "OnMouseClick:" << m_name;
        try
        {
            SDL_Event user_event;

            user_event.type = SDL_USEREVENT;
            user_event.user.type = USER_EVNET_TYPE_BUTTON;
            user_event.user.code = USER_EVNET_CODE_LCLICK;
            user_event.user.data1 = this;
            user_event.user.data2 = NULL;
            SDL_PushEvent(&user_event);
        }
        catch(...)
        {
            LOG(ERROR) << "OnLButtonDown Event Undefined:" << m_name;
        }
    }
}