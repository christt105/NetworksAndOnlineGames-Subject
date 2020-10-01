#pragma once

#include "Module.h"

class Scene
{
public:

	// Virtual methods

	virtual void enable() { }

	virtual void update() { }

	virtual void gui() { }

	virtual void disable() { }


	// Enable / disable scenes

	bool isEnabled() const;


private:

	bool enabled = false;
	bool wasEnabled = false;
	friend class ModuleScene;
};

class ModuleScene : public Module
{
public:

	bool init() override;

	bool update() override;

	bool gui() override;

	bool cleanUp() override;

	void setActiveScene(Scene *scene);

	Scene *activeScene = nullptr;

	Scene *sceneLoading = nullptr;
	Scene *sceneGame = nullptr;
	Scene *scenes[2];
};
