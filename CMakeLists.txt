cmake_minimum_required(VERSION 4.1.0)

project(rt-engine)

set(CMAKE_CXX_STANDARD 20)

add_executable(${PROJECT_NAME}
    base.cpp
    base.h
    benchmark.hpp
    camera.hpp
    CommandLineParser.hpp
    Entrypoints.h
    keycodes.hpp
    main.cpp
    VulkanBuffer.cpp
    VulkanBuffer.h
    VulkanDebug.cpp
    VulkanDebug.h
    VulkanDevice.cpp
    VulkanDevice.h
    VulkanInitializers.hpp
    VulkanSwapChain.cpp
    VulkanSwapChain.h
    VulkanTexture.cpp
    VulkanTexture.h
    VulkanTools.h
    VulkanTools.cpp
    VulkanUIOverlay.cpp
    VulkanUIOverlay.h
    imconfig.h
    imgui.cpp
    imgui.h
    imgui_draw.cpp
    imgui_demo.cpp
    imgui_widgets.cpp
    imgui_internal.h
    imstb_rectpack.h
    imstb_textedit.h
    imstb_truetype.h)

find_package(Vulkan REQUIRED)

add_subdirectory(glfw)
add_subdirectory(glm)
add_subdirectory(KTX-Software/lib ktx)

target_link_libraries(${PROJECT_NAME} PRIVATE Vulkan::Vulkan glfw glm::glm ktx)
