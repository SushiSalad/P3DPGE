#pragma once
#include "System.h"

struct CameraSystem : public System {
	void Init() override;
	void Update() override;
};