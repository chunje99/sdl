#ifndef SPRITE_COMPONENTS_H
#define SPRITE_COMPONENTS_H
#include "Components.h"
#include "SDL.h"
#include "CSurface.h"

class SpriteComponent : public Component
{
private:
    PositionComponent * position;
    SDL_Renderer * renderer;
    SDL_Texture* texture;
    SDL_Rect srcRect, destRect;

public:
    SpriteComponent() = default;
    SpriteComponent(SDL_Renderer* rend, const char* path)
    {
        renderer = rend;
        texture = CSurface::Load(renderer, path);
    }

    void init() override
    {
        position = &entity->getComponent<PositionComponent>();

        srcRect.x = srcRect.y = 0;
        srcRect.w = srcRect.h = 32;
        destRect.w = destRect.h = 64;
    }
    void update() override
    {
        destRect.x = position->x() / 100;
        destRect.y = position->y() / 100;
    }
    void draw() override
    {
        CSurface::Draw(renderer, texture, srcRect, destRect);
    }
};
#endif