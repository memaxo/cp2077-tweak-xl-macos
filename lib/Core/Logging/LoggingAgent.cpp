#include "LoggingAgent.hpp"

#include <cassert>
#include <iostream>

namespace
{
Core::LoggingDriver* s_driver = nullptr;
}

void Core::LoggingAgent::SetDriver(Core::LoggingDriver& aDriver)
{
    std::cerr << "[LoggingAgent] SetDriver called" << std::endl;
    s_driver = &aDriver;
}

Core::LoggingDriver& Core::LoggingAgent::GetLoggingDriver()
{
    if (!s_driver) {
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
