# ECS static library
set(ECS_LIB r-type_ecs)

add_library(${ECS_LIB} STATIC
    ecs.cpp
    Prefab.cpp
    Prefab.hpp
    Core.hpp
    Signature.hpp
    Builder/Builder.hpp
    Component/Component.hpp
    Component/ComponentArray.hpp
    Component/ComponentManager.hpp
    Entity/Entity.hpp
    Entity/EntityManager.hpp
    System/System.hpp
    System/SystemManager.hpp
    System/GravitySystem.hpp
    System/InputControllerSystem.hpp
    System/MissileSystem.hpp
    System/RendererSystem.hpp
    System/ClientRendererSystem.hpp
)

# Include directories
target_include_directories(${ECS_LIB} PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Builder
    ${CMAKE_CURRENT_SOURCE_DIR}/Component
    ${CMAKE_CURRENT_SOURCE_DIR}/Entity
    ${CMAKE_CURRENT_SOURCE_DIR}/System
)

# Link dependencies (raylib for rendering systems)
target_link_libraries(${ECS_LIB} PUBLIC 
    raylib
)

# Set C++20 standard
target_compile_features(${ECS_LIB} PUBLIC cxx_std_20)
