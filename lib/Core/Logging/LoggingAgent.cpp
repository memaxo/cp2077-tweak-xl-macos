#include "LoggingAgent.hpp"

#include <cassert>
#include <iostream>
#include <cstdlib>

namespace
{
Core::LoggingDriver* s_driver = nullptr;

bool IsBootTraceEnabled()
{
    static const bool enabled = []() {
        const char* value = std::getenv("TWEAKXL_BOOT_TRACE");
        return value && value[0] != '\0' && value[0] != '0';
    }();
    return enabled;
}
}

void Core::LoggingAgent::SetDriver(Core::LoggingDriver& aDriver)
{
    if (IsBootTraceEnabled())
    {
        std::cerr << "[LoggingAgent] SetDriver called" << std::endl;
    }
    s_driver = &aDriver;
}

Core::LoggingDriver& Core::LoggingAgent::GetLoggingDriver()
{
    if (!s_driver && IsBootTraceEnabled()) {
        std::cerr << "[LoggingAgent] ERROR: GetLoggingDriver called but driver is null!" << std::endl;
    }
    assert(s_driver);
    return *s_driver;
}

void Core::LoggingAgent::LogFlush()
{
    if (s_driver)
        s_driver->LogFlush();
}
