#pragma once
#include "../ECS/ECS.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/ProjectileComponent.h"
#include "../Components/HealthComponent.h"
#include "../Components/AudioComponent.h"
#include "../EventBus/EventBus.h"
#include "../Events/CollisionEvent.h"
#include "../Logger/Logger.h"
#include <SDL_mixer.h>

class DamageSystem : public System {
public:
	DamageSystem() {
		RequireComponent<BoxColliderComponent>();
	}

	void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus) {
		eventBus->SubscribeToEvent<CollisionEvent>(this, &DamageSystem::OnCollision);
	}

	void OnCollision(CollisionEvent& event) {
		Entity a = event.a;
		Entity b = event.b;
		Logger::Log("Collision event emitted " + std::to_string(a.GetId()) + " and " + std::to_string(b.GetId()));
		
		if (a.BelongsToGroup("projectiles") && b.HasTag("player")) {
			OnProjectileHitsPlayer(a, b);
		}

		if (b.BelongsToGroup("projectiles") && a.HasTag("player")) {
			OnProjectileHitsPlayer(b, a);
		}

		if (a.BelongsToGroup("projectiles") && b.BelongsToGroup("enemies")) {
			OnProjectileHitsEnemy(a, b);
		}

		if (b.BelongsToGroup("projectiles") && a.BelongsToGroup("enemies")) {
			OnProjectileHitsEnemy(b, a);
		}
	}

	void OnProjectileHitsPlayer(Entity projectile, Entity player) {
		const auto projectileComponent = projectile.GetComponent<ProjectileComponent>();

		if (!projectileComponent.isFriendly) {
			// Reduce the health of the player by the projectile hitPercentDamage
			auto& health = player.GetComponent<HealthComponent>();
			health.healthPercentage -= projectileComponent.hitPercentDamage;

			if (health.healthPercentage <= 0) {
				if (player.HasComponent<AudioComponent>()) {
					const auto audio = player.GetComponent<AudioComponent>();
					// Stop the audio immediately
					Mix_HaltChannel(audio.channel);
				}
				player.Kill();
			}

			// Kill the projectile
			projectile.Kill();
		}
	}


	void OnProjectileHitsEnemy(Entity projectile, Entity enemy) {
		const auto projectileComponent = projectile.GetComponent<ProjectileComponent>();
		
		if (projectileComponent.isFriendly) {
			auto& health = enemy.GetComponent<HealthComponent>();
			health.healthPercentage -= projectileComponent.hitPercentDamage;

			if (health.healthPercentage <= 0) {
				enemy.Kill();
			}

			projectile.Kill();
		}
	}

	void Update() {

	}
};