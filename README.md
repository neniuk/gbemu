# gbemu

## ROM compatibility
Supports for now only DMG ROMs with ROM sizes <= 32KiB, and no RAM.

## Building
```bash
    # Debug
    cmake --preset=linux-vcpkg-debug
    cmake --build --preset=linux-vcpkg-debug

    # Release
    cmake --preset=linux-vcpkg-release
    cmake --build --preset=linux-vcpkg-release
```

## Running
```bash
    # Debug
    ./build/linux-vcpkg-release/gbemu path/to/{rom_name}.gb

    # Release
    ./build/linux-vcpkg-release/gbemu path/to/{rom_name}.gb
```
