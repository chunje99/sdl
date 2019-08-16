#ifndef TILE_COMPONENT_H
#define TILE_COMPONENT_H

#include "ECS.h"
#include "TransformComponent.h"
#include "SDL.h"

class TileComponent : public Component
{
public:
    SDL_Texture* texture;
    SDL_Rect srcRect, destRect;

    TileComponent() = default;
    ~TileComponent()
    {
        SDL_DestroyTexture(texture);
    }

    TileComponent(int srcX, int srcY, int xpos, int ypos, const char* path)
    {
        texture = CSurface::Load(CGame::renderer, path);

        srcRect.x = srcX;
        srcRect.y = srcY;
        srcRect.w = srcRect.h = 32;

        destRect.x = xpos;
        destRect.y = ypos;
        destRect.w = destRect.h = 32;
    }

    void draw() override
    {
        CSurface::Draw(CGame::renderer, texture, srcRect, destRect, SDL_FLIP_NONE);
    }
};

#endif