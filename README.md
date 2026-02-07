# TweakXL - macOS Port

> **⚠️ This is a macOS-only fork.** For Windows, use the original: [psiberx/cp2077-tweak-xl](https://github.com/psiberx/cp2077-tweak-xl)

TweakXL is a modding tool and framework to create mods that modify TweakDB, 
a proprietary database of REDengine 4, 
containing essential information about game entities and behavior.

- [YAML](https://github.com/psiberx/cp2077-tweak-xl/wiki/YAML-Tweaks) and [RED](https://github.com/psiberx/cp2077-tweak-xl/wiki/RED-Tweaks) formats for manipulating data in a declarative style 
- [Script extensions](https://github.com/psiberx/cp2077-tweak-xl/wiki/Script-Extensions) to add complex logic and dynamic changes 
- Focused on mods compatibility and maintainability  

## macOS Requirements

- **macOS 14+** (Sonoma) on Apple Silicon (M1/M2/M3)
- **Cyberpunk 2077** for macOS (GOG/Steam)
- **[RED4ext-macos](https://github.com/memaxo/RED4ext-macos)** - macOS mod loader
- **Frida Gadget** integration (included with RED4ext setup)

## Installation

### Pre-built Binary

1. Download `TweakXL.dylib` from [Releases](https://github.com/memaxo/cp2077-tweak-xl-macos/releases)
2. Place in `<game>/red4ext/plugins/TweakXL/`
3. Copy `Data/` folder contents

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

## Usage

1. Place YAML tweaks in `<game>/r6/tweaks/`
2. Launch game using `launch_red4ext.sh`
3. TweakXL automatically loads and applies tweaks

### Example Tweak

Create `r6/tweaks/my_tweak.yaml`:
```yaml
# Increase Johnny's coolness
Character.intriguing_npc_Johnny_Silverhand:
  $base: Character.intriguing_npc
  displayName: "Johnny 'Cool' Silverhand"
```

## Documentation

For documentation, see the original TweakXL wiki:
- [YAML Tweaks](https://github.com/psiberx/cp2077-tweak-xl/wiki/YAML-Tweaks)
- [RED Tweaks](https://github.com/psiberx/cp2077-tweak-xl/wiki/RED-Tweaks)
- [Script Extensions](https://github.com/psiberx/cp2077-tweak-xl/wiki/Script-Extensions)
- [Examples](https://github.com/psiberx/cp2077-tweak-xl/wiki/Examples)

## macOS-Specific Docs

- [docs/STATUS.md](docs/STATUS.md) - Current port status + “what to update” pointers
- [README_MACOS.md](README_MACOS.md) - Full macOS guide
- [MACOS_CHANGES.md](MACOS_CHANGES.md) - Technical changes from Windows version

## Credits

- **psiberx** - Original TweakXL
- **WopsS** - RED4ext framework  
- **Frida** - Dynamic instrumentation toolkit

## License

Same as original TweakXL
