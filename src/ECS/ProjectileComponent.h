#ifndef SDL_PROJECTILE_COMPONENT_H
#define SDL_PROJECTILE_COMPONENT_H

#include "ECS.h"
#include "Components.h"
#include "../Vector2D.h"
#include <glog/logging.h>
#include <gflags/gflags.h>

class ProjectileComponent : public Component
{
public:
	ProjectileComponent(int rng, int sp, Vector2D vel) : range(rng), speed(sp), velocity(vel) {}
	~ProjectileComponent(){}

	void init() override
	{
		transform = &entity->getComponent<TransformComponent>();
		transform->velocity = velocity;
	}

	void update() override
	{
		distance += speed;
		if (distance > range)
		{
			LOG(INFO) << "Out of Range";
			entity->destroy();
		}
		else if (transform->position.x > CGame::camera.x + CGame::camera.w ||
				 transform->position.x < CGame::camera.x ||
				 transform->position.y > CGame::camera.y + CGame::camera.h ||
				 transform->position.y < CGame::camera.y)
		{
			LOG(INFO) << "Out of bounds!";
			entity->destroy();
		}
	}

private:
	TransformComponent* transform;

	int range = 0;
	int speed = 0;
	int distance = 0;
	Vector2D velocity;
};
#endif