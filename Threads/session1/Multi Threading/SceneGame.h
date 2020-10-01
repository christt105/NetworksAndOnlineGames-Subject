#pragma once

#include "ModuleScene.h"

struct GameObject;

class SceneGame : public Scene
{
public:

	void enable() override;

	void update() override;

	void disable() override;


private:

	GameObject *player = nullptr;
	GameObject *asteroid1 = nullptr;
};

