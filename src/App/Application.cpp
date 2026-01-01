#include "Application.hpp"
#include "App/Environment.hpp"
#include "App/Migration.hpp"
#include "App/Project.hpp"
#include "App/Stats/StatService.hpp"
#include "App/Tweaks/TweakService.hpp"
#include "Core/Foundation/RuntimeProvider.hpp"
#include "Support/RED4ext/RED4extProvider.hpp"
#include "Support/Spdlog/SpdlogProvider.hpp"

#if defined(_WIN32) || defined(_WIN64)
#include "Support/MinHook/MinHookProvider.hpp"
#include "Support/RedLib/RedLibProvider.hpp"
#else
// macOS: Use custom address resolver and hooking provider
#include "Support/macOS/TweakXLAddressResolver.hpp"
#include "Support/macOS/MacOSHookingProvider.hpp"
#endif

#include <iostream>

#if defined(_WIN32) || defined(_WIN64)
App::Application::Application(HMODULE aHandle, const RED4ext::Sdk* aSdk)
#else
App::Application::Application(void* aHandle, const RED4ext::Sdk* aSdk)
#endif
{
    std::cerr << "[TweakXL::App] Step 1: RuntimeProvider..." << std::endl;
    Register<Core::RuntimeProvider>(aHandle)
        ->SetBaseImagePathDepth(2);

#if defined(_WIN32) || defined(_WIN64)
    Register<Support::MinHookProvider>();
#endif

    std::cerr << "[TweakXL::App] Step 2: SpdlogProvider..." << std::endl;
    Register<Support::SpdlogProvider>()
        ->AppendTimestampToLogName()
        ->CreateRecentLogSymlink();
    
#if defined(_WIN32) || defined(_WIN64)
    // Windows: Use RED4ext's address library
    Register<Support::RED4extProvider>(aHandle, aSdk)
        ->EnableAddressLibrary()
        ->RegisterScripts(Env::PluginScriptsDir());
#else
    std::cerr << "[TweakXL::App] Step 3: TweakXLAddressResolver..." << std::endl;
    // macOS: Use custom address resolver (SDK's resolver requires 126+ addresses we don't have)
    Register<Support::TweakXLAddressResolver>();
    
    std::cerr << "[TweakXL::App] Step 3b: MacOSHookingProvider..." << std::endl;
    // macOS: Use hooking provider that forwards to RED4ext's SDK
    Register<Support::MacOSHookingProvider>(aHandle, aSdk);
    
    std::cerr << "[TweakXL::App] Step 4: RED4extProvider..." << std::endl;
    Register<Support::RED4extProvider>(aHandle, aSdk)
        ->RegisterScripts(Env::PluginScriptsDir());
#endif

#if defined(_WIN32) || defined(_WIN64)
    Register<Support::RedLibProvider>();
#endif

    std::cerr << "[TweakXL::App] Step 5: TweakService..." << std::endl;
    Register<App::TweakService>(Env::GameVer(), Env::GameDir(), Env::TweaksDir(),
                                Env::InheritanceMapPath(), Env::ExtraFlatsPath(),
                                Env::RedModSourcesDir());
    
    std::cerr << "[TweakXL::App] Step 6: StatService..." << std::endl;
    Register<App::StatService>();
    
    std::cerr << "[TweakXL::App] Construction complete!" << std::endl;
}

void App::Application::OnStarting()
{
    std::cerr << "[TweakXL::App::OnStarting] Starting..." << std::endl;
    
    try {
        std::cerr << "[TweakXL::App::OnStarting] Trying simple LogInfo..." << std::endl;
        LogInfo("TweakXL is starting...");  // Use simple string instead of format
        
        std::cerr << "[TweakXL::App::OnStarting] Calling Migration::CleanUp..." << std::endl;
        Migration::CleanUp(Env::LegacyScriptsDir());
        
        std::cerr << "[TweakXL::App::OnStarting] Complete!" << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "[TweakXL::App::OnStarting] Exception: " << e.what() << std::endl;
        throw;
    }
}
