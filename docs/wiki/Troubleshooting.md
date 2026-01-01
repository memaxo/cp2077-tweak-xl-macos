# Troubleshooting

## Common Issues

### TweakXL Not Loading

**Symptoms:** No TweakXL messages in RED4ext logs

**Solutions:**
1. Verify file structure:
```bash
ls -la "$GAME/red4ext/plugins/TweakXL/"
# Should show: TweakXL.dylib, ExtraFlats.dat, InheritanceMap.dat
```

2. Check RED4ext is working:
```bash
cat "$GAME/red4ext/logs/red4ext.log"
# Should show TweakXL being loaded
```

3. Ensure you're launching with `launch_red4ext.sh`

### Game Crashes on Launch

**Check the logs:**
```bash
cat "$GAME/red4ext/logs/red4ext.log"
cat "$GAME/red4ext/logs/tweakxl.log"
```

**Common causes:**
- Missing data files (InheritanceMap.dat, ExtraFlats.dat)
- Version mismatch between TweakXL and RED4ext
- Corrupted YAML tweak files

### YAML Tweaks Not Applied

1. **Check file location:** Tweaks must be in `<game>/r6/tweaks/`
2. **Check syntax:** YAML is sensitive to indentation
3. **Check logs:** `<game>/red4ext/logs/tweakxl.log`

### Syntax Errors in Tweaks

Valid YAML:
```yaml
Items.Preset_Katana_Default:
  damage: 100  # Must use spaces, not tabs
```

Invalid:
```yaml
Items.Preset_Katana_Default:
	damage: 100  # Tab character - will fail!
```

## Debug Mode

For more verbose logging, create `<game>/r6/tweaks/debug.yaml`:
```yaml
TweakXL:
  debug: true
```

## Getting Help

1. Check [existing issues](https://github.com/memaxo/cp2077-tweak-xl-macos/issues)
2. Search the [original TweakXL wiki](https://github.com/psiberx/cp2077-tweak-xl/wiki)
3. Open a new issue with:
   - macOS version
   - Game version
   - RED4ext log output
   - TweakXL log output
