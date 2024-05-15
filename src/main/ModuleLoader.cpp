#include "ModuleLoader.h"

#include "modules/Crasher.h"
#include <random>

std::vector<ModuleBase*> g_modules;

void LoadModules()
{
    g_modules = {};

    g_modules.emplace_back(reinterpret_cast<ModuleBase*>(new Crasher()));
}