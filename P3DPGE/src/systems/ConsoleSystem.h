#pragma once
#include "System.h"

struct ConsoleSystem : public System {
	void Init() override;
	void Update() override;
	void DrawConsole();
};