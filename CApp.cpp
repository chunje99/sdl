//==============================================================================
#include "CApp.h"
#include <unistd.h>
#include <chrono>
extern "C"
{
#include <libavutil/imgutils.h>
}

//==============================================================================
CApp::CApp()
{
    Surf_Test = NULL;
    Surf_Display = NULL;
    screen = NULL;
    renderer = NULL;
    tex = NULL;
    videoTexture = NULL;
    m_decoder = NULL;
    pFrameRGB = NULL;
    m_idx = 0;

    Running = true;
}

bool CApp::OnInit()
{
    //openfile
    m_decoder = new CDecode();
    if (m_decoder->Init("SampleVideo_1280x720_20mb.mp4") != 0)
        return false;

    m_decoder->ReadFrame();

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        return false;
    }

    screen = SDL_CreateWindow("My Game Window",
                              SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED,
                              m_decoder->GetVideoCtx()->width,
                              m_decoder->GetVideoCtx()->height,
                              0);

    renderer = SDL_CreateRenderer(screen, -1, 0);

    //if ((Surf_Test = CSurface::OnLoad(screen, "myimage.bmp")) == NULL)
    //{
    //    return false;
    //}
    //tex = SDL_CreateTextureFromSurface(renderer, Surf_Test);


    videoTexture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGB24,
        SDL_TEXTUREACCESS_STREAMING,
        m_decoder->GetVideoCtx()->width,
        m_decoder->GetVideoCtx()->height);

    pFrameRGB = av_frame_alloc();
    if (pFrameRGB == NULL)
        return false;
    uint8_t *buffer = NULL;
    int numBytes;
    // Determine required buffer size and allocate buffer
    //numBytes = avpicture_get_size(AV_PIX_FMT_RGB24, m_decoder->GetVideoCtx()->width,
    //                              m_decoder->GetVideoCtx()->height);
    numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGB24,
                                        m_decoder->GetVideoCtx()->width,
                                        m_decoder->GetVideoCtx()->height,
                                        8);
    buffer = (uint8_t *)av_malloc(numBytes * sizeof(uint8_t));
    //avpicture_fill((AVPicture *)pFrameRGB, buffer, AV_PIX_FMT_RGB24,
    //               m_decoder->GetVideoCtx()->width, m_decoder->GetVideoCtx()->height);
    av_image_fill_arrays(&pFrameRGB->data[0], &pFrameRGB->linesize[0], buffer, AV_PIX_FMT_RGB24,
                                        m_decoder->GetVideoCtx()->width,
                                        m_decoder->GetVideoCtx()->height,
                                        1);
    return true;
}

//------------------------------------------------------------------------------
int CApp::OnExecute()
{
    if (OnInit() == false)
    {
        return -1;
    }

    m_now = std::chrono::system_clock::now();
    SDL_Event Event;
    while (Running)
    {
        while (SDL_PollEvent(&Event))
        {
            OnEvent(&Event);
        }

        OnLoop();
        OnRender();
    }

    OnCleanup();

    return 0;
}

void CApp::OnEvent(SDL_Event *Event)
{
    CEvent::OnEvent(Event);
}

void CApp::OnRender()
{
    //CSurface::OnDraw(renderer, tex, Surf_Test, X, Y);
    CSurface::OnDraw(renderer, videoTexture, X, Y,
                              m_decoder->GetVideoCtx()->width/2,
                              m_decoder->GetVideoCtx()->height/2 );
}
void CApp::OnLoop()
{
    auto now = std::chrono::system_clock::now();
    std::chrono::duration<double> diff = now-m_now;
    if (m_decoder->DecodeFrame(pFrameRGB, diff.count()) == 0)
    {
        SDL_UpdateTexture(videoTexture, NULL, pFrameRGB->data[0], pFrameRGB->linesize[0]);
        //SDL_UpdateTexture(tex, NULL, pFrameRGB->data[0], pFrameRGB->linesize[0]);
        double pts = av_frame_get_best_effort_timestamp(pFrameRGB);
        std::cout << "PTS:" << pts << std::endl;
        usleep(100);
    }
}

void CApp::OnCleanup()
{
    SDL_DestroyTexture(tex);
    SDL_DestroyTexture(videoTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(screen);
    SDL_FreeSurface(Surf_Test);
    SDL_FreeSurface(Surf_Display);
    if (m_decoder)
    {
        delete (m_decoder);
        m_decoder = NULL;
    }
    SDL_Quit();
}

void CApp::OnExit()
{
    Running = false;
}

void CApp::OnKeyDown(SDL_Keycode sym, Uint16 mod, SDL_Scancode scancode)
{
    std::cout << "On KeyDown: " << sym << std::endl;
    switch (sym)
    {
    case (SDLK_RIGHT):
        X++;
        break;
    case (SDLK_LEFT):
        X--;
        break;
    case (SDLK_UP):
        Y--;
        break;
    case (SDLK_DOWN):
        Y++;
        break;
    case (SDLK_ESCAPE):
    case (SDLK_END):
        OnExit();
    }
    if (X < 0)
        X = 0;
    if (Y < 0)
        Y = 0;
}

void CApp::OnKeyUp(SDL_Keycode sym, Uint16 mod, SDL_Scancode scancode)
{
    std::cout << "On KeyUp: " << sym << std::endl;
    switch (sym)
    {
    case (SDLK_RIGHT):
        std::cout << "RIGHT" << std::endl;
        break;
    case (SDLK_LEFT):
        std::cout << "LEFT" << std::endl;
        break;
    case (SDLK_UP):
        std::cout << "UP" << std::endl;
        break;
    case (SDLK_DOWN):
        std::cout << "DOWN" << std::endl;
        break;
    }
}
void CApp::OnMouseMove(int mX, int mY, int relX, int relY, bool Left, bool Right, bool Middle)
{
    std::cout << "mX:" << mX << "mY:" << mY << "relX:" << relX << "relY:" << relY << std::endl;
    X = mX;
    Y = mY;
}
//==============================================================================
int main(int argc, char *argv[])
{
    CApp theApp;

    return theApp.OnExecute();
}

//==============================================================================
