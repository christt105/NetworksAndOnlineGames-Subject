#include "ModuleResources.h"
#include "ModuleTextures.h"
#include "ModuleTaskManager.h"
#include "Application.h"
#include "Log.h"

#if defined(USE_TASK_MANAGER)
class TaskLoadTexture : public Task
{
public:

	const char *filename = nullptr;
	Texture *texture = nullptr;

	void execute() override
	{
		texture = App->modTextures->loadTexture(filename);
	}
};
#endif


bool ModuleResources::init()
{
	// load a texture
	loading = App->modTextures->loadTexture("loading_screen.png");
	yellow = App->modTextures->loadTexture("yellow_color.png");

#if !defined(USE_TASK_MANAGER)
	mario = App->modTextures->loadTexture("mario.jpg");
	space = App->modTextures->loadTexture("space_background.jpg");
	asteroid1 = App->modTextures->loadTexture("asteroid1.png");
	asteroid2 = App->modTextures->loadTexture("asteroid2.png");
	spacecraft1 = App->modTextures->loadTexture("spacecraft1.png");
	spacecraft2 = App->modTextures->loadTexture("spacecraft2.png");
	spacecraft3 = App->modTextures->loadTexture("spacecraft3.png");
	loadingFinished = true;
	completionRatio = 1.0f;
#else
	const char *texturesToLoad[] = {
		"mario.jpg",
		"space_background.jpg",
		"asteroid1.png",
		"asteroid2.png",
		"spacecraft1.png",
		"spacecraft2.png",
		"spacecraft3.png"
	};
	for (auto filename : texturesToLoad)
	{
		loadTextureAsync(filename);
	}
#endif

	return true;
}

#if defined(USE_TASK_MANAGER)

void ModuleResources::loadTextureAsync(const char * filename)
{
	TaskLoadTexture *task = new TaskLoadTexture;
	task->owner = this;
	task->filename = filename;
	App->modTaskManager->scheduleTask(task, this);
}

void ModuleResources::onTaskFinished(Task * task)
{
	TaskLoadTexture *taskLoadTexture = dynamic_cast<TaskLoadTexture*>(task);
	if (taskLoadTexture != nullptr)
	{
		if      (taskLoadTexture->filename == "mario.jpg") mario = taskLoadTexture->texture;
		else if (taskLoadTexture->filename == "space_background.jpg") space = taskLoadTexture->texture;
		else if (taskLoadTexture->filename == "asteroid1.png") asteroid1 = taskLoadTexture->texture;
		else if (taskLoadTexture->filename == "asteroid2.png") asteroid2 = taskLoadTexture->texture;
		else if (taskLoadTexture->filename == "spacecraft1.png") spacecraft1 = taskLoadTexture->texture;
		else if (taskLoadTexture->filename == "spacecraft2.png") spacecraft2 = taskLoadTexture->texture;
		else if (taskLoadTexture->filename == "spacecraft3.png") spacecraft3 = taskLoadTexture->texture;

		int numLoaded = 0;
		numLoaded += (int)(mario != nullptr);
		numLoaded += (int)(space != nullptr);
		numLoaded += (int)(asteroid1 != nullptr);
		numLoaded += (int)(asteroid2 != nullptr);
		numLoaded += (int)(spacecraft1 != nullptr);
		numLoaded += (int)(spacecraft2 != nullptr);
		numLoaded += (int)(spacecraft3 != nullptr);

		loadingFinished = numLoaded >= 7;
		completionRatio = (float)numLoaded / 7.0f;
	}

	delete task;
}

#endif
