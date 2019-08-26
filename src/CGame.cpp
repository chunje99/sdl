#include "CGame.h"
#include "CSurface.h"
#include "ECS/Components.h"
#include "Vector2D.h"
#include "Collision.h"
#include "Map.h"
#include "AssetManager.h"

Map* map;
Manager manager;
SDL_Renderer *CGame::renderer;
SDL_Event CGame::event;

bool CGame::isRunning = false;

SDL_Rect CGame::camera = {0,0,800,640};

AssetManager* CGame::assets = new AssetManager(&manager);

auto &player(manager.addEntity());

CGame::CGame() {}

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

    assets->AddTexture("terrain", "images/map_tile.png");
    assets->AddTexture("player", "images/player_ani.png");
    assets->AddTexture("projectile", "images/projectile.png");

    //map = new Map("images/map_tile.png", 3, 32);
    map = new Map("terrain", 2, 32);
    map->LoadMap("images/map_16x16.map", 16, 16);

    player.addComponent<TransformComponent>(400, 320, 32, 32, 4);
    player.addComponent<SpriteComponent>("player", true);
    player.addComponent<KeyboardController>();
    player.addComponent<ColliderComponent>("player");
    player.addGroup(groupPlayers);

    assets->CreateProjectile(Vector2D(350, 320), Vector2D(2, 0), 200, 2, "projectile");

    //manager.init();

    isRunning = true;
}

auto& tiles(manager.getGroup(CGame::groupMap));
auto& players(manager.getGroup(CGame::groupPlayers));
auto& colliders(manager.getGroup(CGame::groupColliders));
auto& projectiles(manager.getGroup(CGame::groupProjectiles));


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
    SDL_Rect playerCol = player.getComponent<ColliderComponent>().collider;
    Vector2D playerPos = player.getComponent<TransformComponent>().position;

    manager.refresh();
    manager.update();

    for( auto& c : colliders)
    {
        SDL_Rect cCol = c->getComponent<ColliderComponent>().collider;
        if(Collision::AABB(cCol, playerCol))
        {
            player.getComponent<TransformComponent>().position = playerPos;
        }
    }

    for( auto& p : projectiles)
    {
        if(Collision::AABB(player.getComponent<ColliderComponent>().collider, p->getComponent<ColliderComponent>().collider))
        {
            LOG(INFO) << "Hit player!";
            p->destroy();
        }
    }

    camera.x = player.getComponent<TransformComponent>().position.x - 400;
    camera.y = player.getComponent<TransformComponent>().position.y - 320;

    if (camera.x < 0)
        camera.x = 0;
    if (camera.y < 0)
        camera.y = 0;
    if (camera.x > camera.w)
        camera.x = camera.w;
    if (camera.y > camera.h)
        camera.y = camera.h;
}
void CGame::render()
{
    SDL_RenderClear(renderer);
    for( auto& t : tiles)
        t->draw();
    for( auto& p : players)
        p->draw();
    for( auto& p : projectiles)
        p->draw();
    SDL_RenderPresent(renderer);
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