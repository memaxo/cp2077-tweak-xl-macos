# TweakXL macOS Porting Audit

**Date:** December 31, 2025  
**Status:** Analysis Complete

---

## Executive Summary

TweakXL is a RED4ext plugin that modifies Cyberpunk 2077's TweakDB (game database). Porting to macOS requires:

1. **Build system migration** (xmake → CMake)
2. **MinHook replacement** (Windows-only) → Frida or RED4ext hooking
3. **Windows API abstraction** (WIL, version info, module loading)
4. **SDK replacement** with macOS fork

### Estimated Effort: **Medium** (3-5 days)

---

## Architecture Overview

```
TweakXL
├── RED4ext Plugin Interface (main.cpp)
│   └── DllMain for ASI loading (Windows-specific)
├── Core Library (lib/Core/)
│   ├── Hooking (MinHook-based)
│   ├── Runtime (Windows module APIs)
│   └── Memory (Address resolution)
├── Support Providers (lib/Support/)
│   ├── MinHookProvider (NEEDS REPLACEMENT)
│   ├── RED4extProvider (uses RED4ext SDK hooking)
│   └── SpdlogProvider
└── App Logic (src/App/)
    ├── TweakService (main logic)
    └── Tweaks/ (YAML/RED parsing - portable)
```

---

## Platform-Specific Code Analysis

### 🔴 Critical - Must Change

#### 1. MinHook Dependency
**Files:** `lib/Support/MinHook/MinHookProvider.cpp`

```cpp
#include <MinHook.h>  // Windows-only hooking library

MH_Initialize();
MH_CreateHook(address, callback, &original);
MH_EnableHook(address);
```

**Solution:** Replace with `RED4extProvider` which uses RED4ext SDK's hooking (already Frida-compatible on macOS).

#### 2. DllMain Entry Point
**File:** `src/main.cpp`

```cpp
BOOL APIENTRY DllMain(HMODULE aHandle, DWORD aReason, LPVOID)
{
    // ASI loader support
}
```

**Solution:** Add `__attribute__((constructor/destructor))` for macOS dylib loading.

#### 3. Windows Module APIs
**File:** `lib/Core/Runtime/HostImage.cpp`

```cpp
GetModuleHandleW(nullptr);
wil::GetModuleFileNameW(handle, filePath);
GetFileVersionInfoSizeW(filePath.c_str(), nullptr);
GetFileVersionInfoW(filePath.c_str(), 0, size, data.get());
VerQueryValueW(data.get(), L"\\", ...);
```

**Solution:** macOS equivalents using `dladdr`, `_NSGetExecutablePath`, `CFBundle` version APIs.

#### 4. WIL (Windows Implementation Library)
**File:** `lib/Core/Win.hpp`

```cpp
#include <wil/stl.h>
#include <wil/win32_helpers.h>
```

**Solution:** Create `lib/Core/Platform.hpp` with macOS equivalents or remove WIL dependency.

### 🟡 Moderate - Conditional Compilation

#### 5. Wide String Usage
**Files:** Multiple

```cpp
std::wstring filePath;
IsEXE(L"Cyberpunk2077.exe");
aInfo->name = App::Project::NameW;  // Wide string constants
```

**Solution:** Use narrow strings on macOS, platform guards around wide string usage.

#### 6. System Links
**File:** `xmake.lua`

```lua
add_syslinks("Version", "User32")  -- Windows libraries
```

**Solution:** CMake with conditional linking (`CoreFoundation`, `dl` on macOS).

#### 7. Address Resolution
**File:** `lib/Core/Raw.hpp`

```cpp
static const auto base = reinterpret_cast<uintptr_t>(GetModuleHandleW(nullptr));
```

**Solution:** Use `_dyld_get_image_header(0)` on macOS.

### 🟢 Portable - No Changes Needed

- **YAML parsing** (`yaml-cpp`) - Cross-platform
- **PEGTL** (Parser Expression Grammar) - Header-only, portable
- **nameof** - Header-only, portable
- **semver** - Header-only, portable
- **TweakDB logic** - Pure C++, uses RED4ext SDK types
- **File I/O** - Standard `<filesystem>` usage

---

## Dependencies Analysis

| Dependency | Type | macOS Status |
|------------|------|--------------|
| RED4ext.SDK | Submodule | ⚠️ Replace with `memaxo/RED4ext.SDK-macos` |
| hopscotch-map | Package (xmake) | ✅ Portable (header-only) |
| minhook | Package (xmake) | ❌ Windows-only → Remove |
| spdlog | Package (xmake) | ✅ Portable |
| tiltedcore | Package (xmake) | ⚠️ May have Windows code |
| yaml-cpp | Package (xmake) | ✅ Portable |
| nameof | Submodule | ✅ Header-only |
| pegtl | Submodule | ✅ Header-only |
| semver | Submodule | ✅ Header-only |
| wil | Submodule | ❌ Windows-only → Remove/Replace |

---

## Required Changes

### Phase 1: Build System (Day 1)

