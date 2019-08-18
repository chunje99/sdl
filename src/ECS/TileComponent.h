#ifndef TILE_COMPONENT_H
#define TILE_COMPONENT_H

#include "ECS.h"
#include "TransformComponent.h"
#include "SDL.h"
#include "../CGame.h"
#include "../AssetManager.h"

class TileComponent : public Component
{
public:
    SDL_Texture* texture;
    SDL_Rect srcRect, destRect;
    Vector2D position;

    TileComponent() = default;
    ~TileComponent()
    {
        SDL_DestroyTexture(texture);
    }

    TileComponent(int srcX, int srcY, int xpos, int ypos, int tsize, int tscale, std::string tID)
    {
        texture = CGame::assets->GetTexture(tID);

        position.x = xpos;
        position.y = ypos;

        srcRect.x = srcX;
        srcRect.y = srcY;
        srcRect.w = srcRect.h = tsize;

        destRect.x = xpos;
        destRect.y = ypos;
        destRect.w = destRect.h = tsize * tscale;
    }

    void update() override
    {
        destRect.x = position.x - CGame::camera.x;
        destRect.y = position.y - CGame::camera.y;
    }

    void draw() override
    {
        CSurface::Draw(CGame::renderer, texture, srcRect, destRect, SDL_FLIP_NONE);
    }
};

#endif