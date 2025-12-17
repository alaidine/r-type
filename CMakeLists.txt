cmake_minimum_required(VERSION 4.1.0)

project(rt-engine)

set(CMAKE_CXX_STANDARD 20)

find_package(Vulkan REQUIRED)

add_subdirectory(glfw)
add_subdirectory(glm)
add_subdirectory(KTX-Software/lib ktx)

add_subdirectory(renderer)
add_subdirectory(samples)
