//==============================================================================
#include "CApp.h"

//==============================================================================
CApp::CApp() {
    Surf_Test = NULL;
    Surf_Display = NULL;
    screen = NULL;
    renderer = NULL;
    tex = NULL;

    Running = true;
}

bool CApp::OnInit()
{
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        return false;
    }

    screen = SDL_CreateWindow("My Game Window",
                          SDL_WINDOWPOS_UNDEFINED,
                          SDL_WINDOWPOS_UNDEFINED,
                          640, 480,
                          SDL_WINDOW_OPENGL);

    renderer = SDL_CreateRenderer(screen, -1, 0);

    if ((Surf_Test = CSurface::OnLoad(screen, "myimage.bmp")) == NULL)
    {
        return false;
    }
	tex = SDL_CreateTextureFromSurface(renderer, Surf_Test);

    return true;
}

//------------------------------------------------------------------------------
int CApp::OnExecute() {
    if(OnInit() == false) {
        return -1;
    }

    SDL_Event Event;

    while(Running) {
        while(SDL_PollEvent(&Event)) {
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
    CSurface::OnDraw(renderer, tex, Surf_Test, X, Y);
}
void CApp::OnLoop()
{
}

void CApp::OnCleanup()
{
    SDL_DestroyTexture(tex);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(screen);
    SDL_FreeSurface(Surf_Test);
    SDL_FreeSurface(Surf_Display);
    SDL_Quit();
}

void CApp::OnExit() {
    Running = false;
}

void CApp::OnKeyDown(SDL_Keycode sym, Uint16 mod, SDL_Scancode scancode)
{
    std::cout << "On KeyDown: " << sym << std::endl;
    switch(sym){
        case(SDLK_RIGHT):
            X++;
            break;
        case(SDLK_LEFT):
            X--;
            break;
        case(SDLK_UP):
            Y--;
            break;
        case(SDLK_DOWN):
            Y++;
            break;
        case(SDLK_ESCAPE):
        case(SDLK_END):
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
    switch(sym){
        case(SDLK_RIGHT):
            std::cout << "RIGHT" << std::endl;
            break;
        case(SDLK_LEFT):
            std::cout << "LEFT" << std::endl;
            break;
        case(SDLK_UP):
            std::cout << "UP" << std::endl;
            break;
        case(SDLK_DOWN):
            std::cout << "DOWN" << std::endl;
            break;
    }
}
void CApp::OnMouseMove(int mX, int mY, int relX, int relY, bool Left, bool Right, bool Middle)
{
    std::cout << "mX:" << mX << "mY:" << mY <<  "relX:" << relX << "relY:" << relY << std::endl;
    X = mX;
    Y = mY;
}
//==============================================================================
int main(int argc, char* argv[]) {
    CApp theApp;

    return theApp.OnExecute();
}

//==============================================================================
