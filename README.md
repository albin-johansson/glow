# Gravel

[![CI](https://github.com/albin-johansson/gravel/actions/workflows/ci.yml/badge.svg)](https://github.com/albin-johansson/gravel/actions/workflows/ci.yml)

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
* Vulkan 1.2 backend

## Build

This project uses [Vcpkg](https://github.com/microsoft/vcpkg) for dependency management.
However, you will need to install the [Vulkan SDK](https://vulkan.lunarg.com/) to build the Vulkan backend.
You can verify your Vulkan installation by checking the value of the `VULKAN_SDK` environment variable.
Information on Vcpkg triplets can be found [here](https://github.com/microsoft/vcpkg/docs/users/triplets.md).

```
> export VCPKG_ROOT=<path-to-vcpkg>
> mkdir build && cd build
> cmake .. -GNinja \
           -DCMAKE_BUILD_TYPE=<mode> \
           -DVCPKG_TARGET_TRIPLET=<triplet>
> ninja
```

## Usage

```
Usage: gravel [options]
Options:
   --api, -a        'OpenGL' or 'Vulkan', defaults to 'OpenGL' (optional)
   --env, -e        Path to an environment texture to load at startup (optional)
   --models, -m     Space-separated list of model file paths to load at startup (optional)
   --log, -l        Verbosity of log output, valid values are within [0, 6], defaults to 4 (optional)
```

Some examples are provided below.

```shell
# Use default graphics API, and no initial environment texture or models
> ./gravel

# Use a specific graphics API 
> ./gravel --api Vulkan

# Disable all logging output
> ./gravel --log 0

# Load an environment texture and several models
> ./gravel --env textures/env.hdr --models models/teapot.dae models/kettle.fbx
```

## Dependencies

See [vcpkg.json](vcpkg.json) for the full Vcpkg manifest file.

* [argparse](https://github.com/p-ranav/argparse)
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
