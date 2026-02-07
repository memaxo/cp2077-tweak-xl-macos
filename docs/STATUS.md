# TweakXL macOS Port — Status

> **Last updated:** 2026-02-01  
> **Target game build:** Cyberpunk 2077 macOS **v2.3.1**  
> **Target arch:** Apple Silicon (arm64)  

## Scope

This repo is a macOS-only fork/port of TweakXL (psiberx) for Cyberpunk 2077 on Apple Silicon, built as a **RED4ext `.dylib` plugin** and hooked via **Frida Gadget**.

## Key “source of truth” files

- **Address mappings (macOS)**: `lib/Support/macOS/AddressResolverOverride.hpp` (must be included before any RED4ext SDK headers).
- **Custom hash IDs**: `src/Red/Addresses/Library.hpp` (TweakXL-specific; separate from the SDK’s hashes).
- **Hook wiring**: `src/main.cpp` and services under `src/App/`.

## Quick build / install

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j"$(sysctl -n hw.ncpu)"
```

Copy:

- `TweakXL.dylib` → `<game>/red4ext/plugins/TweakXL/`
- plugin `Data/` and `Scripts/` folders (see `README_MACOS.md`)

## Address update workflow (high level)

Addresses are **game-version specific**. When the game updates, re-discover/update offsets and then validate:

- See `docs/ADDRESS_UPDATE_GUIDE.md`
- See `docs/MACOS_ADDRESS_DISCOVERY.md`

## Related docs

- `README_MACOS.md` (full macOS install + layout guide)
- `MACOS_CHANGES.md` (port deltas vs Windows)

