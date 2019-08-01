//==============================================================================
#include "CApp.h"
#include <unistd.h>
#include <chrono>
#include "config.h"
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
    wantedSpec = {0};
    audioSpec = {0};
    m_controller = NULL;
    m_play = true;

    Running = true;
}

bool CApp::OnInit()
{
    //openfile
    m_decoder = new CDecode();
    if (m_decoder->Init(this, m_fileName.c_str()) != 0)
    {
        LOG(ERROR) << "Decoder Error";
        return false;
    }

    //m_decoder->ReadFrame();

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        LOG(ERROR) << "SDL_Init Error";
        return false;
    }

    screen = SDL_CreateWindow("My Window",
                              SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED,
                              m_decoder->GetVideoCtx()->width,
                              m_decoder->GetVideoCtx()->height,
                              0);

    renderer = SDL_CreateRenderer(screen, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags))
    {
        LOG(ERROR) << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError();
    }

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
    ///init audio
    if (m_decoder->GetAudioCtx() != NULL)
    {
        wantedSpec.channels = m_decoder->GetAudioCtx()->channels;
        wantedSpec.freq = m_decoder->GetAudioCtx()->sample_rate;
        wantedSpec.format = AUDIO_F32;
        wantedSpec.silence = 0;
        wantedSpec.samples = SDL_AUDIO_BUFFER_SIZE;
        //wantedSpec.userdata = m_decoder->GetAudioCtx();
        wantedSpec.userdata = this;
        wantedSpec.callback = CApp::AudioCallback;

        if (SDL_OpenAudio(&wantedSpec, &audioSpec) < 0)
        {
            LOG(ERROR) << "SDL_OpenAudio: " << SDL_GetError();
            return false;
        }

        SDL_PauseAudio(0);
    }

    m_controller = new CController();
    if(!(m_controller->OnInit(screen, renderer)))
    {
        LOG(ERROR) << "Controller Init Error";
        return false;
    }
    if(!m_controller->OnLoad())
    {
        LOG(ERROR) << "Controller Load Error";
        return false;
    }
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
    while (Running)
    {
        while (SDL_PollEvent(&m_event))
        {
            OnEvent(&m_event);
        }

        OnLoop();
        OnRender();
        usleep(100);
    }

    OnCleanup();

    return 0;
}

void CApp::OnEvent(SDL_Event *Event)
{
    CEvent::OnEvent(Event);
    m_controller->OnEvent(Event);
    m_decoder->OnEvent(Event);
}

void CApp::OnRender()
{
    SDL_RenderClear(renderer);
    CSurface::OnDrawPlayer(renderer, videoTexture, 0, 0,
                              m_decoder->GetVideoCtx()->width/2,
                              m_decoder->GetVideoCtx()->height/2 );
    m_controller->OnDraw();
    SDL_RenderPresent(renderer);
}
void CApp::OnLoop()
{
    auto now = std::chrono::system_clock::now();
    std::chrono::duration<double> diff = now-m_now;
    m_now = now;
    if(!m_play)
    {
        return;
    }
    m_playTime += diff;
    //double diff2 = now-m_now;
    if (m_decoder->DecodeFrame(pFrameRGB, m_playTime.count()) == 0)
    {
        SDL_UpdateTexture(videoTexture, NULL, pFrameRGB->data[0], pFrameRGB->linesize[0]);
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

void CApp::OnAudioCallback(Uint8 *stream, int len)
{
    if(m_play && Running )
        m_decoder->onCallback(stream, len);
    else
        memset(stream, 0, len);
}

void CApp::AudioCallback(void *userdata, uint8_t * stream, int len)
{
    static_cast<CApp*>(userdata)->OnAudioCallback(stream, len);
}
//==============================================================================
void CApp::SetFileName(const char *fileName)
{
    m_fileName = fileName;
}

void CApp::OnPlayClick()
{
    LOG(INFO) << "OnPlayClick";
    if(m_play)
        m_play = false;
    else
        m_play = true;
}

void CApp::OnUser(Uint8 type, int code, void *data1, void *data2)
{

    switch (type)
    {
    case USER_EVNET_TYPE_BUTTON:
    {
        if (data1 == NULL)
            break;
        CButton* button = (CButton*)data1;
        switch (code)
        {
        case USER_EVNET_CODE_LCLICK:
            DLOG(INFO) << "Button LClick:" << button->GetName();
            if(button->GetName() == "Play")
                OnPlayClick();
            else if(button->GetName() == "Switch")
                OnExit();
            break;
        case USER_EVNET_CODE_MOUSEOUT:
            LOG(INFO) << "Mouse Out:" << button->GetName();
            break;
        case USER_EVNET_CODE_MOUSEOVER:
            LOG(INFO) << "Mouse Over:" << button->GetName();
            break;
        }
        break;
    }
    }
}

DEFINE_string(file, "", "video file name");
int main(int argc, char *argv[])
{
    google::InitGoogleLogging(argv[0]);
#if defined OS_LINUX
    gflags::ParseCommandLineFlags(&argc, &argv, true);
#elif defined OS_MAC
    google::ParseCommandLineFlags(&argc, &argv, true);
#endif
    LOG(INFO) << "FileName = " << FLAGS_file;

    CApp theApp;
    theApp.SetFileName(FLAGS_file.c_str());

    return theApp.OnExecute();
}

//==============================================================================
