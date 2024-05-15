#include "PluginState.h"

IDirect3DDevice9* g_pDevice = nullptr;
cRender* g_pRender = nullptr;

constexpr char PROJECT_MAIN_DIR[] = "RadmirCrasher";

std::filesystem::path GetProjectDirectory(std::optional<std::filesystem::path> append)
{
    std::filesystem::path path = std::filesystem::current_path()/PROJECT_MAIN_DIR;

    if ( append.has_value() )
        path /= append.value();

    return path;
}
