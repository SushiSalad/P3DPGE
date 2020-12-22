#pragma once
#include "../EntityAdmin.h"

struct System {
	EntityAdmin* admin; //reference to owning admin
	virtual void Init() {}
	virtual void Update() = 0;
	//virtual void NotifyComponent(Component*) = 0;
};