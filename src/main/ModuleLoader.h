#pragma once

#include <vector>
#include "models/ModuleBase.h"

extern std::vector<ModuleBase*> g_modules;

void LoadModules();
