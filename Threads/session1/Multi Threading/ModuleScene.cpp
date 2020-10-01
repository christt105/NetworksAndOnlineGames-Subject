#include "ModuleScene.h"
#include "ModuleGameObject.h"
#include "SceneLoading.h"
#include "SceneGame.h"
#include "Application.h"

bool Scene::isEnabled() const
{
	return enabled;
}

bool ModuleScene::init()
{
	scenes[0] = sceneLoading = new SceneLoading;
	scenes[1] = sceneGame    = new SceneGame;

	setActiveScene(sceneLoading);

	return true;
}

bool ModuleScene::update()
{
	for (auto scene : scenes)
	{
		if (scene == nullptr) continue;

		if (scene->enabled && !scene->wasEnabled)
		{
			scene->enable();
		}

		if (!scene->enabled && scene->wasEnabled)
		{
			scene->disable();
		}

		if (scene->enabled)
		{
			scene->update();
		}

		scene->wasEnabled = scene->enabled;
	}

	return true;
}

bool ModuleScene::gui()
{
	for (auto scene : scenes)
	{
		if (scene->enabled)
		{
			scene->gui();
		}
	}

	return true;
}

bool ModuleScene::cleanUp()
{
	for (auto &scene : scenes)
	{
		delete scene;
		scene = nullptr;
	}
	return true;
}

void ModuleScene::setActiveScene(Scene *scene)
{
	if (activeScene != nullptr)
	{
		App->modGameObject->deleteGameObjectsInScene(activeScene);
		activeScene->enabled = false;
	}

	if (scene != nullptr)
	{
		activeScene = scene;
		activeScene->enabled = true;
	}
}
