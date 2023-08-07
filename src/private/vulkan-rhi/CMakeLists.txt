project(glow-vulkan-rhi CXX)

file(GLOB_RECURSE GLOW_VULKAN_RHI_SOURCE_FILES
     CONFIGURE_DEPENDS
     ${GLOW_INCLUDE_DIR}/vulkan-rhi/*.hpp
     ${PROJECT_SOURCE_DIR}/*.cpp
     ${PROJECT_SOURCE_DIR}/*.hpp
     )

add_library(${GLOW_VULKAN_RHI_TARGET} MODULE ${GLOW_VULKAN_RHI_SOURCE_FILES})

glow_prepare_target(${GLOW_VULKAN_RHI_TARGET})

target_compile_definitions(${GLOW_VULKAN_RHI_TARGET}
                           PRIVATE
                           GLOW_BUILDING_VULKAN_RHI
                           )

target_link_libraries(${GLOW_VULKAN_RHI_TARGET}
                      PRIVATE
                      ${GLOW_CORE_TARGET}
                      Vulkan::Vulkan
                      unofficial::vulkan-memory-allocator::vulkan-memory-allocator
                      )
