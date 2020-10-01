#include "SceneLoading.h"
#include "ModuleGameObject.h"
#include "ModuleResources.h"
#include "Application.h"
#include "Time.h"


void SceneLoading::enable()
{
	auto gameObject = new GameObject;
	gameObject->texture = App->modResources->loading;

	progressBar = new GameObject;
	progressBar->texture = App->modResources->yellow;
	progressBar->x = -331;
	progressBar->y = 74;
	progressBar->pivot_x = 0;
	progressBar->pivot_y = 0;
	progressBar->width = 1; // 750 is the max size
	progressBar->height = 45;

	elapsedTime = 0.0f;
}

void SceneLoading::update()
{
	progressBar->width = 750 * App->modResources->completionRatio;

	if (App->modResources->loadingFinished)
	{
		elapsedTime += Time.deltaTime;

		if (elapsedTime > 0.5f)
		{
			App->modScene->setActiveScene(App->modScene->sceneGame);
		}
	}
}

void SceneLoading::disable()
{
}
