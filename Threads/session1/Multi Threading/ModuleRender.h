#pragma once

#include "Module.h"
#include <Windows.h>

struct Texture;

class ModuleRender : public Module
{
public:

	// Virtual functions

	bool init() override;

	bool postUpdate() override;

	bool cleanUp() override;


	// Public methods

	void resizeBuffers(unsigned int width, unsigned int height);

	void present();

	
private:

	void renderScene();

	bool CreateDeviceD3D(HWND hWnd);
	void CleanupDeviceD3D();
	void CreateRenderTarget();
	void CleanupRenderTarget();

	Texture * whitePixel = nullptr;
	Texture * blackPixel = nullptr;
};

