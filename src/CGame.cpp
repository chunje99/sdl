#include "CGame.h"
#include "CSurface.h"
#include "ECS/Components.h"
#include "Vector2D.h"
#include "Collision.h"

Manager manager;
SDL_Renderer *CGame::renderer;
SDL_Event CGame::event;

std::vector<ColliderComponent *> CGame::colliders;

auto &player(manager.addEntity());
auto &wall(manager.addEntity());

auto &tile0(manager.addEntity());
auto &tile1(manager.addEntity());
auto &tile2(manager.addEntity());

CGame::CGame() : isRunning(false) {}

CGame::~CGame() {}

void CGame::init(const char *titl, int width, int height, bool fullscreen)
{
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        LOG(ERROR) << "SDL_Init Error";
        return;
    }

    int flags = 0;
    if (fullscreen)
        flags = SDL_WINDOW_FULLSCREEN;
    window = SDL_CreateWindow(titl,
                              SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED,
                              width,
                              height,
                              flags);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    player.addComponent<TransformComponent>(2);
    player.addComponent<SpriteComponent>(renderer, "images/player1.png");
    player.addComponent<KeyboardController>();
    player.addComponent<ColliderComponent>("player");

    wall.addComponent<TransformComponent>(300.0f, 300.0f, 300, 20, 1);
    wall.addComponent<SpriteComponent>(renderer, "images/wall.png");
    wall.addComponent<ColliderComponent>("wall");

    tile0.addComponent<TileComponent>(200, 200, 32, 32, 0);
    tile1.addComponent<TileComponent>(250, 250, 32, 32, 1);
    tile1.addComponent<ColliderComponent>("dirt");
    tile2.addComponent<TileComponent>(150, 150, 32, 32, 2);
    tile2.addComponent<ColliderComponent>("grass");

    //manager.init();

    isRunning = true;
}

void CGame::handleEvents()
{
    if (SDL_PollEvent(&event))
    {
        OnEvent(&event);
    }
}
void CGame::update()
{
    cnt++;
    manager.refresh();
    manager.update();
    //player.getComponent<TransformComponent>().position += Vector2D(1,1);
    //if (Collision::AABB(player.getComponent<ColliderComponent>().collider,
    //                    wall.getComponent<ColliderComponent>().collider))
    //{
    //    player.getComponent<TransformComponent>().scale = 1;
    //    player.getComponent<TransformComponent>().velocity *= -1;
    //    LOG(INFO) << "Wall Hit";
    //}

    for( auto cc : colliders)
    {
        if (player.getComponent<ColliderComponent>().tag != (*cc).tag)
            Collision::AABB(player.getComponent<ColliderComponent>(), *cc);
    }
}
void CGame::render()
{
    SDL_RenderClear(renderer);
    manager.draw();
    SDL_RenderPresent(renderer);
    LOG(INFO) << "CNT: " << cnt;
}
void CGame::clean()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
}

void CGame::OnEvent(SDL_Event *Event)
{
    CEvent::OnEvent(Event);
}

void CGame::OnExit()
{
    isRunning = false;
}