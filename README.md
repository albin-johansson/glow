# Gravel

A work in progress 3D graphics engine, developed for fun.

## Features

* Written in C++23
* Supports common 3D model file formats, such as `.dae`, `.fbx`, and `.obj`.

## Build

This project uses [Vcpkg](https://github.com/microsoft/vcpkg) for dependency management.
Information on Vcpkg triplets can be found [here](https://github.com/microsoft/vcpkg/docs/users/triplets.md).

```
> export VCPKG_ROOT=<path-to-vcpkg>
> mkdir build && cd build
> cmake .. -GNinja \
           -DCMAKE_BUILD_TYPE=<mode> \
           -DVCPKG_TARGET_TRIPLET=<triplet>
> ninja
```

## Dependencies

See [vcpkg.json](vcpkg.json) for the full Vcpkg manifest file.

* [assimp](https://github.com/assimp/assimp)
* [Boost](https://www.boost.org/)
* [entt](https://github.com/skypjack/entt)
* [fmt](https://github.com/fmtlib/fmt)
* [glad](https://github.com/Dav1dde/glad)
* [glm](https://github.com/g-truc/glm)
* [Dear ImGui](https://github.com/ocornut/imgui)
* [Magic Enum C++](https://github.com/Neargye/magic_enum)
* [SDL2](https://github.com/libsdl-org/SDL)
* [spdlog](https://github.com/gabime/spdlog)
* [stb](https://github.com/nothings/stb)
