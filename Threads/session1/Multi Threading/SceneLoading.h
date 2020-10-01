#pragma once

#include "ModuleScene.h"

struct GameObject;

class SceneLoading : public Scene
{
public:

	void enable() override;

	void update() override;

	void disable() override;

private:

	float elapsedTime = 0.0f;

	GameObject *progressBar = nullptr;
};

