#include "CGame.h"
#include "CSurface.h"
#include "ECS/Components.h"
#include "Vector2D.h"
#include "Collision.h"
#include "Map.h"

Manager manager;
SDL_Renderer *CGame::renderer;
SDL_Event CGame::event;

std::vector<ColliderComponent *> CGame::colliders;

auto &player(manager.addEntity());
auto &wall(manager.addEntity());

enum groupLabels : std::size_t
{
    groupMap,
    groupPlayers,
    groupEnemies,
    groupColliders

};

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

    Map::LoadMap("images/map_16x16.map", 16, 16);

    player.addComponent<TransformComponent>(2);
    player.addComponent<SpriteComponent>(renderer, "images/player1.png");
    player.addComponent<KeyboardController>();
    player.addComponent<ColliderComponent>("player");
    player.addGroup(groupPlayers);

    wall.addComponent<TransformComponent>(300.0f, 300.0f, 300, 20, 1);
    wall.addComponent<SpriteComponent>(renderer, "images/wall.png");
    wall.addComponent<ColliderComponent>("wall");
    wall.addGroup(groupMap);

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

    for( auto cc : colliders)
    {
        if (player.getComponent<ColliderComponent>().tag != (*cc).tag)
            Collision::AABB(player.getComponent<ColliderComponent>(), *cc);
    }
}
auto& tiles(manager.getGroup(groupMap));
auto& players(manager.getGroup(groupPlayers));
auto& enemies(manager.getGroup(groupEnemies));

void CGame::render()
{
    SDL_RenderClear(renderer);
    for( auto& t : tiles)
        t->draw();
    for( auto& p : players)
        p->draw();
    for( auto& e : enemies)
        e->draw();
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

void CGame::AddTile(int id, int x, int y)
{
    auto& tile(manager.addEntity());
    tile.addComponent<TileComponent>(x, y, 32, 32, id);
    tile.addGroup(groupMap);
}