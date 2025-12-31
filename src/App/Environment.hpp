#pragma once

#include "Core/Facades/Runtime.hpp"

namespace App::Env
{
inline auto GameDir()
{
    return Core::Runtime::GetRootDir();
}

inline auto TweaksDir()
{
#if defined(_WIN32) || defined(_WIN64)
    return GameDir() / L"r6" / L"tweaks";
#else
    return GameDir() / "r6" / "tweaks";
#endif
}

inline auto RedModSourcesDir()
{
#if defined(_WIN32) || defined(_WIN64)
    return GameDir() / L"tools" / L"redmod" / L"tweaks";
#else
    return GameDir() / "tools" / "redmod" / "tweaks";
#endif
}

inline auto LegacyScriptsDir()
{
#if defined(_WIN32) || defined(_WIN64)
    return GameDir() / L"r6" / L"scripts" / L"TweakXL";
#else
    return GameDir() / "r6" / "scripts" / "TweakXL";
#endif
}

inline auto PluginDir()
{
    return Core::Runtime::GetModuleDir();
}

inline auto PluginScriptsDir()
{
#if defined(_WIN32) || defined(_WIN64)
    return PluginDir() / L"Scripts";
#else
    return PluginDir() / "Scripts";
#endif
}

inline auto PluginDataDir()
{
#if defined(_WIN32) || defined(_WIN64)
    return PluginDir() / L"Data";
#else
    return PluginDir() / "Data";
#endif
}

inline auto ExtraFlatsPath()
{
#if defined(_WIN32) || defined(_WIN64)
    return PluginDataDir() / L"ExtraFlats.dat";
#else
    return PluginDataDir() / "ExtraFlats.dat";
#endif
}

inline auto InheritanceMapPath()
{
#if defined(_WIN32) || defined(_WIN64)
    return PluginDataDir() / L"InheritanceMap.dat";
#else
    return PluginDataDir() / "InheritanceMap.dat";
#endif
}

inline const auto& GameVer()
{
    return Core::Runtime::GetHost()->GetProductVer();
}
}
