#pragma once
#include <bitset>
#include <vector>

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
	const int GetId() const;

	bool operator ==(const Entity& other) const {
		return id == other.id;
	}
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
	// Vector index is the component type id
	// Pool index is the entity id
	std::vector<IPool*> componentPools;
};

template <typename TComponent> 
void System::RequireComponent() {
	const auto componentId = Component<TComponent>::GetId();
	componentSignature.set(componentId);
}