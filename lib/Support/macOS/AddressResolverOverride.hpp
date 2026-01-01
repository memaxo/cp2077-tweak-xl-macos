#pragma once

/**
 * Address Resolver Override for TweakXL on macOS
 * 
 * This header provides a compile-time specialization of the SDK's AddressResolverOverride
 * that must be included BEFORE any RED4ext SDK headers that use address resolution.
 * 
 * This is necessary because on macOS, the SDK's default address resolution calls into
 * RED4ext.dylib which only has 9 addresses configured. TweakXL needs additional addresses
 * that we provide here.
 */

#include <cstdint>
#include <type_traits>
#include <unordered_map>
#include <mach-o/dyld.h>

namespace RED4ext::Detail
{

/**
 * Specialization of AddressResolverOverride for uint32_t hashes.
 * This overrides the SDK's default behavior of calling RED4ext_ResolveAddress.
 */
template<>
struct AddressResolverOverride<uint32_t> : std::true_type
{
    inline static uintptr_t Resolve(uint32_t aHash)
    {
        // Get image base (cached)
        static const uintptr_t imageBase = reinterpret_cast<uintptr_t>(_dyld_get_image_header(0));
        
        // Address table mapping hash -> offset from image base
        // These offsets are for Cyberpunk 2077 macOS ARM64 v2.3.1
        static const std::unordered_map<uint32_t, uintptr_t> addressTable = {
            // =========================================================================
            // TweakXL Required Addresses (from src/Red/Addresses/Library.hpp)
            // =========================================================================
            
            // Main function (hash: 240386859 = 0x0E54032B)
            // CONFIRMED: Entry point from Mach-O LC_MAIN
            { 240386859, 0x31E18 },
            
            // TweakDB_Init (hash: 3062572522 = 0xB6832FEA)
            // PLACEHOLDER - needs reverse engineering
            { 3062572522, 0x0 },
            
            // TweakDB_Load (hash: 3602585178 = 0xD6B1DB5A)
            // PLACEHOLDER - needs reverse engineering
            { 3602585178, 0x0 },
            
            // TweakDB_TryLoad (hash: 3512345737 = 0xD16A2999)
            // PLACEHOLDER - needs reverse engineering
            { 3512345737, 0x0 },
            
            // TweakDB_CreateRecord (hash: 838931066 = 0x31FB0F6A)
            // PLACEHOLDER - needs reverse engineering
            { 838931066, 0x0 },
            
            // TweakDBID_Derive (hash: 326438016 = 0x137620C0)
            // PLACEHOLDER - needs reverse engineering
            { 326438016, 0x0 },
            
            // StatsDataSystem_InitializeRecords (hash: 1299190886 = 0x4D6E8066)
            // PLACEHOLDER - needs reverse engineering
            { 1299190886, 0x0 },
            
            // StatsDataSystem_InitializeParams (hash: 3652194890 = 0xD9B5924A)
            // PLACEHOLDER - needs reverse engineering
            { 3652194890, 0x0 },
            
            // StatsDataSystem_GetStatRange (hash: 1444748215 = 0x5620D3B7)
            // PLACEHOLDER - needs reverse engineering
            { 1444748215, 0x0 },
            
            // StatsDataSystem_GetStatFlags (hash: 3123320294 = 0xBA1CE5E6)
            // PLACEHOLDER - needs reverse engineering
            { 3123320294, 0x0 },
            
            // StatsDataSystem_CheckStatFlag (hash: 2954893634 = 0xB01D2542)
            // PLACEHOLDER - needs reverse engineering
            { 2954893634, 0x0 },
            
            // =========================================================================
            // SDK Required Addresses (from RED4ext.SDK AddressHashes.hpp)
            // These are needed by the SDK's internal functions
            // =========================================================================
            
            // CBaseFunction_InternalExecute (hash: 405668637 = 0x1817231D)
            // This is used for calling script functions
            // PLACEHOLDER - needs reverse engineering
            { 405668637, 0x0 },
        };
        
        auto it = addressTable.find(aHash);
        if (it != addressTable.end())
        {
            if (it->second == 0)
            {
                // Placeholder address - return 0 to indicate not found
                // This will cause the calling code to handle the missing address
                return 0;
            }
            return imageBase + it->second;
        }
        
        // Address not in our table - return 0
        return 0;
    }
};

} // namespace RED4ext::Detail
