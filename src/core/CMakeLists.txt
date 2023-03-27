project(gravel-core LANGUAGES CXX)

file(GLOB_RECURSE GRAVEL_CORE_SOURCE_FILES
     CONFIGURE_DEPENDS
     **/*.cpp
     **/*.hpp
     )

add_library(libgravel ${GRAVEL_CORE_SOURCE_FILES})

set_target_properties(libgravel PROPERTIES
                      PREFIX ""
                      )

gravel_configure_compile_options(libgravel)

target_include_directories(libgravel
                           PUBLIC
                           ${PROJECT_SOURCE_DIR}

                           SYSTEM PUBLIC
                           ${GRAVEL_LIB_DIR}
                           ${Stb_INCLUDE_DIR}
                           ${VULKAN_INCLUDE_DIR}
                           )

target_link_libraries(libgravel
                      PUBLIC
                      argparse::argparse
                      assimp::assimp
                      Boost::boost
                      EnTT::EnTT
                      fmt::fmt
                      glad::glad
                      glm::glm
                      imgui::imgui
                      imguizmo::imguizmo
                      magic_enum::magic_enum
                      OpenGL::GL
                      spdlog::spdlog
                      SDL2::SDL2
                      Vulkan::Vulkan
                      unofficial::vulkan-memory-allocator::vulkan-memory-allocator
                      )

target_precompile_headers(libgravel PRIVATE ${GRAVEL_PRECOMPILED_HEADERS})