1. **Create CMakeLists.txt** replacing xmake.lua
2. **Configure dependencies:**
   - Replace RED4ext.SDK submodule URL
   - Remove MinHook, WIL
   - Add platform detection

### Phase 2: Platform Abstraction (Day 2)

1. **Create `lib/Core/Platform.hpp`:**
   ```cpp
   #if defined(_WIN32)
   #include "Core/Win.hpp"
   #else
   #include "Core/macOS.hpp"  // NEW
   #endif
   ```

2. **Create `lib/Core/macOS.hpp`:**
   - `GetModuleHandle` → `dlopen/dladdr`
   - `GetModuleFileName` → `_NSGetExecutablePath`
   - `GetFileVersionInfo` → `CFBundle` APIs

3. **Update `lib/Core/Runtime/HostImage.cpp`:**
   - Platform-specific module loading
   - Version extraction from Info.plist

### Phase 3: Hooking System (Day 2-3)

1. **Modify `lib/Support/MinHook/MinHookProvider.cpp`:**
   - Add platform guard
   - On macOS, redirect to RED4extProvider (which uses Frida)

2. **Or simpler:** Always use `RED4extProvider` on macOS:
   ```cpp
   #ifdef __APPLE__
   // MinHook not available, RED4extProvider handles hooking via RED4ext SDK
   #endif
   ```

### Phase 4: Entry Point (Day 3)

1. **Update `src/main.cpp`:**
   ```cpp
   #ifdef __APPLE__
   __attribute__((constructor))
   static void TweakXL_Init() {
       // Initialization code
   }
   
   __attribute__((destructor))
   static void TweakXL_Fini() {
       // Cleanup code
   }
   #else
   BOOL APIENTRY DllMain(...) { ... }
   #endif
   ```

### Phase 5: Testing (Day 4-5)

1. Build and verify compilation
2. Test RED4ext plugin loading
3. Test TweakDB modifications
4. Test YAML/RED file parsing

---

## Files to Modify

| File | Changes |
|------|---------|
| `xmake.lua` → `CMakeLists.txt` | Complete rewrite |
| `src/main.cpp` | Add macOS constructor/destructor |
| `src/pch.hpp` | Add platform guards |
| `lib/Core/Win.hpp` | Rename to Platform.hpp, add macOS |
| `lib/Core/Raw.hpp` | Platform-specific base address |
| `lib/Core/Runtime/HostImage.cpp` | macOS module/version APIs |
| `lib/Core/Runtime/ModuleImage.cpp` | macOS module path |
| `lib/Core/Facades/Runtime.hpp` | Remove wide string from macOS |
| `lib/Support/MinHook/*` | Platform guards or remove |
| `src/App/Application.hpp` | Remove HMODULE on macOS |

---

## New Files to Create

| File | Purpose |
|------|---------|
| `CMakeLists.txt` | Build system |
| `lib/Core/macOS.hpp` | macOS platform compatibility |
| `lib/Core/macOS.cpp` | macOS implementations |
| `cmake/` | CMake modules |

---

## Address Library

TweakXL uses these addresses (from `src/Red/Addresses/Library.hpp`):

| Hash | Function |
|------|----------|
| `240386859` | Main |
| `1299190886` | StatsDataSystem_InitializeRecords |
| `3652194890` | StatsDataSystem_InitializeParams |
| `1444748215` | StatsDataSystem_GetStatRange |
| `3123320294` | StatsDataSystem_GetStatFlags |
| `2954893634` | StatsDataSystem_CheckStatFlag |
| `3062572522` | TweakDB_Init |
| `3602585178` | TweakDB_Load |
| `3512345737` | TweakDB_TryLoad |
| `838931066` | TweakDB_CreateRecord |
| `326438016` | TweakDBID_Derive |

**Action:** Add these to RED4ext's address database for macOS.

---

## Risk Assessment

| Risk | Likelihood | Impact | Mitigation |
|------|------------|--------|------------|
| MinHook removal breaks hooking | Low | High | Use RED4extProvider (already works) |
| WIL removal causes issues | Medium | Medium | Create minimal macOS equivalents |
| tiltedcore has Windows code | Medium | Low | Evaluate or remove dependency |
| Address resolution fails | Low | High | Already solved in RED4ext port |

---

## Recommendation

**Start with TweakXL** because:
1. It uses RED4ext SDK's hooking API → Already Frida-compatible
2. Most code is portable C++ (YAML, TweakDB logic)
3. Windows-specific code is isolated in `lib/Core/`
4. Smaller scope than ArchiveXL

**Key insight:** TweakXL's `RED4extProvider` already wraps RED4ext's hooking API, which means it will automatically use Frida on macOS without code changes to the hooking logic itself.

---

## Next Steps

1. Fork `psiberx/cp2077-tweak-xl`
2. Create CMakeLists.txt
3. Replace RED4ext.SDK submodule
4. Add platform abstraction layer
5. Test build on macOS
6. Verify hooks work via RED4ext's Frida integration
