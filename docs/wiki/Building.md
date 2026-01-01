# Building from Source

## Requirements

- **Xcode Command Line Tools**: `xcode-select --install`
- **Homebrew**: https://brew.sh
- **CMake 3.20+**: `brew install cmake`
- **spdlog**: `brew install spdlog`
- **yaml-cpp**: `brew install yaml-cpp`

## Clone Repository

```bash
git clone --recursive https://github.com/memaxo/cp2077-tweak-xl-macos.git
cd cp2077-tweak-xl-macos
git checkout macos-port
```

**Important:** The `--recursive` flag is required to fetch the RED4ext.SDK submodule.

If you forgot `--recursive`:
```bash
git submodule update --init --recursive
```

## Build

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(sysctl -n hw.ncpu)
```

### Build Options

| Option | Description | Default |
|--------|-------------|---------|
| `-DCMAKE_BUILD_TYPE=Release` | Optimized build | Debug |
| `-DCMAKE_BUILD_TYPE=Debug` | Debug symbols | - |

## Output

The build produces:
- `build/TweakXL.dylib` - The main plugin

## Development Workflow

For rapid iteration:

```bash
# Build
cd build && make -j8

# Install to game
GAME="/Users/YOU/Library/Application Support/Steam/steamapps/common/Cyberpunk 2077"
cp TweakXL.dylib "$GAME/red4ext/plugins/TweakXL/"

# Launch
cd "$GAME" && ./launch_red4ext.sh
```

## Troubleshooting Build Issues

### "spdlog not found"
```bash
brew install spdlog
```

### "yaml-cpp not found"
```bash
brew install yaml-cpp
```

### Submodule issues
```bash
git submodule update --init --recursive
```

### Clean rebuild
```bash
rm -rf build
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(sysctl -n hw.ncpu)
```
