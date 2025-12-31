#pragma once

// MinHook is Windows-only - this file is excluded from macOS builds
#if defined(_WIN32) || defined(_WIN64)

#include "Core/Foundation/Feature.hpp"
#include "Core/Hooking/HookingDriver.hpp"

namespace Support
{
class MinHookProvider
    : public Core::Feature
    , public Core::HookingDriver
{
public:
    MinHookProvider();
    ~MinHookProvider() override;

    bool HookAttach(uintptr_t aAddress, void* aCallback) override;
    bool HookAttach(uintptr_t aAddress, void* aCallback, void** aOriginal) override;
    bool HookDetach(uintptr_t aAddress) override;
};
}

#endif // Windows
