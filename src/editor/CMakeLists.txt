project(gravel-editor LANGUAGES CXX)

file(GLOB_RECURSE GRAVEL_EDITOR_SOURCE_FILES
     CONFIGURE_DEPENDS
     **/*.cpp
     **/*.hpp
     )

add_executable(gravel ${GRAVEL_EDITOR_SOURCE_FILES})
add_dependencies(gravel libgravel)

gravel_configure_compile_options(gravel)

target_include_directories(gravel PRIVATE ${PROJECT_SOURCE_DIR})
target_link_libraries(gravel
                      PRIVATE
                      libgravel
                      $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
                      )

target_precompile_headers(gravel REUSE_FROM libgravel)

if (WIN32)
  gravel_copy_directory_post_build(gravel ${GRAVEL_ASSET_DIR} "${CMAKE_CURRENT_BINARY_DIR}/assets")
else ()
  gravel_symlink_directory_post_build(gravel ${GRAVEL_ASSET_DIR} "${CMAKE_CURRENT_BINARY_DIR}/assets")
endif ()
