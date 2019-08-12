#ifndef SPRITE_COMPONENTS_H
#define SPRITE_COMPONENTS_H
#include "Components.h"
#include "SDL.h"
#include "CSurface.h"

class SpriteComponent : public Component
{
private:
    TransformComponent *transform;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    SDL_Rect srcRect, destRect;

public:
    SpriteComponent() = default;
    SpriteComponent(SDL_Renderer *rend, const char *path)
    {
        renderer = rend;
        setTex(path);
        transform = NULL;
    }
    ~SpriteComponent()
    {
        SDL_DestroyTexture(texture);
    }

    void setTex( const char* path)
    {
        texture = CSurface::Load(renderer, path);
    }

    void init() override
    {
        if (entity->hasComponent<TransformComponent>())
            transform = &entity->getComponent<TransformComponent>();

        srcRect.x = srcRect.y = 0;
        srcRect.w = transform->width;
        srcRect.h = transform->height;
    }
    void update() override
    {
        if (entity->hasComponent<TransformComponent>())
        {
            destRect.x = static_cast<int>(transform->position.x);
            destRect.y = static_cast<int>(transform->position.y);
            destRect.w = transform->width * transform->scale;
            destRect.h = transform->height * transform->scale;
        }
    }
    void draw() override
    {
        CSurface::Draw(renderer, texture, srcRect, destRect);
    }
};
#endif