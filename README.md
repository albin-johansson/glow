# Gravel

A work in progress 3D graphics engine, developed for fun.

## Purpose

This project is developed for recreational and educational purposes.
The goal is to be able to render nice looking scenes using modern graphics APIs, with a performant and readable code base.

### What this project is not

* A game engine
* A 3D model editor

## Features

* Written in C++23
* ECS architecture
* Support for common 3D model file formats, such as `.dae`, `.fbx`, and `.obj`.
* OpenGL 4.1.0 backend

### Goals

* Spotlights
* Directional lights
* Point lights
* Shadow maps
* Phong shading
* Physically based shading
* Automatic irradiance and reflection maps
* Forward and deferred renderers
* Transparent object support
* View frustum culling
* Fog
* Screen-space ambient occlusion (SSAO)
* Chromatic aberration
* Physically based camera
* Scene loader
* Instanced rendering
* Vulkan backend

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
* [Font Awesome](https://github.com/FortAwesome/Font-Awesome)
* [glad](https://github.com/Dav1dde/glad)
* [glm](https://github.com/g-truc/glm)
* [Dear ImGui](https://github.com/ocornut/imgui)
* [IconFontCppHeaders](https://github.com/juliettef/IconFontCppHeaders)
* [ImGuizmo](https://github.com/CedricGuillemet/ImGuizmo)
* [Magic Enum C++](https://github.com/Neargye/magic_enum)
* [SDL2](https://github.com/libsdl-org/SDL)
* [spdlog](https://github.com/gabime/spdlog)
* [stb](https://github.com/nothings/stb)
* [Vulkan Memory Allocator](https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator) 
* [volk](https://github.com/gnuradio/volk)
