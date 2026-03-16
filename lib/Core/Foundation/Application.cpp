#include "Application.hpp"
#include <iostream>
#include <cstdlib>

namespace
{
bool IsBootTraceEnabled()
{
    static const bool enabled = []() {
        const char* value = std::getenv("TWEAKXL_BOOT_TRACE");
        return value && value[0] != '\0' && value[0] != '0';
    }();
    return enabled;
}

void BootTrace(const char* aMessage)
{
    if (IsBootTraceEnabled())
    {
        std::cerr << "[Core::Application] " << aMessage << std::endl;
    }
}
}

void Core::Application::Bootstrap()
{
    BootTrace("Bootstrap starting...");
    
    if (m_booted)
    {
        BootTrace("Already booted, returning");
        return;
    }

    if (!s_discoveryCallbacks.empty())
    {
        if (IsBootTraceEnabled())
        {
            std::cerr << "[Core::Application] Processing " << s_discoveryCallbacks.size() << " discovery callbacks"
                      << std::endl;
        }
        for (const auto& callback : s_discoveryCallbacks)
        {
            callback(*this);
        }
        s_discoveryCallbacks.clear();
    }

    m_booted = true;

    BootTrace("Calling OnStarting...");
    OnStarting();

    if (IsBootTraceEnabled())
    {
        std::cerr << "[Core::Application] Bootstrapping " << GetRegistered().size() << " features..." << std::endl;
    }
    int featureIdx = 0;
    for (const auto& feature : GetRegistered())
    {
        if (IsBootTraceEnabled())
        {
            std::cerr << "[Core::Application] Bootstrapping feature #" << featureIdx++ << std::endl;
        }
        feature->OnBootstrap();
    }

    BootTrace("Calling OnStarted...");
    OnStarted();
    
    BootTrace("Bootstrap complete");
}

void Core::Application::Shutdown()
{
    if (!m_booted)
        return;

    OnStopping();

    for (const auto& feature : GetRegistered())
    {
        feature->OnShutdown();
    }

    OnStopped();

    m_booted = false;
}

void Core::Application::OnRegistered(const SharedPtr<Feature>& aFeature)
{
    aFeature->OnRegister();

    if (m_booted)
    {
        aFeature->OnBootstrap();
    }
}

bool Core::Application::Discover(AutoDiscoveryCallback aCallback)
{
    s_discoveryCallbacks.push_back(aCallback);
    return true;
}
