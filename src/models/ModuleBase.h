#pragma once

#include <d3d9.h>
#include <functional>
#include <optional>
#include <string>

typedef void ( *ModuleDirect3DPresentSign )(void*, IDirect3DDevice9*, CONST RECT*, CONST RECT*, HWND,
                                            CONST RGNDATA*);
typedef void ( *ModuleDirect3DResetSign )(void*, IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);
typedef void ( *ModuleCallbackSign )(void*);

using ModuleDirect3DPresentDecl = void (void*, IDirect3DDevice9*, CONST RECT*, CONST RECT*, HWND, CONST RGNDATA*);
using ModuleDirect3DResetDecl = void (void*, IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);
using ModuleCallbackDecl = void (void*);

template<typename Ty>
using OptionalFunc = std::optional<std::function<Ty>>;

struct ModuleProperties
{
    std::string name;

    struct
    {
        bool        enabled{};
        std::string tabName;

        std::function<ModuleCallbackDecl> callback;
    }                                     imGuiTab;

    OptionalFunc<ModuleDirect3DPresentDecl> direct3DPresent;
    OptionalFunc<ModuleDirect3DResetDecl>   direct3DReset;

    OptionalFunc<ModuleCallbackDecl> mainLoop;
};

class ModuleBase
{
protected:
    ModuleProperties properties;

public:
    const ModuleProperties getProperties() const;

    ModuleBase() : properties({0}) {}
};