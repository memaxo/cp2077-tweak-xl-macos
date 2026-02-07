# StatsDataSystem Address Discovery - macOS ARM64

## Status

**Current Status:** Partial discovery - Found related functions but actual method addresses still need vtable analysis.

## Discovered Functions

### String References Found

Using `strings` and `find_string_references`, we found:

1. **"StatsDataSystem/OnInitialize"** string at offset `0x106D6C24D`
   - Referenced by function at: `0x103A56AD0` (offset: `0x3A56AD0`)
   - This appears to be a callback handler, not the actual `InitializeRecords` method

2. **"StatsDataSystem/OnInitializeAsync"** string at offset `0x106D6CBAB`
   - Referenced by function at: `0x103A92844` (offset: `0x3A92844`)
   - This appears to be a callback handler, not the actual `InitializeParams` method

3. **"GetStatsDataSystem"** string at offset `0x106CB3DD1`
   - Referenced by function at: `0x101F2F530` (offset: `0x1F2F530`)
   - This is likely the singleton getter function

### Analysis Results

The functions referencing the initialization strings (`0x3A56AD0`, `0x3A92844`) are likely callback handlers that are invoked when the game system initializes. They are NOT the actual `InitializeRecords`/`InitializeParams` methods that TweakXL needs to hook.

## Required Addresses

TweakXL needs these 5 StatsDataSystem function addresses:

| Function | Hash (dec) | Hash (hex) | Signature | Status |
|----------|-----------|------------|-----------|--------|
| StatsDataSystem_InitializeRecords | 1299190886 | 0x4D6E8066 | `void (*)(void* aSystem)` | ❌ Not Found |
| StatsDataSystem_InitializeParams | 3652194890 | 0xD9B5924A | `void (*)(void* aSystem)` | ❌ Not Found |
| StatsDataSystem_GetStatRange | 1444748215 | 0x5620D3B7 | `uint64_t* (*)(void*, uint64_t*, uint32_t)` | ❌ Not Found |
| StatsDataSystem_GetStatFlags | 3123320294 | 0xBA1CE5E6 | `uint32_t (*)(void*, uint32_t)` | ❌ Not Found |
| StatsDataSystem_CheckStatFlag | 2954893634 | 0xB01D2542 | `bool (*)(void*, uint32_t, uint32_t)` | ❌ Not Found |

## Discovery Attempts

### Attempted Methods

1. **String Reference Analysis** ✅
   - Found callback handlers referencing initialization strings
   - Found `GetStatsDataSystem` singleton getter
   - These are NOT the actual method addresses

2. **Call Tracing** ✅
   - Analyzed `GetStatsDataSystem` (0x1F2F530) - makes 19 BL calls
   - Analyzed initialization callbacks - no direct BL calls (likely wrappers)
   - Too many candidate functions to narrow down

3. **Signature Pattern Matching** ✅
   - Created `find_stats_by_signature.py` to search for functions matching expected signatures
   - Found 2000+ candidates matching basic patterns
   - Need more sophisticated filtering

4. **VTable Analysis** ⚠️
   - Attempted to find StatsDataSystem vtable near class name strings
   - Vtables are in `__DATA_CONST` segment with chained fixups
   - Requires decoding chained fixups to read actual vtable addresses

5. **Pattern Comparison with TweakDB** ✅
   - Analyzed TweakDB function patterns (BL counts, sizes)
   - Searched similar address ranges
   - Found many candidates but no definitive matches

## Next Steps for Discovery

### Recommended Approach: Runtime Tracing

The most reliable method would be to:

1. **Hook GetStatsDataSystem at runtime:**
   ```cpp
   // In Frida script or hook
   Interceptor.attach(ptr("0x101F2F530"), {
       onLeave: function(retval) {
           // retval is StatsDataSystem instance
           // Read vtable pointer from instance
           var vtable = retval.readPointer();
           // Read vtable entries
           for (var i = 0; i < 20; i++) {
               var method = vtable.add(i * 8).readPointer();
               console.log("VTable[" + i + "] = " + method);
           }
       }
   });
   ```

2. **Hook initialization callbacks:**
   - Hook the callbacks at 0x3A56AD0 and 0x3A92844
   - Trace what functions they call
   - Identify which ones match the expected signatures

3. **Compare with Windows addresses:**
   - If Windows address database exists, find relative positions
   - Apply similar offsets/patterns to macOS binary

### Alternative: Manual Testing

Since Stats hooks are optional:
1. Leave addresses as placeholders (0x0)
2. TweakXL will work without them
3. Test specific game scenarios that use stats
4. Use breakpoints/debugger to identify which functions are called
5. Map those to the expected signatures

## Impact

**Stats hooks are OPTIONAL** - TweakXL will function without them but will show warnings in logs. Core TweakDB functionality does not depend on StatsDataSystem addresses.

## Tools Used

- `find_stats_system.py` - Custom script for string reference analysis
- `arm64_pattern_scanner.py` - ARM64 instruction pattern matching
- `string_xref.py` - String cross-reference finder
- `otool` - Mach-O binary analysis

## Binary Information

- **Path:** `/Users/jackmazac/Library/Application Support/Steam/steamapps/common/Cyberpunk 2077/Cyberpunk2077.app/Contents/MacOS/Cyberpunk2077`
- **Architecture:** ARM64 (macOS)
- **Base Address:** `0x100000000`
- **__TEXT Segment:** `0x100000000 - 0x106DE0000`

## Related Files

- `/Users/jackmazac/Development/cp2077-tweak-xl/lib/Support/macOS/TweakXLAddressResolver.cpp` - Address resolver implementation
- `/Users/jackmazac/Development/cp2077-tweak-xl/src/Red/StatsDataSystem.hpp` - Function definitions
- `/Users/jackmazac/Development/RED4ext/scripts/dev/find_stats_system.py` - Discovery script
