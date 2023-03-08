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
                           ${Stb_INCLUDE_DIR}
                           )

target_link_libraries(libgravel
                      PUBLIC
                      assimp::assimp
                      EnTT::EnTT
                      fmt::fmt
                      glad::glad
                      glm::glm
                      imgui::imgui
                      magic_enum::magic_enum
                      OpenGL::GL
                      spdlog::spdlog
                      $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
                      )

target_precompile_headers(libgravel PRIVATE ${GRAVEL_PRECOMPILED_HEADERS})
