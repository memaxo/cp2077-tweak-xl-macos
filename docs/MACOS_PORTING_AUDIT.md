# TweakXL macOS Port - Testing Audit

## Test Date: 2025-12-31

## Test Results Summary

| Phase | Status | Notes |
|-------|--------|-------|
| Installation | **PASS** | TweakXL.dylib installs correctly |
| Plugin Loading | **PASS** | RED4ext finds and loads TweakXL |
| SDK Address Resolution | **FAIL** | SDK requires 126+ addresses, only 9 available |
| In-Game Testing | **BLOCKED** | Crash before game starts |

## Detailed Findings

### Phase 1: Installation - PASS

TweakXL.dylib successfully installed to:
```
<game>/red4ext/plugins/TweakXL/TweakXL.dylib
```

Directory structure created:
```
red4ext/plugins/TweakXL/
├── TweakXL.dylib (6.6MB, arm64)
├── Data/
└── Scripts/
```

### Phase 2: Plugin Loading - PASS

RED4ext log shows successful plugin discovery:
```
[RED4ext] [info] Loading plugins...
[RED4ext] [info] Loading plugin from '.../red4ext/plugins/TweakXL/TweakXL.dylib'...
```

### Phase 3: SDK Address Resolution - FAIL

When TweakXL initializes, it uses RED4ext.SDK functions that require address resolution.
The SDK calls `RED4ext_ResolveAddress()` to look up function addresses by hash.

**Error encountered:**
```
[RED4ext: Address Resolver] Could not get the 'Query' function for the current mod.
```

**Root cause:**
- SDK requires hash `0x1817231D` = `CBaseFunction_InternalExecute`
- This address is not in our addresses.json file
- The SDK has 126+ address hashes that need resolution
- We only have 9 addresses (RED4ext core hooks)

**RED4ext log showing missing address:**
```
[RED4ext] [warning] Could not resolve hash 0x1817231D - no symbol mapping or address entry
```

### Phase 4: In-Game Testing - BLOCKED

Cannot complete in-game testing due to SDK address resolution failure.
Game terminates before reaching main menu.

## Required Work to Complete TweakXL Port

### Option A: Resolve All SDK Addresses (Comprehensive)
- Reverse engineer 117 additional addresses
- Estimated time: 40-80 hours
- Result: Full TweakXL functionality

### Option B: Custom Address Resolver (Recommended)
- Implement `AddressResolverOverride` in TweakXL
- Only resolve addresses TweakXL actually uses
- Estimated time: 4-8 hours
- Result: TweakXL-specific functionality only

### Option C: Stub Unused SDK Functions
- Identify which SDK functions TweakXL uses
- Provide stub implementations or disable features
- Estimated time: 8-16 hours
- Result: Partial TweakXL functionality

## Key SDK Addresses Needed

Based on TweakXL's usage, these are likely the most critical addresses:

| Function | Hash | Purpose |
|----------|------|---------|
| CBaseFunction_InternalExecute | 0x1817231D | Script function calls |
| TweakDB_Init | 0xB6832FEA | TweakDB initialization |
| TweakDB_Load | 0xD6B1DB5A | TweakDB loading |
| TweakDB_CreateRecord | 0x31FB0F6A | Record creation |
| ResourceDepot_Get | ? | Resource loading |
| RTTI_GetClass | ? | Type information |

## Next Steps

1. **Immediate**: Add TweakDB addresses to manual_addresses_template.json
2. **Short-term**: Implement custom address resolver for TweakXL
3. **Long-term**: Resolve full SDK address set for complete mod compatibility

## Files Modified During Testing

- `/Users/jackmazac/Development/RED4ext/scripts/frida/red4ext_hooks.js` - Added TweakDB hooks (disabled)
- `<game>/r6/tweaks/test_tweak.yaml` - Test YAML tweak file
- `<game>/red4ext/plugins/TweakXL/` - TweakXL installation
