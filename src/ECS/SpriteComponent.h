#ifndef SPRITE_COMPONENTS_H
#define SPRITE_COMPONENTS_H
#include "Components.h"
#include "SDL.h"
#include "CSurface.h"
#include "Animation.h"
#include <map>
#include "../CGame.h"
#include "../AssetManager.h"

class SpriteComponent : public Component
{
private:
    TransformComponent *transform;
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
    SpriteComponent(std::string id)
    {
        setTex(id);
        transform = NULL;
    }
    SpriteComponent(std::string id, bool isAnimated)
    {
        animated = isAnimated;

        Animation idle = Animation(0, 4, 100);
        Animation walk = Animation(1, 4, 100);
        animations.emplace("Idle", idle);
        animations.emplace("Walk", walk);

        Play("Idle");

        setTex(id);
        transform = NULL;
    }
    ~SpriteComponent()
    {
    }

    void setTex(std::string id)
    {
        texture = CGame::assets->GetTexture(id);
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
            destRect.x = static_cast<int>(transform->position.x) - CGame::camera.x;
            destRect.y = static_cast<int>(transform->position.y) - CGame::camera.y;
            destRect.w = transform->width * transform->scale;
            destRect.h = transform->height * transform->scale;
        }
    }
    void draw() override
    {
        CSurface::Draw(texture, srcRect, destRect, spriteFlip);
    }

    void Play(const char* animName)
    {
        frames = animations[animName].frames;
        animIndex = animations[animName].index;
        speed = animations[animName].speed;
    }
};
#endif