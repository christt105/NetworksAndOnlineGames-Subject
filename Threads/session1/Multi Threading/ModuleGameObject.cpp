#include "ModuleGameObject.h"
#include "ModuleScene.h"
#include "Application.h"


GameObject::GameObject()
{
	scene = App->modScene->activeScene;

	// NOTE(jesus): Game objects are automatically insterted into ModuleGameObject
	bool inserted = false;
	for (auto &gameObject : App->modGameObject->gameObjects)
	{
		if (gameObject == nullptr)
		{
			gameObject = this;
			inserted = true;
		}
	}

	if (!inserted)
	{
		App->modGameObject->gameObjects.push_back(this);
	}
}


bool ModuleGameObject::init()
{
	return true;
}

bool ModuleGameObject::preUpdate()
{
	return true;
}

bool ModuleGameObject::update()
{
	return true;
}

bool ModuleGameObject::postUpdate()
{
	for (auto &gameObject : gameObjects)
	{
		if (gameObject == nullptr) continue;

		if (gameObject->deleteFlag)
		{
			delete gameObject;
			gameObject = nullptr;
		}
	}
	return true;
}

bool ModuleGameObject::cleanUp()
{
	for (auto gameObject : gameObjects)
	{
		delete gameObject;
	}
	gameObjects.clear();

	return true;
}

void ModuleGameObject::deleteGameObjectsInScene(Scene *scene)
{
	for (auto &gameObject : gameObjects)
	{
		if (gameObject == nullptr) continue;

		if (gameObject->scene == scene)
		{
			gameObject->deleteFlag = true;
		}
	}
}
