# Slint/KDFoundation/GLX Example

Example project to reproduce an issue in Slint's C++ with GLX failing to
provide an opengl context or window surface on Linux.

This is a small subsection of an early patch to a KDAB R+D project.

The output of `my_application` should be:

```txt
thread '<unnamed>' panicked at 'called `Result::unwrap()` on an `Err` value: Other("Skia OpenGL Renderer: Failed to create OpenGL Window Surface: provided native display isn't supported")', api/cpp/platform.rs:316:10
```

## Additional Issues

- Event loop is not complete so the window will immediately close if created
successfully, while the process will hang.

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
