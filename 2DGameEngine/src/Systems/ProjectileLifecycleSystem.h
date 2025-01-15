#pragma once
#include <SDL.h>
#include "../ECS/ECS.h"
#include "../Components/ProjectileComponent.h"

class ProjectileLifecycleSystem : public System {
public:
	ProjectileLifecycleSystem() {
		RequireComponent<ProjectileComponent>();
	}

	void Update() {
		for (auto entity : GetSystemEntities()) {
			auto projectile = entity.GetComponent<ProjectileComponent>();

			// kill projectiles after they reached their duration limit
			if (SDL_GetTicks() - projectile.startTime > projectile.duration) {
				entity.Kill();
			}
		}
	}
};