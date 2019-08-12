#ifndef COLLISION_H
#define COLLISION_H

#include <SDL.h>

class ColliderComponent;
class Collision
{
public:
    static bool AABB(const SDL_Rect& recA, const SDL_Rect& rectB);
    static bool AABB(const ColliderComponent& colA, const ColliderComponent& colB);
};

#endif