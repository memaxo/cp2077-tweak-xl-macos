# TweakXL macOS Port

## Summary

This PR ports TweakXL to macOS Apple Silicon, enabling YAML/RED tweak mods on Mac.

## Prerequisites

- [RED4ext-macos](https://github.com/memaxo/RED4ext-macos) - macOS mod loader
- [RED4ext.SDK-macos](https://github.com/memaxo/RED4ext.SDK-macos) - macOS SDK

## Key Changes

### Build System
- Replaced xmake with CMake for macOS compatibility
- Uses system spdlog/yaml-cpp from Homebrew
- Header-only RED4ext.SDK integration

### Platform Abstraction
- Created `lib/Core/Platform.hpp` platform switch
- Created `lib/Core/macOS.hpp` with POSIX implementations
- Replaced Win32 APIs with POSIX equivalents (dlopen, dladdr, etc.)

### Removed Dependencies
- **MinHook** → Frida (via RED4ext SDK)
- **WIL** → POSIX headers
- **TiltedCore** → std::allocator
- **hopscotch-map** → std::unordered_map

### Code Fixes
- Fixed iterator `.value()` for std::unordered_map
- Fixed `std::exception` constructor for POSIX
- Added platform guards throughout

## Testing

- [x] CMake configuration works
- [x] Compilation succeeds (0 errors)
- [x] dylib created successfully
- [ ] Runtime testing (pending)

## Files Changed

### New Files
- CMakeLists.txt
- lib/Core/Platform.hpp
- lib/Core/macOS.hpp
- README_MACOS.md
- MACOS_CHANGES.md

### Modified Files
- .gitmodules (SDK → macOS fork)
- config/Project.hpp.in (CMake syntax)
- lib/Core/Win.hpp
- lib/Core/Stl.hpp
- lib/Core/Raw.hpp
- lib/Core/Runtime/*
- lib/Core/Facades/*
- lib/Core/Foundation/*
- lib/Support/MinHook/* (disabled)
- lib/Support/RED4ext/RED4extProvider.hpp
- lib/Red/TypeInfo/Resolving.hpp
- src/main.cpp
- src/App/Application.*
- src/App/Environment.hpp
- src/App/Tweaks/Declarative/Yaml/YamlReader.Template.cpp
- src/Red/TweakDB/Manager.cpp
- src/Red/TweakDB/Reflection.cpp
- src/Red/TweakDB/Source/Parser.cpp
- vendor/RED4ext.SDK/* (ScriptGameInstance-inl.hpp)

## Notes

This is a standalone macOS fork, not a dual-platform PR. The Windows version remains unchanged in the original repository.

## Related

- memaxo/RED4ext-macos
- memaxo/RED4ext.SDK-macos
