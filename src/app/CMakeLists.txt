project(glow-app LANGUAGES CXX)

add_executable(glow main.cpp)
add_dependencies(glow libglow)

glow_configure_compile_options(glow)

target_include_directories(glow PRIVATE ${PROJECT_SOURCE_DIR})
target_link_libraries(glow
                      PRIVATE
                      libglow
                      $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
                      )

target_precompile_headers(glow REUSE_FROM libglow)

if (WIN32)
  glow_copy_directory_post_build(glow ${GLOW_ASSET_DIR} "${CMAKE_CURRENT_BINARY_DIR}/assets")
else ()
  glow_symlink_directory_post_build(glow ${GLOW_ASSET_DIR} "${CMAKE_CURRENT_BINARY_DIR}/assets")
endif ()
