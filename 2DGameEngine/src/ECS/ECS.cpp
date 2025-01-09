#include "ECS.h"
#include "../Logger/Logger.h"

int IComponent::nextId = 0;

const int Entity::GetId() const {
	return id;
}

void System::AddEntityToSystem(Entity entity) {
	entities.push_back(entity);
}

void System::RemoveEntityFromSystem(Entity entity) {
	entities.erase(std::remove_if(entities.begin(), entities.end(), [&entity](Entity other) {
		return entity == other;
	}), entities.end());
}

std::vector<Entity> System::GetSystemEntities() const {
	return entities;
}

const Signature& System::GetComponentSignature() const {
	return componentSignature;
}

Entity Registry::CreateEntity() {
	int entityId = numEntities++;

	Entity entity(entityId);
	entity.registry = this;
	entitiesToBeAdded.insert(entity);

	// Make sure the entityComponentSignatures vector can accomodate the new entity
	if (entityId >= entityComponentSignatures.size()) {
		entityComponentSignatures.resize(entityId + 1);
	}

	Logger::Log("Entity created with id = " + std::to_string(entityId));
	return entity;
}

void Registry::AddEntityToSystem(Entity entity) {
	const int entityId = entity.GetId();

	const auto& entityComponentSignature = entityComponentSignatures[entityId];
	
	// Loop all the systems
	for (auto& system : systems) {
		const auto& systemComponentSignature = system.second->GetComponentSignature();

		bool isInterested = (entityComponentSignature & systemComponentSignature) == systemComponentSignature;

		if (isInterested) {
			system.second->AddEntityToSystem(entity);
		}
	}
}

void Registry::Update() {
	// Add the entites that are waiting to be created to the active Systems
	for (Entity entity : entitiesToBeAdded) {
		AddEntityToSystem(entity);
	}
	entitiesToBeAdded.clear();

	// Remove the entites that are waiting to be removed in the active Systems
}



