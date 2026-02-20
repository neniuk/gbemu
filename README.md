# gbemu

## ROM compatibility
Supports for now only DMG ROMs with ROM sizes <= 32KiB, and no RAM.

## Building
```bash
    # Debug
    cmake --preset={linux/macos/windows}-vcpkg-debug
    cmake --build --preset={linux/macos/windows}-vcpkg-debug

    # Release
    cmake --preset={linux/macos/windows}-vcpkg-release
    cmake --build --preset={linux/macos/windows}-vcpkg-release
```

## Running
```bash
    # Debug
    ./build/{linux/macos/windows}-vcpkg-release/gbemu path/to/{rom_name}.gb

    # Release
    ./build/{linux/macos/windows}-vcpkg-release/gbemu path/to/{rom_name}.gb
```
