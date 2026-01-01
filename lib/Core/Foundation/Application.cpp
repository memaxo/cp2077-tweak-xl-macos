#include "Application.hpp"
#include <iostream>

void Core::Application::Bootstrap()
{
    std::cerr << "[Core::Application] Bootstrap starting..." << std::endl;
    
    if (m_booted)
    {
        std::cerr << "[Core::Application] Already booted, returning" << std::endl;
        return;
    }

    if (!s_discoveryCallbacks.empty())
    {
        std::cerr << "[Core::Application] Processing " << s_discoveryCallbacks.size() << " discovery callbacks" << std::endl;
        for (const auto& callback : s_discoveryCallbacks)
        {
            callback(*this);
        }
        s_discoveryCallbacks.clear();
    }

    m_booted = true;

    std::cerr << "[Core::Application] Calling OnStarting..." << std::endl;
    OnStarting();

    std::cerr << "[Core::Application] Bootstrapping " << GetRegistered().size() << " features..." << std::endl;
    int featureIdx = 0;
    for (const auto& feature : GetRegistered())
    {
        std::cerr << "[Core::Application] Bootstrapping feature #" << featureIdx++ << std::endl;
        feature->OnBootstrap();
    }

    std::cerr << "[Core::Application] Calling OnStarted..." << std::endl;
    OnStarted();
    
    std::cerr << "[Core::Application] Bootstrap complete!" << std::endl;
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
