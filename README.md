# gbemu
Simple emulator for the original GB.

## Compatibility
Supports for now only DMG ROMs with ROM sizes <= 32KiB, and no RAM. Does not support SGB or CGB ROMs.

## Prerequisites
- cmake >= 3.28.0
- ninja
- g++/MSVC/clang
- vcpkg ($VCPKG_ROOT set)

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
    ./build/{linux/macos/windows}-vcpkg-debug/gbemu path/to/{rom_name}.gb
```

```bash
    # Release
    ./build/{linux/macos/windows}-vcpkg-release/gbemu path/to/{rom_name}.gb
```
