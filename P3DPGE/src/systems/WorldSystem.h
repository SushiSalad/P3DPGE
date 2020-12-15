#pragma once
#include "System.h"

/*
	Note on how this system is meant to work:

	Rather than dealing with objects being created/deleted in the middle of the frame and
	the consequence that might occur from that, I opted to use a creation/deletion buffer
	that only acts at the end of the frame (after rendering).

	Advantages:
	1. Objects wont get removed/inserted in the middle of the frame, possibly causing
	unexpected behaviour and affecting some systems while avoiding others.
	2. If the frame is going too long, we can opt to delay the creation or deletion of
	an entity or component, though I would say to be careful with delayed creation as players
	might notice even if the creation of something is one or two frames delayed.
	3. You can always add/remove an Entity manually into the middle of the frame through
	the EntityAdmin if necessary

	Disadvantages:
	1. More noticeable delays when on low framerates.
	2. An entity queued for deletion will still affect systems until the end of the frame.
	3. The buffer has potential to grow too large if delays keep getting stacked
*/

struct Entity;
struct Component;
typedef uint32 EntityID;

struct WorldSystem : public System {
//// Entity Lifetime Functions ////

	//initializes an entity with a single component and adds it to the creation buffer
	//returns the entity that was created
	Entity* CreateEntity(Component* singleton);

	//initializes an entity with a component vector and adds it to the creation buffer
	//returns the entity that was created
	Entity* CreateEntity(std::vector<Component*> components);

	//adds an already initialized entity to the creation buffer
	//returns its place in the queue
	int32 AddEntityToCreationBuffer(Entity* entity);

	//adds an already initialized entity to the deletion buffer
	//returns its place in the queue, -1 if the entity could not be found
	int32 AddEntityToDeletionBuffer(Entity* entity);

//// Entity Component Functions ////

	//adds a component to the end of an entity's components vector
	//returns the position in the vector, or -1 if the entity could not be found
	int32 AddAComponentToEntity(Entity* entity, Component* component);

	//function description goes here
	void AddComponentsToEntity(Entity* entity, std::vector<Component*> components);

	//returns an entity's component vector
	//returns 0 if the entity could not be found
	std::vector<Component*>* GetComponentsOnEntity(Entity* entity);

	//removes and deletes a component from an entity's components vector
	//returns true if successful removal, false otherwise
	bool RemoveAComponentFromEntity(Entity* entity, Component* component);

	//function description goes here
	bool RemoveComponentsFromEntity(Entity* entity, std::vector<Component*> components);

	//function description goes here
	void Update(float deltaTime, olc::PixelGameEngine* p) override;
};