project(glow-opengl-rhi CXX)

file(GLOB_RECURSE GLOW_OPENGL_RHI_SOURCE_FILES
     CONFIGURE_DEPENDS
     ${GLOW_INCLUDE_DIR}/opengl-rhi/*.hpp
     ${PROJECT_SOURCE_DIR}/*.cpp
     ${PROJECT_SOURCE_DIR}/*.hpp
     )

add_library(${GLOW_OPENGL_RHI_TARGET} MODULE ${GLOW_OPENGL_RHI_SOURCE_FILES})

glow_prepare_target(${GLOW_OPENGL_RHI_TARGET})

target_compile_definitions(${GLOW_OPENGL_RHI_TARGET}
                           PRIVATE
                           GLOW_BUILDING_OPENGL_RHI
                           )

target_link_libraries(${GLOW_OPENGL_RHI_TARGET}
                      PRIVATE
                      ${GLOW_CORE_TARGET}
                      OpenGL::GL
                      glad::glad
                      )
