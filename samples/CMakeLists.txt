add_executable(samples main.cpp)

target_include_directories(samples PRIVATE ${CMAKE_SOURCE_DIR}/renderer)
target_link_libraries(samples PRIVATE renderer Vulkan::Vulkan glfw glm::glm ktx)
