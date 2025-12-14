# Core static library - shared functionality between client and server
add_library(${CORE_LIB} STATIC
    shared.cpp
    shared.h
    framework.h
    netlib.hpp
    nbnet.h
    udp.h
)

# Link all shared dependencies
target_link_libraries(${CORE_LIB} PUBLIC 
    raylib 
    ${LUA_LIBRARY}
    sol2::sol2
    box2d::box2d
    ${ECS_LIB}
)

# Include directories
target_include_directories(${CORE_LIB} PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${LUA_INCLUDE_DIR}
)

# Platform-specific libraries
if(WIN32)
    # Windows-specific libraries (Winsock for networking)
    target_link_libraries(${CORE_LIB} PUBLIC ws2_32)
elseif(UNIX AND NOT APPLE)
    # Linux-specific libraries
    target_link_libraries(${CORE_LIB} PUBLIC pthread)
endif()

# Set C++20 standard
target_compile_features(${CORE_LIB} PUBLIC cxx_std_20)
