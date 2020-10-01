#pragma once

// Module declarations
class Module;
class ModuleGameObject;
class ModulePlatform;
class ModuleRender;
class ModuleResources;
class ModuleScene;
class ModuleTaskManager;
class ModuleTextures;
class ModuleUI;

class Application
{
public:

	// Constructor and destructor

	Application();

	~Application();


	// Application methods

	bool wantsToExit() const { return wannaExit; }

	void exit() { wannaExit = true; }


	// Application lifetime methods

	bool init();

	bool update();

	bool cleanUp();


private:

	// Private lifetime methods

	bool doPreUpdate();

	bool doUpdate();

	bool doGui();
	
	bool doPostUpdate();


public:

	// Modules
	ModulePlatform *modPlatform = nullptr;
	ModuleTaskManager *modTaskManager = nullptr;
	ModuleTextures *modTextures = nullptr;
	ModuleResources *modResources = nullptr;
	ModuleGameObject *modGameObject = nullptr;
	ModuleScene *modScene = nullptr;
	ModuleUI *modUI = nullptr;
	ModuleRender *modRender = nullptr;


private:

	// All modules
	Module* modules[16] = {};
	int numModules = 0;

	// Exit flag
	bool wannaExit = false;
};

extern Application* App;
