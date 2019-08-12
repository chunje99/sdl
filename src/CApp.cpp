//==============================================================================
#include "CApp.h"
#include <unistd.h>
#include <chrono>
#include "ECS/Components.h"
extern "C"
{
#include <libavutil/imgutils.h>
}

Manager manager;
auto& player(manager.addEntity());
auto& enemy(manager.addEntity());

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
    m_curPos = 0;
    m_ctext = NULL;
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
                              WINDOW_W,
                              WINDOW_H,
                              0);

    renderer = SDL_CreateRenderer(screen, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    //Open the font
    if (TTF_Init() < 0)
    {
        LOG(ERROR) << "TTF_Init Error";
        return false;
    }
    m_ctext = new CText();
    m_ctext->OnInit(screen, renderer, "HELLO");
    SDL_Rect tmpRect;
    tmpRect.x = WINDOW_W - 200;
    tmpRect.y = WINDOW_H - 100;
    tmpRect.w = 200;
    tmpRect.h = 100;
    m_ctext->SetRect(tmpRect);

    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags))
    {
        LOG(ERROR) << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError();
    }

    videoTexture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGB24,
        SDL_TEXTUREACCESS_STREAMING,
        WINDOW_W,
        WINDOW_H);
        //m_decoder->GetVideoCtx()->width/2,
        //m_decoder->GetVideoCtx()->height/2);

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
    if(!(m_controller->OnInit(this, screen, renderer)))
    {
        LOG(ERROR) << "Controller Init Error";
        return false;
    }
    if(!m_controller->OnLoad())
    {
        LOG(ERROR) << "Controller Load Error";
        return false;
    }

    player.addComponent<TransformComponent>();
    player.addComponent<SpriteComponent>(renderer, "images/player1.png");
    enemy.addComponent<TransformComponent>(5000,5000);
    enemy.addComponent<SpriteComponent>(renderer, "images/player1.png");

    manager.init();

    return true;
}

//------------------------------------------------------------------------------
int CApp::OnExecute()
{
    if (OnInit() == false)
    {
        return -1;
    }

    while (Running)
    {
        while (SDL_PollEvent(&m_event))
        {
            OnEvent(&m_event);
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
    m_controller->OnEvent(Event);
    m_decoder->OnEvent(Event);
}

void CApp::OnRender()
{
    SDL_RenderClear(renderer);
    CSurface::OnDrawPlayer(renderer, videoTexture, 0, 0,
                            WINDOW_W,
                            WINDOW_H);
                            // m_decoder->GetVideoCtx()->width/2,
                            // m_decoder->GetVideoCtx()->height/2 );
    m_controller->OnDraw();
    m_ctext->OnDraw();
    manager.draw();
    //player.getComponent<SpriteComponent>().draw();
    //LOG(INFO) << newPlayer.getComponent<PositionComponent>().x() << "," << 
    //            newPlayer.getComponent<PositionComponent>().y();
    SDL_RenderPresent(renderer);
}
void CApp::OnLoop()
{
    if(!m_play)
    {
        return;
    }
    //if (m_decoder->DecodeFrame(pFrameRGB) == 0)
    AVFrame* frame;
    if ((frame = m_decoder->GetFrame()) != NULL)
    {
        SDL_UpdateTexture(videoTexture, NULL, frame->data[0], frame->linesize[0]);
        av_frame_free(&frame);
    }
    player.getComponent<TransformComponent>().position += Vector2D(1,1);
    manager.update();
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
        delete m_decoder;
        m_decoder = NULL;
    }
    if( m_controller)
    {
        delete m_controller;
        m_controller = NULL;
    }
    if( m_ctext)
    {
        delete m_ctext;
        m_ctext = NULL;
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

void CApp::OnKeyUp(SDL_Keycode sym, Uint16 mod, SDL_Scancode scancode)
{
    if( sym == SDLK_UP )
    {
        OnVolumeUp();
    }
    else if( sym == SDLK_DOWN )
    {
        OnVolumeDown();
    }
    else if( sym == SDLK_LEFT)
    {
        OnRewindClick();
    }
    else if( sym == SDLK_RIGHT)
    {
        OnFFClick();
    }
    else if( sym == SDLK_SPACE)
    {
        if(m_play)
            OnPauseClick();
        else
            OnPlayClick();
    }
}

void CApp::OnPlayClick()
{
    LOG(INFO) << "OnPlayClick";
    m_play = true;
    m_controller->m_playButton.SetVisible(false);
    m_controller->m_pauseButton.SetVisible(true);
}
void CApp::OnStopClick()
{
    LOG(INFO) << "OnStopClick";
    OnExit();
}
void CApp::OnPauseClick()
{
    LOG(INFO) << "OnPauseClick";
    m_play = false;
    m_controller->m_playButton.SetVisible(true);
    m_controller->m_pauseButton.SetVisible(false);
}

void CApp::OnRewindClick()
{
    LOG(INFO) << "OnRewindClick";
    m_decoder->Seek(-10.0, AVSEEK_FLAG_BACKWARD);
}

void CApp::OnFFClick()
{
    LOG(INFO) << "OnFFClick";
    m_decoder->Seek(10.0, 0);
}
void CApp::OnVolumeUp()
{
    LOG(INFO) << "OnVolumeUp";
    m_decoder->SetVolume(m_decoder->GetVolume() + 10);
}
void CApp::OnVolumeDown()
{
    LOG(INFO) << "OnVolumeDown";
    m_decoder->SetVolume(m_decoder->GetVolume() - 10);
}

void CApp::OnPositionClick(SDL_Rect rect)
{
    LOG(INFO) << "OnPositionClick";
    //시간 계산
    //double posSec = (rect.x+(rect.w/2))* (m_decoder->GetDuraion()/1000000)/((m_decoder->GetVideoCtx()->width / 2) - 100) ;
    double posSec = (rect.x+(rect.w/2))* (m_decoder->GetDuraion()/1000000)/((WINDOW_W) - ICON_W) ;
    LOG(INFO) << "Seek Time: " << posSec;
    posSec = posSec - (m_decoder->get_audio_clock() - m_decoder->GetBasePTS()/1000000);
    LOG(INFO) << "Seek Time: " << posSec;
    int flags = 0;
    if(posSec < 0 )
    {
        flags = AVSEEK_FLAG_BACKWARD;
    }
    m_decoder->Seek(posSec, flags);
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
            if(button->GetName() == "Pause")
                OnPauseClick();
            if(button->GetName() == "Stop")
                OnStopClick();
            if(button->GetName() == "Rewind")
                OnRewindClick();
            if(button->GetName() == "FastForward")
                OnFFClick();
            if(button->GetName() == "Position")
                OnPositionClick(button->GetRect());
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

double CApp::GetCurPos()
{
    if(m_decoder->GetDuraion() <= 0)
        return 0L;
    double curTime = m_decoder->get_audio_clock()*1000000 - m_decoder->GetBasePTS();
    std::string timeStr = std::to_string((int)(curTime/1000000));
    timeStr += " / ";
    timeStr += std::to_string((int)(m_decoder->GetDuraion()/1000000));
    m_ctext->SetText(timeStr.c_str());

    return  curTime / m_decoder->GetDuraion();
}
//==============================================================================
