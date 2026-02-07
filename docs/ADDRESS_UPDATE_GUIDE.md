# TweakXL macOS Address Update Guide

Quick reference for updating TweakXL addresses after a Cyberpunk 2077 game update.

## File Locations

| File | Purpose |
|------|---------|
| `lib/Support/macOS/AddressResolverOverride.hpp` | **PRIMARY** - Address table for SDK hooks |
| `lib/Support/macOS/TweakXLAddressResolver.cpp` | Secondary runtime resolver (deprecated) |
| `src/Red/Addresses/Library.hpp` | Hash constants for all addresses |
| `docs/MACOS_ADDRESS_DISCOVERY.md` | Full discovery methodology documentation |

## Quick Update Procedure

### 1. Identify Changed Addresses

After a game update, addresses will likely change. Use RED4ext SDK analysis tools:

```bash
# Navigate to RED4ext scripts directory
cd /path/to/RED4ext/scripts

# Run address generation for SDK addresses
python3 generate_addresses.py /path/to/Cyberpunk2077.app/Contents/MacOS/Cyberpunk2077

# For TweakXL-specific addresses, use custom scripts
python3 dev/find_stats_system.py
python3 dev/find_tweakdb_functions.py
```

### 2. Update Address Tables

Edit `lib/Support/macOS/AddressResolverOverride.hpp`:

```cpp
static const std::unordered_map<uint32_t, uintptr_t> addressTable = {
    // Update these offsets (relative to 0x100000000)
    { 240386859, 0xNEW_OFFSET },       // Main
    { 3062572522, 0xNEW_OFFSET },      // TweakDB_Init
    { 3602585178, 0xNEW_OFFSET },      // TweakDB_Load
    // ... etc
};
```

### 3. Verify Hash Constants

Check `src/Red/Addresses/Library.hpp` matches the hashes in the address table:

```cpp
namespace Red::AddressLib
{
constexpr uint32_t Main = 240386859;
constexpr uint32_t TweakDB_Init = 3062572522;
// ... etc
}
```

### 4. Rebuild

```bash
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j8
```

### 5. Validate

See [MACOS_SMOKE_TEST.md](MACOS_SMOKE_TEST.md) for validation steps.

## Address Categories

### Required (Core TweakXL)

These 7 addresses are **mandatory** for TweakXL to function. They must resolve to valid non-zero offsets.

| Function | Hash (dec) | Hash (hex) | Purpose | Failure Impact | Source |
|----------|-----------|------------|---------|----------------|--------|
| Main | 240386859 | 0x0E54032B | Entry point | **Critical** - Plugin won't load | Manual (LC_MAIN) |
| TweakDB_Init | 3062572522 | 0xB6832FEA | TweakDB init | **Critical** - No tweaks loaded | Manual discovery |
| TweakDB_Load | 3602585178 | 0xD6B1DB5A | Load .bin | **Critical** - No tweaks loaded | Manual discovery |
| TweakDB_TryLoad | 3512345737 | 0xD16A2999 | Load .tweak | **Critical** - No tweaks loaded | Manual discovery |
| TweakDB_CreateRecord | 838931066 | 0x31FB0F6A | Record creation | **Critical** - Tweaks fail | SDK DB |
| TweakDBID_Derive | 326438016 | 0x137620C0 | ID derivation | **Critical** - Tweaks fail | Manual discovery |
| CBaseFunction_InternalExecute | 405668637 | 0x1817231D | Script execution | **Critical** - SDK fails | SDK DB |

### Optional (Stats Modification)

These 5 addresses are **optional**. TweakXL core functionality works without them. If any resolve to 0, StatsDataSystem features are disabled with a warning.

| Function | Hash (dec) | Hash (hex) | Purpose | Failure Impact | Source |
|----------|-----------|------------|---------|----------------|--------|
| StatsDataSystem_InitializeRecords | 1299190886 | 0x4D6E8066 | Stats init | Warning only | Manual discovery |
| StatsDataSystem_InitializeParams | 3652194890 | 0xD9B5924A | Params init | Warning only | Manual discovery |
| StatsDataSystem_GetStatRange | 1444748215 | 0x5620D3B7 | Range getter | Warning only | Manual discovery |
| StatsDataSystem_GetStatFlags | 3123320294 | 0xBA1CE5E6 | Flags getter | Warning only | Manual discovery |
| StatsDataSystem_CheckStatFlag | 2954893634 | 0xB01D2542 | Flag checker | Warning only | Manual discovery |

**Source Legend:**
- **SDK DB**: Address comes from RED4ext SDK's `cyberpunk2077_addresses.json` database
- **Manual Discovery**: Address found via reverse engineering (string references, vtable analysis, etc.)

**Note:** Stats hooks are optional. TweakXL core functionality works without them.

## Discovery Methods

### String References (Most Reliable ~85%)

1. Find unique string in binary:
   ```bash
   strings Cyberpunk2077 | grep "TweakDB"
   ```

