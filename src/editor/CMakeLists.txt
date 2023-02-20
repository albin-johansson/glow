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
target_link_libraries(gravel PRIVATE libgravel)

target_precompile_headers(gravel REUSE_FROM libgravel)
