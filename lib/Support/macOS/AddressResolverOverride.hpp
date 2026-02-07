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
#include <iostream>

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
            // First function in TweakDB method cluster
            { 3062572522, 0x2B79AC0 },
            
            // TweakDB_Load (hash: 3602585178 = 0xD6B1DB5A)
            // Found via LoadOptimized string proximity
            { 3602585178, 0x2B7BE94 },
            
            // TweakDB_TryLoad (hash: 3512345737 = 0xD16A2999)
            // CONFIRMED: References ".tweak" file extension string
            { 3512345737, 0x2B7BAB0 },
            
            // TweakDB_CreateRecord (hash: 838931066 = 0x31FB0F6A)
            // Same as SDK TweakDB_CreateRecord
            { 838931066, 0x2B737AC },
            
            // TweakDBID_Derive (hash: 326438016 = 0x137620C0)
            // Found in TweakDB method cluster
            { 326438016, 0x2B7D228 },
            
            // StatsDataSystem_InitializeRecords (hash: 1299190886 = 0x4D6E8066)
            // DISCOVERED: Medium function at 0x3a939b8 that works with TweakDB records (offset 0xD8)
            // Matches initialization pattern for stat records
            { 1299190886, 0x3A939B8 },
            
            // StatsDataSystem_InitializeParams (hash: 3652194890 = 0xD9B5924A)
            // DISCOVERED: Large function at 0x3a932c4 working with both records (0xD8) and params (0xE8)
            // Called to initialize stat parameter arrays
            { 3652194890, 0x3A932C4 },
            
            // StatsDataSystem_GetStatRange (hash: 1444748215 = 0x5620D3B7)
            // DISCOVERED: Small function at 0x3a94744, uses LDP to load pair (min/max floats)
            // Signature: uint64_t* (*)(void*, uint64_t*, uint32_t)
            { 1444748215, 0x3A94744 },
            
            // StatsDataSystem_GetStatFlags (hash: 3123320294 = 0xBA1CE5E6)
            // DISCOVERED: Small function at 0x3a93f00, returns W0 (32-bit flags)
            // Signature: uint32_t (*)(void*, uint32_t)
            { 3123320294, 0x3A93F00 },
            
            // StatsDataSystem_CheckStatFlag (hash: 2954893634 = 0xB01D2542)
            // DISCOVERED: Small function at 0x3a93e7c with comparison and conditional return
            // Signature: bool (*)(void*, uint32_t, uint32_t)
            { 2954893634, 0x3A93E7C },
            
            // =========================================================================
            // SDK Required Addresses (from RED4ext.SDK AddressHashes.hpp)
            // These are needed by the SDK's internal functions
            // =========================================================================
            
            // CBaseFunction_InternalExecute (hash: 405668637 = 0x1817231D)
            // CONFIRMED from SDK address resolution
            { 405668637, 0x94FE44 },
        };
        
        auto it = addressTable.find(aHash);
        if (it != addressTable.end())
        {
            if (it->second == 0)
            {
                // Placeholder address - log actionable warning
                std::cerr << "[TweakXL::AddressResolver] WARNING: Hash 0x" << std::hex << aHash 
                          << std::dec << " is a placeholder (offset 0x0)" << std::endl;
                std::cerr << "  -> Update lib/Support/macOS/AddressResolverOverride.hpp with discovered offset" << std::endl;
                return 0;
            }
            uintptr_t resolved = imageBase + it->second;
            std::cerr << "[TweakXL::AddressResolver] Resolved 0x" << std::hex << aHash 
                      << " -> 0x" << resolved << std::dec << " (offset: 0x" << std::hex 
                      << it->second << std::dec << ")" << std::endl;
            return resolved;
        }
        
        // Address not in our table - log actionable error
        std::cerr << "[TweakXL::AddressResolver] ERROR: Unknown hash 0x" << std::hex << aHash 
                  << std::dec << " (" << aHash << ")" << std::endl;
        std::cerr << "  -> Add to lib/Support/macOS/AddressResolverOverride.hpp address table" << std::endl;
        std::cerr << "  -> See docs/MACOS_ADDRESS_DISCOVERY.md for discovery methods" << std::endl;
        return 0;
    }
};

} // namespace RED4ext::Detail
