//==============================================================================
// SDL Tutorial 1
//==============================================================================
#ifndef _CAPP_H_
#define _CAPP_H_

#include <iostream>
#include <SDL.h>
#include <SDL_ttf.h>

#include "CSurface.h"
#include "CEvent.h"
#include "Decode.h"
#include "CText.h"
#include "CController.h"
#include <glog/logging.h>
#include <gflags/gflags.h>

#define WINDOW_W 1280
#define WINDOW_H 720
#define ICON_W 100
#define ICON_W 100


//==============================================================================
class CDecode;
class CController;
class CApp : public CEvent
{
private:

    SDL_Event m_event;
    SDL_Surface *Surf_Display;
    SDL_Surface *Surf_Test;

    SDL_Window *screen;
    SDL_Renderer *renderer;
    SDL_Texture *tex;
    SDL_Texture *videoTexture;

    CText* m_ctext;
    CController* m_controller;
    int X;
    int Y;
    CDecode* m_decoder;
    AVFrame* pFrameRGB;
    int m_idx;
    double m_curPos;
    SDL_AudioSpec wantedSpec;
    SDL_AudioSpec audioSpec;
    std::string m_fileName;
    bool m_play;

public:
    bool Running;
    CApp();

    int OnExecute();
    void SetFileName(const char* fileName);

public:
    bool OnInit();

    void OnEvent(SDL_Event *Event);

    void OnLoop();

    void OnRender();

    void OnCleanup();

    void OnExit();

    void OnKeyUp(SDL_Keycode sym, Uint16 mod, SDL_Scancode scancode);

    void OnUser(Uint8 type, int code, void *data1, void *data2);

    void OnPlayClick();

    void OnPauseClick();

    void OnStopClick();

    void OnRewindClick();

    void OnPositionClick(SDL_Rect rect);

    void OnFFClick();

    void OnVolumeUp();

    void OnVolumeDown();

    double GetCurPos();

public:
    void OnAudioCallback(Uint8 *stream, int len);
    static void AudioCallback(void *userdata, uint8_t * stream, int len);
};
    //==============================================================================

#endif
