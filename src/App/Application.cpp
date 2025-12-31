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
#endif

#if defined(_WIN32) || defined(_WIN64)
App::Application::Application(HMODULE aHandle, const RED4ext::Sdk* aSdk)
#else
App::Application::Application(void* aHandle, const RED4ext::Sdk* aSdk)
#endif
{
    Register<Core::RuntimeProvider>(aHandle)
        ->SetBaseImagePathDepth(2);

#if defined(_WIN32) || defined(_WIN64)
    Register<Support::MinHookProvider>();
#endif

    Register<Support::SpdlogProvider>()
        ->AppendTimestampToLogName()
        ->CreateRecentLogSymlink();
    Register<Support::RED4extProvider>(aHandle, aSdk)
        ->EnableAddressLibrary()
        ->RegisterScripts(Env::PluginScriptsDir());

#if defined(_WIN32) || defined(_WIN64)
    Register<Support::RedLibProvider>();
#endif

    Register<App::TweakService>(Env::GameVer(), Env::GameDir(), Env::TweaksDir(),
                                Env::InheritanceMapPath(), Env::ExtraFlatsPath(),
                                Env::RedModSourcesDir());
    Register<App::StatService>();
}

void App::Application::OnStarting()
{
    LogInfo("{} {} is starting...", Project::Name, Project::Version.to_string());

    Migration::CleanUp(Env::LegacyScriptsDir());
}
