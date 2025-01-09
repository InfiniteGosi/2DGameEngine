#pragma once
#include <bitset>
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <set>
#include <memory>
#include "../Logger/Logger.h"

const unsigned int MAX_COMPONENTS = 32;

/////////////////////////////////////////////////////////////////////////////
// Signature
/////////////////////////////////////////////////////////////////////////////
// We use a bitset to keep track of which components an entity has,
// and also helps keep track of which entities a system is interested in.
/////////////////////////////////////////////////////////////////////////////
typedef std::bitset<MAX_COMPONENTS> Signature;

struct IComponent {
protected:
	static int nextId;
};


// Used to assign a unique id to a component type
template <typename T>
class Component : public IComponent {
public:
	// Returns the unique id of Component<T>
	static int GetId() {
		static auto id = nextId++;
		return id;
	}
};


class Entity {
private:
	int id;
public:
	Entity(int id) : id(id) {};
	Entity(const Entity& entity) = default;
	const int GetId() const;

	Entity& operator =(const Entity& other) = default;

	bool operator ==(const Entity& other) const {
		return id == other.id;
	}

	bool operator !=(const Entity& other) const {
		return id != other.id;
	}

	bool operator <(const Entity& other) const {
		return id < other.id;
	}

	bool operator >(const Entity& other) const {
		return id > other.id;
	}

	template <typename TComponent, typename ...TArgs> void AddComponent(TArgs&& ...args);
	template <typename TComponent, typename ...TArgs> void RemoveComponent();
	template <typename TComponent, typename ...TArgs> bool HasComponent() const;
	template <typename TComponent, typename ...TArgs> TComponent& GetComponent() const;

	// Hold a pointer to the entity's owner registry
	class Registry* registry;
};


/////////////////////////////////////////////////////////////////////////////
// System
/////////////////////////////////////////////////////////////////////////////
// The system processes entities that contain a specific signature
/////////////////////////////////////////////////////////////////////////////
class System {
private:
	Signature componentSignature;
	std::vector<Entity> entities;
public:
	System() = default;
	~System() = default;

	void AddEntityToSystem(Entity entity);
	void RemoveEntityFromSystem(Entity entity);
	std::vector<Entity> GetSystemEntities() const;
	const Signature& GetComponentSignature() const;

	// Define the component type that entities must have to be considered by the system
	template <typename TComponent> void RequireComponent();
};

/////////////////////////////////////////////////////////////////////////////
// Pool
/////////////////////////////////////////////////////////////////////////////
// is just a vector of objects of type T
/////////////////////////////////////////////////////////////////////////////
class IPool {
public:
	virtual ~IPool() {};
};

template <typename T>
class Pool : public IPool {
private:
	std::vector<T> data;
public:
	Pool(int size = 100) {
		Resize(size);
	}

	virtual ~Pool() = default;

	bool isEmpty() const {
		return data.empty();
	}

	int GetSize() const {
		return data.size();
	}

	void Resize(int n) {
		data.resize(n);
	}

	void Clear() {
		data.clear();
	}

	void Add(T object) {
		data.push_back(object);
	}

	void Set(int index, T object) {
		data[index] = object;
	}

	T Get(int index) {
		return static_cast<T&>(data[index]);
	}

	T& operator [](unsigned int index) {
		return data[index];
	}
};

/////////////////////////////////////////////////////////////////////////////
// Registry
/////////////////////////////////////////////////////////////////////////////
// Manages the creation and destruction of entities 
// Adding systems and components to entities
/////////////////////////////////////////////////////////////////////////////
class Registry {
private:
	int numEntities = 0;

	// Vector of component pools, each pool contains all the data for a certain component type
	// [Vector index is the component type id]
	// [Pool index is the entity id]
	std::vector<std::shared_ptr<IPool>> componentPools;

	// Vector of component signatures per entity, saying which component is turned on for a given entity
	// [Vector index is entity id]
	std::vector<Signature> entityComponentSignatures;

	std::unordered_map<std::type_index, std::shared_ptr<System>> systems;

	// Set of entities to be added or removed in the next registry Update()
	std::set<Entity> entitiesToBeAdded;
	std::set<Entity> entitiesToBeRemoved;

public:
	Registry() = default;
	
	// Entity management
	Entity CreateEntity();

	// Finally processes the entities that are waiting to be added/killed
	void Update();

	// Componenet management
	template <typename TComponent, typename ...TArgs> void AddComponent(Entity entity, TArgs&& ...args);
	template <typename TComponent> void RemoveComponent(Entity entity);
	template <typename TComponent> bool HasComponent(Entity entity) const;
	template <typename TComponent> TComponent& GetComponent(Entity entity) const;

