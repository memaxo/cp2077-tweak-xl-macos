# TweakXL - macOS Port

**macOS Apple Silicon port of [psiberx/cp2077-tweak-xl](https://github.com/psiberx/cp2077-tweak-xl)**

This is a macOS-only fork of TweakXL for Cyberpunk 2077, enabling YAML/RED tweak mods on Apple Silicon Macs.

## Status

For the current port status and the “after game update” workflow entry points, see:

- `docs/STATUS.md`
- `docs/ADDRESS_UPDATE_GUIDE.md`
- `docs/MACOS_ADDRESS_DISCOVERY.md`

## Requirements

- macOS 14+ (Sonoma) on Apple Silicon (M1/M2/M3)
- [RED4ext-macos](https://github.com/memaxo/RED4ext-macos) installed and working
- Frida Gadget integration configured (see RED4ext setup)
- Cyberpunk 2077 for macOS (GOG/Steam)

## Installation

### Pre-built Binary

1. Download `TweakXL.dylib` from Releases
2. Place in `<game>/red4ext/plugins/TweakXL/`
3. Create `Data/` folder with required files (see below)

### Build from Source

```bash
# Clone with submodules
git clone --recursive https://github.com/memaxo/cp2077-tweak-xl-macos.git
cd cp2077-tweak-xl-macos

# Install dependencies
brew install spdlog yaml-cpp

# Build
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(sysctl -n hw.ncpu)

# Install
cp TweakXL.dylib "<game>/red4ext/plugins/TweakXL/"
```

## Directory Structure

```
<game>/
├── red4ext/
│   ├── plugins/
│   │   └── TweakXL/
│   │       ├── TweakXL.dylib
│   │       ├── Data/
│   │       │   ├── InheritanceMap.dat
│   │       │   └── ExtraFlats.dat
│   │       └── Scripts/
│   │           └── *.reds
│   ├── RED4ext.dylib
│   ├── FridaGadget.dylib
│   └── red4ext_hooks.js
├── r6/
│   └── tweaks/
│       └── *.yaml  # Your tweaks go here
└── launch_red4ext.sh
```

## Usage

1. Create YAML tweak files in `<game>/r6/tweaks/`
2. Launch game using `launch_red4ext.sh`
3. TweakXL loads and applies tweaks on game startup

### Example Tweak (r6/tweaks/my_tweak.yaml)

```yaml
# Increase player health
BaseStats.HealthBonus_inline0:
  value: 500
```

## Architecture

```
┌─────────────────────────────────────┐
│           Cyberpunk2077             │
│              (macOS)                │
└───────────────┬─────────────────────┘
                │ DYLD_INSERT_LIBRARIES
        ┌───────┴───────┐
        │               │
┌───────▼───────┐ ┌─────▼─────┐
│ RED4ext.dylib │ │FridaGadget│
│   (loader)    │ │ (hooks)   │
└───────┬───────┘ └───────────┘
        │
        │ Load plugin
┌───────▼───────┐
│ TweakXL.dylib │
│  (plugin)     │
└───────┬───────┘
        │ Hook TweakDB
        │
┌───────▼───────┐
│   Game Data   │
│   (TweakDB)   │
└───────────────┘
```

## Key Differences from Windows

| Feature | Windows | macOS |
|---------|---------|-------|
| Binary format | DLL | dylib |
| Hooking | MinHook | Frida Gadget |
| Module handling | Win32 API | POSIX (dlopen/dladdr) |
| Build system | xmake | CMake |
| SDK | WopsS/RED4ext.SDK | memaxo/RED4ext.SDK-macos |

## Troubleshooting

### TweakXL not loading
- Verify RED4ext is working first (check logs)
- Ensure plugin is in correct path
- Check file permissions

### Tweaks not applying
- Check `<game>/red4ext/logs/` for TweakXL errors
- Verify YAML syntax
- Ensure tweak paths match game data

### Game crashes
- Verify game is properly signed (run resign scripts)
- Check Frida Gadget is installed
- Review crash logs

## Limitations

- No ASI loader support (Windows-only feature)
- REDlib integration disabled
- Some Windows-specific features unavailable

## Credits

- **psiberx** - Original TweakXL
- **WopsS** - RED4ext framework
- **Frida** - Dynamic instrumentation toolkit

## License

Same as original TweakXL - see [LICENSE](LICENSE)
