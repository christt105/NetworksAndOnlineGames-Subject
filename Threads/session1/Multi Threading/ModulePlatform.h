#pragma once

#include "Module.h"
#include <windows.h>

class ModulePlatform : public Module
{
public:

	// Virtual functions

	bool init() override;

	bool preUpdate() override;

	bool postUpdate() override;

	bool cleanUp() override;
};
