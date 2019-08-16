#ifndef SPRITE_COMPONENTS_H
#define SPRITE_COMPONENTS_H
#include "Components.h"
#include "SDL.h"
#include "CSurface.h"
#include "Animation.h"
#include <map>

class SpriteComponent : public Component
{
private:
    TransformComponent *transform;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    SDL_Rect srcRect, destRect;

    bool animated = false;
    int frames = 0;
    int speed = 100;

public:
    int animIndex = 0;
    std::map<const char*, Animation> animations;
    SDL_RendererFlip spriteFlip = SDL_FLIP_NONE;

    SpriteComponent() = default;
    SpriteComponent(SDL_Renderer *rend, const char *path)
    {
        renderer = rend;
        setTex(path);
        transform = NULL;
    }
    SpriteComponent(SDL_Renderer *rend, const char *path, bool isAnimated)
    {
        renderer = rend;
        animated = isAnimated;

        Animation idle = Animation(0, 4, 100);
        Animation walk = Animation(1, 4, 100);
        animations.emplace("Idle", idle);
        animations.emplace("Walk", walk);

        Play("Idle");

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
            if(animated)
            {
                srcRect.x = srcRect.w * static_cast<int>((SDL_GetTicks() / speed) % frames);
            }
            srcRect.y = animIndex * transform->height;
            destRect.x = static_cast<int>(transform->position.x);
            destRect.y = static_cast<int>(transform->position.y);
            destRect.w = transform->width * transform->scale;
            destRect.h = transform->height * transform->scale;
        }
    }
    void draw() override
    {
        CSurface::Draw(renderer, texture, srcRect, destRect, spriteFlip);
    }

    void Play(const char* animName)
    {
        frames = animations[animName].frames;
        animIndex = animations[animName].index;
        speed = animations[animName].speed;
    }
};
#endif