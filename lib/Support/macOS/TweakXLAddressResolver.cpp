#include "TweakXLAddressResolver.hpp"
#include <mach-o/dyld.h>
#include <iostream>

namespace Support
{

std::unordered_map<uint32_t, int> TweakXLAddressResolver::s_requestedAddresses;

TweakXLAddressResolver::TweakXLAddressResolver()
{
    InitializeAddressTable();
}

void TweakXLAddressResolver::OnInitialize()
{
    // Set this resolver as the default for Core::AddressResolver
    AddressResolver::SetDefault(*this);
    std::cerr << "[TweakXLAddressResolver] Registered as default address resolver" << std::endl;
}

uintptr_t TweakXLAddressResolver::GetImageBase()
{
    static const uintptr_t base = reinterpret_cast<uintptr_t>(_dyld_get_image_header(0));
    return base;
}

void TweakXLAddressResolver::InitializeAddressTable()
{
    // =========================================================================
    // TweakXL Required Addresses (from src/Red/Addresses/Library.hpp)
    // =========================================================================
    // These are the 11 functions TweakXL needs to operate.
    // Offsets are relative to the __TEXT segment base (0x100000000)
    //
    // NOTE: Most of these offsets are PLACEHOLDERS and need to be found via
    // reverse engineering the macOS ARM64 binary.
    // =========================================================================
    
    // Main function (hash: 240386859 = 0x0E54032B)
    // CONFIRMED: Entry point from Mach-O LC_MAIN
    m_addressTable[240386859] = 0x31E18;
    
    // TweakDB_Init (hash: 3062572522 = 0xB6832FEA)
    // game::data::TweakDB::Init - TweakDB initialization
    // CONFIRMED: First function in TweakDB method cluster
    m_addressTable[3062572522] = 0x2B79AC0;
    
    // TweakDB_Load (hash: 3602585178 = 0xD6B1DB5A)
    // game::data::TweakDB::LoadOptimized - Load TweakDB from .bin file
    // CONFIRMED: Found via LoadOptimized string proximity
    m_addressTable[3602585178] = 0x2B7BE94;
    
    // TweakDB_TryLoad (hash: 3512345737 = 0xD16A2999)
    // CONFIRMED: References ".tweak" file extension string
    m_addressTable[3512345737] = 0x2B7BAB0;
    
    // TweakDB_CreateRecord (hash: 838931066 = 0x31FB0F6A)
    // game::data::AddRecord - Create TweakDB record
    // CONFIRMED: Same as SDK TweakDB_CreateRecord
    m_addressTable[838931066] = 0x2B737AC;
    
    // TweakDBID_Derive (hash: 326438016 = 0x137620C0)
    // CONFIRMED: Found in TweakDB method cluster
    m_addressTable[326438016] = 0x2B7D228;
    
    // StatsDataSystem_InitializeRecords (hash: 1299190886 = 0x4D6E8066)
    // DISCOVERED: Medium function at 0x3a939b8 that works with TweakDB records (offset 0xD8)
    // Matches initialization pattern for stat records
    m_addressTable[1299190886] = 0x3A939B8;
    
    // StatsDataSystem_InitializeParams (hash: 3652194890 = 0xD9B5924A)
    // DISCOVERED: Large function at 0x3a932c4 working with both records (0xD8) and params (0xE8)
    // Called to initialize stat parameter arrays
    m_addressTable[3652194890] = 0x3A932C4;
    
    // StatsDataSystem_GetStatRange (hash: 1444748215 = 0x5620D3B7)
    // DISCOVERED: Small function at 0x3a94744, uses LDP to load pair (min/max floats)
    // Signature: uint64_t* (*)(void*, uint64_t*, uint32_t)
    m_addressTable[1444748215] = 0x3A94744;
    
    // StatsDataSystem_GetStatFlags (hash: 3123320294 = 0xBA1CE5E6)
    // DISCOVERED: Small function at 0x3a93f00, returns W0 (32-bit flags)
    // Signature: uint32_t (*)(void*, uint32_t)
    m_addressTable[3123320294] = 0x3A93F00;
    
    // StatsDataSystem_CheckStatFlag (hash: 2954893634 = 0xB01D2542)
    // DISCOVERED: Small function at 0x3a93e7c with comparison and conditional return
    // Signature: bool (*)(void*, uint32_t, uint32_t)
    m_addressTable[2954893634] = 0x3A93E7C;
    
    // =========================================================================
    // Additional SDK addresses that may be needed
    // These are from RED4ext.SDK AddressHashes.hpp
    // =========================================================================
    
    // CBaseFunction_InternalExecute (hash: 405668637 = 0x1817231D)
    // This is the address that was failing - script function execution
    // CONFIRMED: From SDK address resolution
    m_addressTable[405668637] = 0x94FE44;
    
    std::cerr << "[TweakXLAddressResolver] Initialized with " << m_addressTable.size() 
              << " address mappings (some may be placeholders)" << std::endl;
}

uintptr_t TweakXLAddressResolver::ResolveAddress(uint32_t aAddressID)
{
    // Track requested addresses for debugging
    s_requestedAddresses[aAddressID]++;
    
    auto it = m_addressTable.find(aAddressID);
    if (it != m_addressTable.end())
    {
        uintptr_t offset = it->second;
        
        if (offset == 0)
        {
            // Placeholder address - log warning
            std::cerr << "[TweakXLAddressResolver] WARNING: Address 0x" << std::hex << aAddressID 
                      << std::dec << " (" << aAddressID << ") is a placeholder (offset 0x0)" << std::endl;
            return 0;
        }
        
        uintptr_t resolved = GetImageBase() + offset;
        std::cerr << "[TweakXLAddressResolver] Resolved 0x" << std::hex << aAddressID 
                  << " -> 0x" << resolved << std::dec << std::endl;
        return resolved;
    }
    
    // Address not in our table - log error
    std::cerr << "[TweakXLAddressResolver] ERROR: Unknown address hash 0x" << std::hex << aAddressID 
              << std::dec << " (" << aAddressID << ")" << std::endl;
    std::cerr << "  This address needs to be added to TweakXLAddressResolver" << std::endl;
    
    return 0;
}

} // namespace Support
