project(glow-core LANGUAGES CXX)

file(GLOB_RECURSE GLOW_CORE_SOURCE_FILES
     CONFIGURE_DEPENDS
     **/*.cpp
     **/*.hpp
     )

add_library(libglow ${GLOW_CORE_SOURCE_FILES})

set_target_properties(libglow PROPERTIES
                      PREFIX ""
                      )

glow_configure_compile_options(libglow)

target_include_directories(libglow
                           PUBLIC
                           ${PROJECT_SOURCE_DIR}

                           SYSTEM PUBLIC
                           ${GLOW_LIB_DIR}
                           ${Stb_INCLUDE_DIR}
                           ${VULKAN_INCLUDE_DIR}
                           )

target_link_libraries(libglow
                      PUBLIC
                      argparse::argparse
                      assimp::assimp
                      Boost::boost
                      EnTT::EnTT
                      fmt::fmt
                      glad::glad
                      glm::glm
                      imgui
                      imguizmo::imguizmo
                      magic_enum::magic_enum
                      OpenGL::GL
                      spdlog::spdlog
                      SDL2::SDL2
                      Vulkan::Vulkan
                      unofficial::vulkan-memory-allocator::vulkan-memory-allocator
                      )

target_precompile_headers(libglow PRIVATE ${GLOW_PRECOMPILED_HEADERS})
