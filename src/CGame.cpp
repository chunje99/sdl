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


const char* mapFile = "images/map_tile.png";

enum groupLabels : std::size_t
{
    groupMap,
    groupPlayers,
    groupEnemies,
    groupColliders

};

auto& tiles(manager.getGroup(groupMap));
auto& players(manager.getGroup(groupPlayers));
auto& enemies(manager.getGroup(groupEnemies));

bool CGame::isRunning = false;

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

    Map::LoadMap("images/map_16x16.map", 16, 16);

    player.addComponent<TransformComponent>(2);
    //player.addComponent<SpriteComponent>(renderer, "images/player1.png");
    player.addComponent<SpriteComponent>(renderer, "images/player_ani.png", true);
    player.addComponent<KeyboardController>();
    player.addComponent<ColliderComponent>("player");
    player.addGroup(groupPlayers);

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

    Vector2D pVec = player.getComponent<TransformComponent>().velocity;
    int pSpeed = player.getComponent<TransformComponent>().speed;

    for( auto t : tiles )
    {
        t->getComponent<TileComponent>().destRect.x += -(pVec.x * pSpeed);
        t->getComponent<TileComponent>().destRect.y += -(pVec.y * pSpeed);
    }
}

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

void CGame::AddTile(int srcX, int srcY, int xpos, int ypos)
{
    auto& tile(manager.addEntity());
    tile.addComponent<TileComponent>(srcX, srcY, xpos, ypos, mapFile);
    tile.addGroup(groupMap);
}