# Slint/KDFoundation/GLX Example

Example project to reproduce an issue in Slint's C++ with GLX failing to
provide an opengl context or window surface on Linux.

Based off of the KDAB MECAPS research project.

## Building

If you have nix installed:

```bash
nix develop github:the-argus/kdab-flake#packages.x86_64-linux.envs.mecaps
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

Otherwise, you need to install:

- `rustc`
- `libxkbcommon`
- `fontconfig`
- xorg deps:
  - `libxcb`
  - `libX11`
  - `libXCursor`
  - `libXrandr`
  - `libXi`

And run the same cmake commands shown in the nix example.
