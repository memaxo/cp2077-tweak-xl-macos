# TweakXL macOS Integration Handoff

## Install Path

Copy the built TweakXL.dylib to:

```
$HOME/Library/Application Support/Steam/steamapps/common/Cyberpunk 2077/red4ext/plugins/TweakXL/TweakXL.dylib
```

## Prerequisites

Ensure RED4ext is installed with the address database at:

```
$HOME/Library/Application Support/Steam/steamapps/common/Cyberpunk 2077/red4ext/bin/x64/cyberpunk2077_addresses.json
```

## Launch Command

Use the RED4ext launcher script (recommended):

```bash
cd "$HOME/Library/Application Support/Steam/steamapps/common/Cyberpunk 2077"
./launch_red4ext.sh
```

This script sets up `DYLD_INSERT_LIBRARIES` for RED4ext + Frida, compiles redscript mods if present, and launches the game.

## Success Log Patterns

Check the RED4ext log for these success indicators:

```bash
# Monitor the log in real-time
tail -f "$HOME/Library/Application Support/Steam/steamapps/common/Cyberpunk 2077/red4ext/logs/red4ext.log"
```

### Expected Success Output

```
[RED4ext] Loading plugin from 'red4ext/plugins/TweakXL/TweakXL.dylib'...
[TweakXL] TweakXL v1.11.3 initialized
[TweakXL::AddressResolver] Resolved 0xe54032b -> 0x100031e18 (offset: 0x31e18)
[TweakXL::AddressResolver] Resolved 0xb6832fea -> 0x102b79ac0 (offset: 0x2b79ac0)
[TweakXL::AddressResolver] Resolved 0xd6b1db5a -> 0x102b7be94 (offset: 0x2b7be94)
[TweakXL::AddressResolver] Resolved 0xd16a2999 -> 0x102b7bab0 (offset: 0x2b7bab0)
[TweakXL::AddressResolver] Resolved 0x31fb0f6a -> 0x102b737ac (offset: 0x2b737ac)
[TweakXL::AddressResolver] Resolved 0x137620c0 -> 0x102b7d228 (offset: 0x2b7d228)
[TweakXL::AddressResolver] Resolved 0x1817231d -> 0x10094fe44 (offset: 0x94fe44)
[StatService] Stats hooks installed successfully. Custom stat types enabled.
# OR (if stats unavailable):
[StatService] Stats hooks unavailable (address not found). Custom stat types will not be supported.
[TweakXL] Registered X tweak directories
[TweakXL] Loaded X tweak files
```

### Acceptance Criteria

- [ ] `[TweakXL] TweakXL v1.11.3 initialized` appears
- [ ] All required TweakDB addresses resolve non-zero (7 addresses)
- [ ] No "One of the required parameters for attaching hook is NULL" errors
- [ ] Stats hooks either attach successfully OR produce graceful warning
- [ ] Tweak files discovered and loaded
- [ ] Game runs without crashes

## Failure Patterns

### Critical Failure (Address Not Found)

```
[TweakXL::AddressResolver] ERROR: Unknown hash 0xXXXXXXXX (YYYYYYYYY)
  -> Add to lib/Support/macOS/AddressResolverOverride.hpp address table
[RED4ext] One of the required parameters for attaching hook is NULL
[RED4ext] TweakXL did not initialize properly, unloading...
```

**Fix:** Update the hash in `lib/Support/macOS/AddressResolverOverride.hpp`

### Stats Unavailable (Acceptable)

```
[StatService] Stats hooks unavailable (address not found). Custom stat types will not be supported.
```

**Status:** Core TweakXL functionality still works. Stats features disabled.

## Quick Verification Commands

```bash
# Check if TweakXL loaded
grep "TweakXL v1.11.3 initialized" \
  "$HOME/Library/Application Support/Steam/steamapps/common/Cyberpunk 2077/red4ext/logs/red4ext.log"

# Check address resolution
grep "TweakXL::AddressResolver" \
  "$HOME/Library/Application Support/Steam/steamapps/common/Cyberpunk 2077/red4ext/logs/red4ext.log"

# Check for critical errors
grep "required parameters for attaching hook is NULL" \
  "$HOME/Library/Application Support/Steam/steamapps/common/Cyberpunk 2077/red4ext/logs/red4ext.log"
```

## Build Command (for reference)

```bash
cd /path/to/cp2077-tweak-xl/build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j8
```

## Release Checklist

- [ ] TweakXL.dylib built successfully (ARM64)
- [ ] All 7 required addresses have non-zero offsets
- [ ] Documentation updated (`ADDRESS_UPDATE_GUIDE.md`, `MACOS_SMOKE_TEST.md`)
- [ ] Smoke test passed (logs show successful initialization)
- [ ] No crashes during 30+ minute gameplay session
- [ ] Tweak files load and apply correctly

## Files Modified in This PR

### Documentation
- `docs/ADDRESS_UPDATE_GUIDE.md` - Address update procedures and sync checklist
- `docs/MACOS_SMOKE_TEST.md` - In-game testing procedures

### Code
- `lib/Support/macOS/AddressResolverOverride.hpp` - Address table with all 12 entries
- `lib/Support/macOS/TweakXLAddressResolver.cpp` - Runtime resolver (deprecated path)
- `src/App/Stats/StatService.cpp` - Optional stats hook handling
- `src/main.cpp` - Init guards and logging

## Notes

- **Required addresses:** 7 (must resolve non-zero for core functionality)
- **Optional addresses:** 5 (StatsDataSystem, graceful degradation if unavailable)
- **Total addresses configured:** 12
- **Address source:** Mix of SDK DB (2) and manual discovery (10)
- **Platform:** macOS ARM64 (Apple Silicon)
- **Game version:** v2.3.1+