2. Locate string address in `__cstring` section

3. Find ADRP+ADD pair referencing the string:
   ```bash
   otool -tV Cyberpunk2077 | grep -B20 "adrp.*page_of_string"
   ```

4. Walk backwards to function prologue (STP X29, X30)

### Function Signatures

Search for ARM64 instruction patterns:

```asm
STP X29, X30, [SP, #-0x10]!  ; Function prologue
LDR X?, [X0, #0xD8]          ; StatsDataSystem::statRecords access
LDR X?, [X0, #0xE8]          ; StatsDataSystem::statParams access
```

### VTable Analysis

For class methods, find vtable in `__DATA_CONST`:

1. Locate class name string in binary
2. Find references (often in vtable metadata)
3. Decode chained fixups to read actual method addresses

## Validation Checklist

- [ ] All required addresses have non-zero offsets
- [ ] Offsets are within `__TEXT` segment (0x100000000 - 0x106DE0000)
- [ ] Functions start with valid ARM64 prologue (STP X29, X30)
- [ ] TweakDB functions are clustered (within ~50KB range)
- [ ] Build completes without errors
- [ ] Dylib loads in game without crashes
- [ ] Tweaks are applied successfully

## Common Issues

### "One of the required parameters for attaching hook is NULL"

**Cause:** Address resolved to 0

**Fix:** Check RED4ext log for which hash failed, update that address in the override table

### Plugin loads but tweaks don't apply

**Cause:** TweakDB hook addresses incorrect

**Fix:** Verify TweakDB function offsets point to actual TweakDB methods (search for ".tweak" string references)

### Crash on startup

**Cause:** Address points to middle of function or data

**Fix:** Verify address points to valid function prologue (STP X29, X30)

## Sync Checklist: When RED4ext DB Updates

When the RED4ext SDK updates its `cyberpunk2077_addresses.json` database, follow this checklist to keep TweakXL addresses in sync:

### 1. SDK-Sourced Addresses (Auto-Sync)

These addresses come from the RED4ext SDK database and should be automatically updated when the DB updates:

- [ ] **CBaseFunction_InternalExecute** (0x1817231D) - Usually in SDK DB
- [ ] **TweakDB_CreateRecord** (0x31FB0F6A) - Often shared with SDK

### 2. TweakXL-Specific Addresses (Manual Verification Required)

These addresses require manual discovery and must be re-verified after each game update:

#### Critical (Required for Core)
- [ ] **Main** (0x0E54032B) - Mach-O LC_MAIN entry point (rarely changes)
- [ ] **TweakDB_Init** (0xB6832FEA) - Verify via "TweakDB" string reference
- [ ] **TweakDB_Load** (0xD6B1DB5A) - Verify via "LoadOptimized" string
- [ ] **TweakDB_TryLoad** (0xD16A2999) - Verify via ".tweak" string reference
- [ ] **TweakDBID_Derive** (0x137620C0) - Check TweakDB function cluster

#### Optional (StatsDataSystem)
- [ ] **StatsDataSystem_InitializeRecords** (0x4D6E8066) - Offset 0xD8 access pattern
- [ ] **StatsDataSystem_InitializeParams** (0xD9B5924A) - Offset 0xE8 access pattern
- [ ] **StatsDataSystem_GetStatRange** (0x5620D3B7) - LDP instruction pattern
- [ ] **StatsDataSystem_GetStatFlags** (0xBA1CE5E6) - Returns W0 pattern
- [ ] **StatsDataSystem_CheckStatFlag** (0xB01D2542) - Comparison pattern

### 3. Verification Steps

After updating addresses:

```bash
# 1. Rebuild
cd build && cmake .. -DCMAKE_BUILD_TYPE=Release && make -j8

# 2. Install
cp build/TweakXL.dylib \
   "$HOME/Library/Application Support/Steam/steamapps/common/Cyberpunk 2077/red4ext/plugins/TweakXL/"

# 3. Launch via RED4ext launcher
cd "$HOME/Library/Application Support/Steam/steamapps/common/Cyberpunk 2077" && ./launch_red4ext.sh

# 4. Check logs for resolution
tail -f "$HOME/Library/Application Support/Steam/steamapps/common/Cyberpunk 2077/red4ext/logs/red4ext.log"
```

**Success Criteria:**
- All required addresses resolve non-zero
- No "required parameter is NULL" errors
- Stats addresses resolve OR produce graceful warnings
- Tweak files are discovered and applied

## Related Documentation

- [MACOS_ADDRESS_DISCOVERY.md](MACOS_ADDRESS_DISCOVERY.md) - Full discovery record
- [STATSDATASYSTEM_ADDRESS_DISCOVERY.md](STATSDATASYSTEM_ADDRESS_DISCOVERY.md) - Stats discovery notes
- [MACOS_SMOKE_TEST.md](MACOS_SMOKE_TEST.md) - Testing procedures
