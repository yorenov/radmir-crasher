#pragma once

#include <renderer.h>
#include <optional>
#include <string>
#include <filesystem>

extern IDirect3DDevice9* g_pDevice;
extern cRender * g_pRender;

std::filesystem::path GetProjectDirectory(std::optional<std::filesystem::path> append = std::nullopt);