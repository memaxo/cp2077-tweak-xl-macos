#include "App/Application.hpp"
#include "App/Project.hpp"
#include "Core/Facades/Hook.hpp"
#include "Core/Facades/Runtime.hpp"
#include <iostream>

namespace
{
Core::UniquePtr<App::Application> g_app;
bool g_initialized = false;  // Guard against double initialization
}

// RED4ext

RED4EXT_C_EXPORT bool RED4EXT_CALL Main(RED4ext::PluginHandle aHandle, RED4ext::EMainReason aReason,
                                        const RED4ext::Sdk* aSdk)
{
    try {
        std::cerr << "[TweakXL] Main called with reason: " << static_cast<int>(aReason) << std::endl;
        
        switch (aReason)
        {
        case RED4ext::EMainReason::Load:
        {
            // Guard against double initialization
            if (g_initialized)
            {
                std::cerr << "[TweakXL] Already initialized, skipping duplicate Load" << std::endl;
                return true;
            }
            g_initialized = true;
            
            std::cerr << "[TweakXL] Creating Application..." << std::endl;
            g_app = Core::MakeUnique<App::Application>(aHandle, aSdk);
            std::cerr << "[TweakXL] Application created, calling Bootstrap..." << std::endl;
            g_app->Bootstrap();
            std::cerr << "[TweakXL] Bootstrap complete" << std::endl;
            break;
        }
        case RED4ext::EMainReason::Unload:
        {
            // Guard against double shutdown
            if (!g_initialized || !g_app)
            {
                std::cerr << "[TweakXL] Not initialized or already shut down, skipping" << std::endl;
                return true;
            }
            g_initialized = false;
            
            std::cerr << "[TweakXL] Shutting down..." << std::endl;
            g_app->Shutdown();
            g_app = nullptr;
            std::cerr << "[TweakXL] Shutdown complete" << std::endl;
            break;
        }
        }

        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "[TweakXL] ERROR: Exception in Main: " << e.what() << std::endl;
        return false;
    }
    catch (...) {
        std::cerr << "[TweakXL] ERROR: Unknown exception in Main" << std::endl;
        return false;
    }
}

RED4EXT_C_EXPORT void RED4EXT_CALL Query(RED4ext::PluginInfo* aInfo)
{
    aInfo->name = App::Project::NameW;
    aInfo->author = App::Project::AuthorW;
    aInfo->version = RED4EXT_SEMVER(App::Project::Version.major,
                                    App::Project::Version.minor,
                                    App::Project::Version.patch);

    aInfo->runtime = RED4EXT_RUNTIME_INDEPENDENT;
    aInfo->sdk = RED4EXT_SDK_LATEST;
}

RED4EXT_C_EXPORT uint32_t RED4EXT_CALL Supports()
{
    return RED4EXT_API_VERSION_LATEST;
}

#if defined(_WIN32) || defined(_WIN64)
// ASI (Windows only)

BOOL APIENTRY DllMain(HMODULE aHandle, DWORD aReason, LPVOID)
{
    using GameMain = Core::RawFunc<Red::AddressLib::Main, int32_t (*)(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                                                                      PWSTR pCmdLine, int nCmdShow)>;

    static const bool s_isGame = Core::Runtime::IsEXE(L"Cyberpunk2077.exe");
    static const bool s_isASI = Core::Runtime::IsASI(aHandle);

    switch (aReason) // NOLINT(hicpp-multiway-paths-covered)
    {
    case DLL_PROCESS_ATTACH:
    {
        DisableThreadLibraryCalls(aHandle);

        if (s_isGame && s_isASI)
        {
            g_app = Core::MakeUnique<App::Application>(aHandle);

            Core::Hook::Before<GameMain>(+[]() {
                g_app->Bootstrap();
            });
        }
        break;
    }
    case DLL_PROCESS_DETACH:
    {
        if (s_isGame && s_isASI)
        {
            g_app->Shutdown();
            g_app = nullptr;
        }
        break;
    }
    }

    return TRUE;
}
#else
// macOS: No ASI loader support - RED4ext handles plugin lifecycle
#endif
