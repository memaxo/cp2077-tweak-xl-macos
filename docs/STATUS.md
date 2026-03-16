# TweakXL macOS Port — Status

> **Last updated:** 2026-02-21
> **Target game build:** Cyberpunk 2077 macOS **v2.3.1**
> **Target arch:** Apple Silicon (arm64)
> **Status:** Complete and validated

## What works

- **All 7 features bootstrapped** successfully at runtime
- **3 StatService hooks** installed and active
- **Custom stat types** enabled
- **120+ seconds** stable runtime validated
- **Tweak file loading** from `r6/tweaks/` directory
- **Record creation** and **TweakDBID derivation** functional

## Key files

- **Address mappings (macOS)**: `lib/Support/macOS/AddressResolverOverride.hpp`
- **Custom hash IDs**: `src/Red/Addresses/Library.hpp`
- **Hook wiring**: `src/main.cpp` and services under `src/App/`

## Quick build / install

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j"$(sysctl -n hw.ncpu)"
```

Copy:
- `TweakXL.dylib` -> `<game>/red4ext/plugins/TweakXL/`
- Plugin `Data/` and `Scripts/` folders

## Address update workflow

Addresses are game-version specific. When the game updates:
- See `docs/ADDRESS_UPDATE_GUIDE.md`
- See `docs/MACOS_ADDRESS_DISCOVERY.md`

## Related docs

- `README_MACOS.md` (full macOS install + layout guide)
- `MACOS_CHANGES.md` (port deltas vs Windows)
