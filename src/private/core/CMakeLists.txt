project(glow-core CXX)

file(GLOB_RECURSE GLOW_CORE_SOURCE_FILES
     CONFIGURE_DEPENDS
     ${GLOW_INCLUDE_DIR}/core/*.hpp
     ${GLOW_INCLUDE_DIR}/core/**/*.hpp
     ${PROJECT_SOURCE_DIR}/**/*.cpp
     ${PROJECT_SOURCE_DIR}/**/*.hpp
     )

add_library(${GLOW_CORE_TARGET} SHARED ${GLOW_CORE_SOURCE_FILES})

glow_prepare_target(${GLOW_CORE_TARGET} FALSE)

target_compile_definitions(${GLOW_CORE_TARGET}
                           PRIVATE
                           GLOW_BUILDING_CORE
                           BOOST_STACKTRACE_GNU_SOURCE_NOT_REQUIRED
                           )

target_include_directories(${GLOW_CORE_TARGET}
                           PRIVATE
                           "${PROJECT_SOURCE_DIR}/.."
                           )

target_link_libraries(${GLOW_CORE_TARGET}
                      PRIVATE
                      Boost::boost
                      SDL2::SDL2
                      Vulkan::Vulkan  # Needed for SDL to be able to locate Vulkan.
                      )