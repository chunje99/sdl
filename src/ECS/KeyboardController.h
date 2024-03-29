#ifndef KEYBOARDCONTROLLER_H
#define KEYBOARDCONTROLLER_H

#include "../CGame.h"
#include "ECS.h"
#include "Components.h"

class KeyboardController : public Component
{
public:
    TransformComponent* transform;
    SpriteComponent* sprite;

    void init() override
    {
        transform = &entity->getComponent<TransformComponent>();
        sprite = &entity->getComponent<SpriteComponent>();
    }

    void update() override
    {
        if(CGame::event.type == SDL_KEYDOWN)
        {
            switch(CGame::event.key.keysym.sym)
            {
                case SDLK_w:
                    transform->velocity.y = -1;
                    sprite->Play("Idle");
                    break;
                case SDLK_a:
                    transform->velocity.x = -1;
                    sprite->Play("Walk");
                    sprite->spriteFlip = SDL_FLIP_HORIZONTAL;
                    break;
                case SDLK_d:
                    transform->velocity.x = 1;
                    sprite->Play("Walk");
                    break;
                case SDLK_s:
                    transform->velocity.y = 1;
                    sprite->Play("Idle");
                    break;
                default:
                    break;
            }
        }
        if(CGame::event.type == SDL_KEYUP)
        {
            switch(CGame::event.key.keysym.sym)
            {
                case SDLK_w:
                    transform->velocity.y = 0;
                    sprite->Play("Idle");
                    break;
                case SDLK_a:
                    transform->velocity.x = 0;
                    sprite->Play("Idle");
                    sprite->spriteFlip = SDL_FLIP_NONE;
                    break;
                case SDLK_d:
                    transform->velocity.x = 0;
                    sprite->Play("Idle");
                    break;
                case SDLK_s:
                    transform->velocity.y = 0;
                    sprite->Play("Idle");
                    break;
                case SDLK_ESCAPE:
                    CGame::isRunning = false;
                    break;
                default:
                    break;
            }
        }
    }

};

#endif