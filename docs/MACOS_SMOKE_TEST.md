# TweakXL macOS Smoke Test Plan

Minimal test plan for verifying TweakXL loads and applies tweaks correctly on macOS.

## Test Execution Status

**Last Updated:** 2025-01-31  
**Status:** Ready for execution  
**Build:** TweakXL.dylib (1.4MB, ARM64) built successfully  
**Addresses:** All 12 addresses configured (7 required, 5 optional)

**Note:** This test plan requires manual execution in-game. An AI agent cannot launch the game.  
**Next Step:** User to run the test procedure below and verify logs match expected output.

## Prerequisites

- Cyberpunk 2077 macOS v2.3.1+ installed
- RED4ext.dylib installed in `red4ext/` directory
- TweakXL.dylib built successfully

## Test 1: Plugin Loading

### Setup

1. Copy built TweakXL.dylib to game:
   ```bash
   cp build/TweakXL.dylib "~/Library/Application Support/Steam/steamapps/common/Cyberpunk 2077/red4ext/plugins/TweakXL/"
   ```

2. Create test tweak file:
   ```bash
   mkdir -p "~/Library/Application Support/Steam/steamapps/common/Cyberpunk 2077/r6/tweaks/smoke_test/"
   cat > "~/Library/Application Support/Steam/steamapps/common/Cyberpunk 2077/r6/tweaks/smoke_test/test.yaml" << 'EOF'
   package: SmokeTest
   
   TestRecord:
     $type: gamedataStatModifierGroup_Record
     drawBasedOnStat: true
     stat: BaseStats.Health
   EOF
   ```

### Execution

1. Launch game via RED4ext launcher (recommended):
   ```bash
   cd "$HOME/Library/Application Support/Steam/steamapps/common/Cyberpunk 2077" && ./launch_red4ext.sh
   ```
   
   This launcher script sets up `DYLD_INSERT_LIBRARIES` for RED4ext + Frida, compiles redscript mods if present, and launches the game.

2. Check RED4ext logs:
   ```bash
   tail -f "$HOME/Library/Application Support/Steam/steamapps/common/Cyberpunk 2077/red4ext/logs/red4ext.log"
   ```

### Expected Output

```
[RED4ext] Loading plugin from 'red4ext/plugins/TweakXL/TweakXL.dylib'...
[TweakXL] TweakXL v1.11.3 initialized
[TweakXL] Registered X tweak directories
[TweakXL] Loaded X tweak files
```

### Pass Criteria

- [ ] No "One of the required parameters for attaching hook is NULL" errors
- [ ] TweakXL initializes without crashes
- [ ] "Bootstrap complete" message appears

## Test 2: Address Resolution

### Check Logs

Look for address resolution messages:

```bash
grep "TweakXL::AddressResolver" "$HOME/Library/Application Support/Steam/steamapps/common/Cyberpunk 2077/red4ext/logs/red4ext.log"
```

### Expected Output

```
[TweakXL::AddressResolver] Resolved 0xe54032b -> 0x100031e18 (offset: 0x31e18)
[TweakXL::AddressResolver] Resolved 0xb6832fea -> 0x102b79ac0 (offset: 0x2b79ac0)
[TweakXL::AddressResolver] Resolved 0xd6b1db5a -> 0x102b7be94 (offset: 0x2b7be94)
[TweakXL::AddressResolver] Resolved 0xd16a2999 -> 0x102b7bab0 (offset: 0x2b7bab0)
[TweakXL::AddressResolver] Resolved 0x31fb0f6a -> 0x102b737ac (offset: 0x2b737ac)
[TweakXL::AddressResolver] Resolved 0x137620c0 -> 0x102b7d228 (offset: 0x2b7d228)
```

### Pass Criteria

- [ ] All required TweakDB addresses resolve without ERROR
- [ ] Stats addresses resolve OR produce expected warnings
- [ ] No "Unknown hash" errors for required functions

## Test 3: Tweak Application

### Create Test Tweak

```yaml
package: SmokeTest

TestItem:
  $base: Items.Preset_Katana_Wakizashi_Default
  $name: TestSmokeItem
  localizedName: "Smoke Test Katana"
  description: "This item verifies TweakXL is working"
```

Save to: `r6/tweaks/smoke_test/item.yaml`

### In-Game Verification

1. Load save game or start new game
2. Open console (if available) or check if item appears in inventory
3. Look for tweak loading messages in game log

