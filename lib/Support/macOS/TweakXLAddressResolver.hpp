#pragma once

#include "Core/Foundation/Feature.hpp"
#include "Core/Memory/AddressResolver.hpp"
#include <unordered_map>
#include <cstdint>

namespace Support
{
/**
 * Custom address resolver for TweakXL on macOS.
 * 
 * This resolver provides ARM64 offsets for the specific functions TweakXL needs,
 * bypassing the SDK's universal resolver which requires 126+ addresses.
 * 
 * Addresses are calculated as: base_address + offset
 * where base_address is obtained from _dyld_get_image_header(0)
 */
class TweakXLAddressResolver : public Core::Feature, public Core::AddressResolver
{
public:
    TweakXLAddressResolver();
    
    uintptr_t ResolveAddress(uint32_t aAddressID) override;
    
    /**
     * Get the image base address (cached for performance)
     */
    static uintptr_t GetImageBase();
    
protected:
    /**
     * Called when the feature is initialized - sets this as the default resolver
     */
    void OnInitialize() override;
    
private:
    /**
     * Initialize the address table with ARM64 offsets
     */
    void InitializeAddressTable();
    
    /**
     * Address table mapping hash -> offset from image base
     */
    std::unordered_map<uint32_t, uintptr_t> m_addressTable;
    
    /**
     * Track which addresses have been requested (for debugging)
     */
    static std::unordered_map<uint32_t, int> s_requestedAddresses;
};
}
