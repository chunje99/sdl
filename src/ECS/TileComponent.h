#ifndef TILE_COMPONENT_H
#define TILE_COMPONENT_H

#include "ECS.h"
#include "TransformComponent.h"
#include "SDL.h"

class TileComponent : public Component
{
public:
    TransformComponent *transform;
    SpriteComponent *sprite;

    SDL_Rect tileRect;
    int tileID;
    char *path;

    TileComponent() = default;

    TileComponent(int x, int y, int w, int h, int id)
    {
        tileRect.x = x;
        tileRect.y = y;
        tileRect.w = w;
        tileRect.h = h;
        tileID = id;

        switch (tileID)
        {
        case 0:
            path = "images/water.png";
            break;
        case 1:
            path = "images/dirt.png";
            break;
        case 2:
            path = "images/grass.png";
            break;
        default:
            break;
        }
    }

    void init() override
    {
        entity->addComponent<TransformComponent>(
            static_cast<float>(tileRect.x),
            static_cast<float>(tileRect.y),
            tileRect.w, tileRect.h, 1);
        transform = &entity->getComponent<TransformComponent>();

        entity->addComponent<SpriteComponent>(CGame::renderer, path);
        sprite = &entity->getComponent<SpriteComponent>();
    }
};

#endif