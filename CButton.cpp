#include "CButton.h"

CButton::CButton() : m_screen(NULL),
                     m_renderer(NULL),
                     m_texture(NULL),
                     m_isMouseOver(false),
                     m_isLDown(false),
                     m_movableX(false),
                     m_movableY(false),
                     m_movableW(false),
                     m_movableH(false),
                     m_isVisible(true)
{
    m_rect.x = 0;
    m_rect.y = 0;
    m_rect.w = 0;
    m_rect.h = 0;
}

bool CButton::OnInit(SDL_Window* screen, SDL_Renderer* renderer, const char* name)
{
    m_screen = screen;
    m_renderer = renderer;
    m_name = name;
    return true;
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
    if(!m_isVisible)
        return true;
    CSurface::OnDraw(m_renderer, m_texture,
                    m_rect.x,
                    m_rect.y, m_rect.w, m_rect.h);
    return true;
}

void CButton::SetRect(SDL_Rect* rect)
{
    if(m_isLDown)
        return;
    bool first = false;
    if (m_rect.x == 0 &&
        m_rect.y == 0 &&
        m_rect.w == 0 &&
        m_rect.h == 0)
        first = true;
    if (first || m_movableX)
        m_rect.x = rect->x;
    if (first || m_movableY)
        m_rect.y = rect->y;
    if (first || m_movableW)
        m_rect.w = rect->w;
    if (first || m_movableH)
        m_rect.h = rect->h;
}

void CButton::OnEvent(SDL_Event *Event)
{
    if(!m_isVisible)
        return ;
    CEvent::OnEvent(Event);
}

void CButton::OnMouseMove(int mX, int mY, int relX, int relY, bool Left, bool Right, bool Middle)
{
    if (mX >= m_rect.x && mX <= m_rect.x + m_rect.w &&
        mY >= m_rect.y && mY <= m_rect.y + m_rect.w)
        OnMouseOver();
    else
        OnMouseOut();
    if(m_isLDown && m_movableX)
        m_rect.x = mX;
    if(m_isLDown && m_movableY)
        m_rect.y = mY;
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
        m_isLDown = true;
    }
}

void CButton::OnLButtonUp(int mX, int mY)
{
    if (mX >= m_rect.x && mX <= m_rect.x + m_rect.w &&
        mY >= m_rect.y && mY <= m_rect.y + m_rect.w && m_isLDown)
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
    m_isLDown = false;
}

void CButton::SetMovableX(bool tf)
{
    m_movableX = tf;
}
void CButton::SetMovableY(bool tf)
{
    m_movableY = tf;
}
void CButton::SetMovableW(bool tf)
{
    m_movableW = tf;
}
void CButton::SetMovableH(bool tf)
{
    m_movableH = tf;
}
void CButton::SetVisible(bool tf)
{
    m_isVisible = tf;
}