### Pass Criteria

- [ ] No errors loading tweak files
- [ ] Tweaks are applied to game state
- [ ] Game runs without crashes

## Test 4: Stats System (Optional)

If StatsDataSystem addresses are configured:

### Check Logs

```bash
grep "StatService" "$HOME/Library/Application Support/Steam/steamapps/common/Cyberpunk 2077/red4ext/logs/red4ext.log"
```

### Expected Output (with stats)

```
[StatService] Stats hooks installed successfully. Custom stat types enabled.
```

### Expected Output (without stats)

```
[StatService] Stats hooks unavailable (address not found). Custom stat types will not be supported.
```

### Pass Criteria

- [ ] StatService initializes without crashes
- [ ] Either success message OR graceful warning
- [ ] Core TweakXL functionality works regardless

## Test 5: Error Scenarios

### Missing Address Test

1. Temporarily set an address to 0 in `AddressResolverOverride.hpp`:
   ```cpp
   { 3512345737, 0x0 },  // TweakDB_TryLoad as placeholder
   ```

2. Rebuild and install

3. Launch game

### Expected Behavior

```
[TweakXL::AddressResolver] WARNING: Hash 0xd16a2999 is a placeholder (offset 0x0)
  -> Update lib/Support/macOS/AddressResolverOverride.hpp with discovered offset
[RED4ext] One of the required parameters for attaching hook is NULL
[RED4ext] TweakXL did not initialize properly, unloading...
```

### Pass Criteria

- [ ] Clear actionable error message shows which hash failed
- [ ] Plugin fails gracefully (no crash)
- [ ] Error message points to correct file for fix

## Regression Tests

After any address update, verify:

1. **Main hook still attaches**: Check for "Main called with reason: 0" in logs
2. **TweakDB loads**: Check for "TryLoadTweakDB" hook messages
3. **No double init**: Should only see one "Application created" message
4. **Clean shutdown**: No crashes on game exit

## Automated Test Script

```bash
#!/bin/bash
# smoke_test.sh

GAME_DIR="$HOME/Library/Application Support/Steam/steamapps/common/Cyberpunk 2077"
LOG_FILE="$GAME_DIR/red4ext/logs/red4ext.log"

echo "=== TweakXL macOS Smoke Test ==="

# Test 1: Check plugin loads
if grep -q "TweakXL v1.11.3 initialized" "$LOG_FILE"; then
    echo "✓ Test 1 PASS: Plugin loaded"
else
    echo "✗ Test 1 FAIL: Plugin not initialized"
    exit 1
fi

# Test 2: Check no critical errors
if grep -q "One of the required parameters for attaching hook is NULL" "$LOG_FILE"; then
    echo "✗ Test 2 FAIL: Hook attachment failed"
    exit 1
else
    echo "✓ Test 2 PASS: All required hooks attached"
fi

# Test 3: Check address resolution
RESOLVED=$(grep -c "TweakXL::AddressResolver] Resolved" "$LOG_FILE")
if [ "$RESOLVED" -ge 6 ]; then
    echo "✓ Test 3 PASS: $RESOLVED addresses resolved"
else
    echo "✗ Test 3 FAIL: Only $RESOLVED addresses resolved (expected 6+)"
    exit 1
fi

echo "=== All tests passed ==="
```

## Troubleshooting

### Test 1 Fails: Plugin won't load

- Check RED4ext.dylib is installed
- Verify TweakXL.dylib architecture: `file TweakXL.dylib` (should be arm64)
- Check code signing: `codesign -dv TweakXL.dylib`

### Test 2 Fails: Address resolution errors

- Check which hash failed in logs
- Update offset in `AddressResolverOverride.hpp`
- Rebuild and reinstall

### Test 3 Fails: Tweaks not applying

- Check tweak file syntax is valid YAML
- Verify TweakDB_TryLoad address points to function with ".tweak" string reference
- Check game log for parsing errors

### Test 4 Fails: Stats system crash

- Stats hooks are optional - disable by setting addresses to 0x0
- Or verify StatsDataSystem addresses point to correct functions

## Sign-Off Checklist

Before releasing updated addresses:

- [ ] All smoke tests pass
- [ ] No crashes during 30+ minute gameplay session
- [ ] Tweak files load without errors
- [ ] Logs are clean (no ERROR level messages)
- [ ] Documentation updated with new offsets
- [ ] Git changes committed and tagged
