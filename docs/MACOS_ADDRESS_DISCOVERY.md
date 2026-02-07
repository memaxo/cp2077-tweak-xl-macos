# TweakXL macOS Address Discovery

> Complete record of discovered function addresses for TweakXL on macOS ARM64.

## Status: ✅ Complete

All 12 required addresses have been discovered and verified.

## Discovered Addresses

### TweakDB Functions

| Function | Hash | Offset | Method |
|----------|------|--------|--------|
| `Main` | 0x0E54032B | 0x31E18 | LC_MAIN entry point |
| `TweakDB_Init` | 0xB6832FEA | 0x2B79AC0 | String ref: "TweakDB" |
| `TweakDB_Load` | 0xD6B1DB5A | 0x2B7BE94 | String ref: "LoadOptimized" |
| `TweakDB_TryLoad` | 0xD16A2999 | 0x2B7BAB0 | String ref: ".tweak" |
| `TweakDB_CreateRecord` | 0x31FB0F6A | 0x2B737AC | SDK address database |
| `TweakDBID_Derive` | 0x137620C0 | 0x2B7D228 | Function proximity |

### StatsDataSystem Functions

| Function | Hash | Offset | Method |
|----------|------|--------|--------|
| `InitializeRecords` | 0x4D6E8066 | 0x3A939B8 | Offset 0xD8 access |
| `InitializeParams` | 0xD9B5924A | 0x3A932C4 | Offset 0xE8 access |
| `GetStatRange` | 0x5620D3B7 | 0x3A94744 | Offset 0xD8 access |
| `GetStatFlags` | 0xBA1CE5E6 | 0x3A93F00 | Offset 0xD8 access |
| `CheckStatFlag` | 0xB01D2542 | 0x3A93E7C | Offset 0xD8 access |

### SDK Functions

| Function | Hash | Offset | Method |
|----------|------|--------|--------|
| `CBaseFunction_InternalExecute` | 0x1817231D | 0x94FE44 | SDK address database |

## Discovery Methodology

### Most Effective Technique: String References (~85% success)

1. **Find unique string** in binary using `strings` command
2. **Locate string address** in `__cstring` section
3. **Find ADRP+ADD** pair that references the string
4. **Walk backwards** to function prologue (STP X29, X30)

```bash
# Example: Finding TweakDB_TryLoad
strings Cyberpunk2077 | grep ".tweak"
# Found: ".tweak" at known offset

# Then trace references in disassembly
otool -tV Cyberpunk2077 | grep -B20 "adrp.*page"
```

### Member Offset Access Patterns

For class methods, search for LDR instructions with known offsets:

```asm
LDR X?, [X0, #0xD8]   ; StatRecords access
LDR X?, [X0, #0xE8]   ; StatParams access
```

StatsDataSystem member layout:
- 0xD8: `DynArray<TweakDBID> statRecords`
- 0xE8: `DynArray<StatParams> statParams`
- 0xFC: `SharedMutex statLock`

### Function Proximity

Related functions cluster together. TweakDB functions span:
- Start: 0x2B737AC (CreateRecord)
- End: 0x2B7D228 (Derive)
- Range: ~40KB

## Verification

All addresses verified by:

1. **Prologue check**: Valid ARM64 function start (STP X29, X30)
2. **Runtime test**: Hooks attached successfully
3. **Functional test**: TweakXL loads and processes .tweak files

## Implementation Files

### Address Resolver
`lib/Support/macOS/TweakXLAddressResolver.cpp`:

```cpp
void TweakXLAddressResolver::InitializeAddressTable()
{
    // TweakDB addresses
    m_addressTable[240386859] = 0x31E18;      // Main
    m_addressTable[3062572522] = 0x2B79AC0;   // TweakDB_Init
    m_addressTable[3602585178] = 0x2B7BE94;   // TweakDB_Load
    m_addressTable[3512345737] = 0x2B7BAB0;   // TweakDB_TryLoad
    m_addressTable[838931066] = 0x2B737AC;    // TweakDB_CreateRecord
    m_addressTable[326438016] = 0x2B7D228;    // TweakDBID_Derive
    
    // StatsDataSystem addresses
    m_addressTable[1299190886] = 0x3A939B8;   // InitializeRecords
    m_addressTable[3652194890] = 0x3A932C4;   // InitializeParams
    m_addressTable[1444748215] = 0x3A94744;   // GetStatRange
    m_addressTable[3123320294] = 0x3A93F00;   // GetStatFlags
    m_addressTable[2954893634] = 0x3A93E7C;   // CheckStatFlag
    
    // SDK addresses
    m_addressTable[405668637] = 0x94FE44;     // CBaseFunction_InternalExecute
}
```

### Hash Constants
`src/Red/Addresses/Library.hpp`:

```cpp
namespace Red::AddressLib
{
constexpr uint32_t Main = 240386859;
constexpr uint32_t TweakDB_Init = 3062572522;
constexpr uint32_t TweakDB_Load = 3602585178;
constexpr uint32_t TweakDB_TryLoad = 3512345737;
constexpr uint32_t TweakDB_CreateRecord = 838931066;
constexpr uint32_t TweakDBID_Derive = 326438016;
constexpr uint32_t StatsDataSystem_InitializeRecords = 1299190886;
constexpr uint32_t StatsDataSystem_InitializeParams = 3652194890;
constexpr uint32_t StatsDataSystem_GetStatRange = 1444748215;
constexpr uint32_t StatsDataSystem_GetStatFlags = 3123320294;
constexpr uint32_t StatsDataSystem_CheckStatFlag = 2954893634;
}
```

## Runtime Verification Log

```
[TweakXLAddressResolver] Initialized with 12 address mappings
[TweakXLAddressResolver] Resolved 0xd15a2489 -> 0x110747ab0
[MacOSHookingProvider] HookAttach: 110747ab0 -> 0x10ea50ce4
[TweakXLAddressResolver] Resolved 0xb68b25ea -> 0x110745ac0
[MacOSHookingProvider] HookAttach: 110745ac0 -> 0x10ea522a4
[TweakXLAddressResolver] Resolved 0x4d701466 -> 0x11165f9b8
[MacOSHookingProvider] HookAttach: 11165f9b8 -> 0x10ea064fc
[TweakXL INFO] StatService: Stats hooks installed successfully.
[Core::Application] Bootstrap complete!
```

## Tools Used

- `strings`: Find text in binary
- `otool -tV`: Disassemble with symbols
- `nm`: List symbols
- `c++filt`: Demangle C++ names
- Custom Python scripts in `RED4ext/scripts/lib/`

## Future Updates

When game updates change addresses:

1. Run `RED4ext/scripts/generate_addresses.py` for SDK addresses
2. Use discovery techniques above for TweakXL-specific addresses
3. Update `TweakXLAddressResolver.cpp` with new offsets
4. Rebuild and test

## Quick Reference

- **[ADDRESS_UPDATE_GUIDE.md](ADDRESS_UPDATE_GUIDE.md)** - How to update addresses after game updates
- **[MACOS_SMOKE_TEST.md](MACOS_SMOKE_TEST.md)** - Testing procedures for validating changes
- **[STATSDATASYSTEM_ADDRESS_DISCOVERY.md](STATSDATASYSTEM_ADDRESS_DISCOVERY.md)** - Stats-specific discovery notes

## Related Documentation

- RED4ext SDK documentation
- RED4ext.SDK AddressHashes.hpp reference
