#pragma once

#include "Module.h"

// TODO 0: Uncomment this line to enable the usage of ModuleTaskManager
#define USE_TASK_MANAGER

struct Texture;

class ModuleResources : public Module
{
public:

	bool init() override;

#if defined(USE_TASK_MANAGER)
	void onTaskFinished(Task *task) override;

	void loadTextureAsync(const char *filename);
#endif

	Texture *loading = nullptr;
	Texture *yellow = nullptr;
	Texture *mario = nullptr;
	Texture *space = nullptr;
	Texture *asteroid1 = nullptr;
	Texture *asteroid2 = nullptr;
	Texture *spacecraft1 = nullptr;
	Texture *spacecraft2 = nullptr;
	Texture *spacecraft3 = nullptr;

	bool loadingFinished = false;

	float completionRatio = 0.0f;
};

