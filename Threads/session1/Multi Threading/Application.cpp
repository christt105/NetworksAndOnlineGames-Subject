#include "Application.h"
#include "ModuleGameObject.h"
#include "ModuleRender.h"
#include "ModuleResources.h"
#include "ModuleScene.h"
#include "ModuleTextures.h"
#include "ModuleTaskManager.h"
#include "ModuleUI.h"
#include "ModulePlatform.h"

#define ADD_MODULE(ModuleClass, module) \
	module = new ModuleClass(); \
	modules[numModules++] = module;

Application::Application()
{
	// Create modules
	ADD_MODULE(ModulePlatform,    modPlatform);
	ADD_MODULE(ModuleRender,      modRender);
	ADD_MODULE(ModuleTaskManager, modTaskManager);
	ADD_MODULE(ModuleTextures,    modTextures);
	ADD_MODULE(ModuleResources,   modResources);
	ADD_MODULE(ModuleUI,          modUI);
	ADD_MODULE(ModuleGameObject,  modGameObject);
	ADD_MODULE(ModuleScene,       modScene);
}


Application::~Application()
{
	// Destroy modules
	for (auto module : modules)
	{
		if (module == nullptr) continue;

		delete module;
	}
}


bool Application::init()
{
	for (auto module : modules)
	{
		if (module == nullptr) continue;

		if (module->init() == false)
		{
			return false;
		}
	}
	return true;
}

bool Application::update()
{
	if (doPreUpdate() == false) return false;
	
	if (doUpdate() == false) return false;

	if (doGui() == false) return false;

	if (doPostUpdate() == false) return false;

	modRender->present();

	return true;
}

bool Application::cleanUp()
{
	for (int i = numModules; i > 0; --i)
	{
		Module *module = modules[i - 1];

		if (module == nullptr) continue;

		if (module->cleanUp() == false)
		{
			return false;
		}
	}

	return true;
}

bool Application::doPreUpdate()
{
	for (auto module : modules)
	{
		if (module == nullptr) continue;
		
		if (module->preUpdate() == false) return false;
	}
	return true;
}

bool Application::doUpdate()
{
	for (auto module : modules)
	{
		if (module == nullptr) continue;

		if (module->update() == false) return false;
	}
	return true;
}

bool Application::doGui()
{
	for (auto module : modules)
	{
		if (module == nullptr) continue;

		if (module->gui() == false) return false;
	}
	return true;
}

bool Application::doPostUpdate()
{
	for (auto module : modules)
	{
		if (module == nullptr) continue;

		if (module->postUpdate() == false) return false;
	}
	return true;
}