	// System management;
	template <typename TSystem, typename ...TArgs> void AddSystem(TArgs&& ...args);
	template <typename TSystem> void RemoveSystem();
	template <typename TSystem> bool HasSystem() const;
	template <typename TSystem> TSystem& GetSystem() const;

	// Checks the component signature of an entity and add the entity to the systems
	// that are interested in it
	void AddEntityToSystem(Entity entity);
};

template <typename TComponent> 
void System::RequireComponent() {
	const auto componentId = Component<TComponent>::GetId();
	componentSignature.set(componentId);
}

template <typename TComponent, typename ...TArgs>
void Registry::AddComponent(Entity entity, TArgs&& ...args) {
	const int componentId = Component<TComponent>::GetId();
	const int entityId = entity.GetId();

	// If componentId is greater than the current size of the componentPools, resize the vector
	if (componentId >= componentPools.size()) {
		componentPools.resize(componentId + 1, nullptr);
	}

	// If there is no pool, create a new pool for that component type
	if (!componentPools[componentId]) {
		std::shared_ptr<Pool<TComponent>> newComponentPool = std::make_shared<Pool<TComponent>>();
		componentPools[componentId] = newComponentPool;
	}

	// Get the pool of component values for that component type
	std::shared_ptr<Pool<TComponent>> componentPool = std::static_pointer_cast<Pool<TComponent>>(componentPools[componentId]);

	// If the entity id is greater than the current size of the component pool, resize the pool
	if (entityId >= componentPool->GetSize()) {
		componentPool->Resize(numEntities);
	}

	// Create a new Component object of the type T, and foward the various parameters to the constructor of the component
	TComponent newComponent(std::forward<TArgs>(args)...);

	// Add the new component to the component pool, using the entity id as index
	componentPool->Set(entityId, newComponent);

	// Turn on the component signature -> The entity has this component
	entityComponentSignatures[entityId].set(componentId);

	Logger::Log("Component id = " + std::to_string(componentId) + " was added to entity id " + std::to_string(entityId));
}

template <typename TComponent>
void Registry::RemoveComponent(Entity entity) {
	const int componentId = Component<TComponent>::GetId();
	const int entityId = entity.GetId();

	// Turn off the component signature -> The entity no longer has this component
	entityComponentSignatures[entityId].set(componentId, false);

	Logger::Log("Component id = " + std::to_string(componentId) + " was removed from entity id " + std::to_string(entityId));
}

template <typename TComponent>
bool Registry::HasComponent(Entity entity) const {
	const int componentId = Component<TComponent>::GetId();
	const int entityId = entity.GetId();
	return entityComponentSignatures[entityId].test(componentId);
}

template <typename TComponent>
TComponent& Registry::GetComponent(Entity entity) const {
	const int componentId = Component<TComponent>::GetId();
	const int entityId = entity.GetId();
	auto componentPool =  std::static_pointer_cast<Pool<TComponent>>(componentPools[componentId]);
	return componentPool->Get(entityId);
}

template <typename TComponent, typename ...TArgs> 
void Entity::AddComponent(TArgs&& ...args) {
	registry->AddComponent<TComponent>(*this, std::forward<TArgs>(args)...);
}

template <typename TComponent, typename ...TArgs>
void Entity::RemoveComponent() {
	registry->RemoveComponent<TComponent>(*this);
}

template <typename TComponent, typename ...TArgs> 
bool Entity::HasComponent() const {
	return registry->HasComponent<TComponent>(*this);
}

template <typename TComponent, typename ...TArgs>
TComponent& Entity::GetComponent() const {
	return registry->GetComponent<TComponent>(*this);
}

template<typename TSystem, typename ...TArgs> 
void Registry::AddSystem(TArgs&& ...args) {
	std::shared_ptr<TSystem> newSystem = std::make_shared<TSystem>(std::forward<TArgs>(args)...);
	systems.insert(std::make_pair(std::type_index(typeid(TSystem)), newSystem));
}

template<typename TSystem> 
void Registry::RemoveSystem() {
	auto system = systems.find(std::type_index(typeid(TSystem)));
	systems.erase(system);
}

template<typename TSystem> 
bool Registry::HasSystem() const {
	return systems.find(std::type_index(typeid(TSystem))) != systems.end();
}

template<typename TSystem> 
TSystem& Registry::GetSystem() const {
	auto system = systems.find(std::type_index(typeid(TSystem)));
	return *(std::static_pointer_cast<TSystem>(system->second));
}

