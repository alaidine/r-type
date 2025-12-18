cmake_minimum_required(VERSION 3.24...4.1)
project(r-type)

# Download CPM.cmake
file(
    DOWNLOAD
    https://github.com/cpm-cmake/CPM.cmake/releases/download/v0.40.2/CPM.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake
    EXPECTED_HASH SHA256=c8cdc32c03816538ce22781ed72964dc864b2a34a310d3b7104812a5ca2d835d
)
include(${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake)

set(CMAKE_CXX_STANDARD 20)

# Project targets
set(CORE_LIB r-type_core)
set(ECS_LIB r-type_ecs)
set(CLIENT r-type_client)
set(SERVER r-type_server)

# Dependencies using CPM
CPMAddPackage(
    NAME raylib
    URL https://github.com/raysan5/raylib/archive/refs/tags/5.0.zip
    VERSION 5.0
    DOWNLOAD_ONLY YES
)
if(raylib_ADDED)
    # Configure raylib build options
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(BUILD_GAMES OFF CACHE BOOL "" FORCE)
    add_subdirectory(${raylib_SOURCE_DIR} ${raylib_BINARY_DIR})
endif()

CPMAddPackage(
    NAME sol2
    URL https://github.com/ThePhD/sol2/archive/refs/tags/v3.3.0.zip
    VERSION 3.3.0
)

CPMAddPackage(
    NAME box2d
    URL https://github.com/erincatto/box2d/archive/refs/tags/v3.1.1.zip
    VERSION 3.1.1
)

# Lua setup
if(APPLE)
    # Use Homebrew Lua on macOS
    execute_process(
        COMMAND /opt/homebrew/bin/brew --prefix lua
        OUTPUT_VARIABLE LUA_PREFIX
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    if(LUA_PREFIX)
        set(LUA_INCLUDE_DIR ${LUA_PREFIX}/include/lua)
        find_library(LUA_LIBRARY lua PATHS ${LUA_PREFIX}/lib NO_DEFAULT_PATH)
        message(STATUS "Using Homebrew Lua: ${LUA_LIBRARY}")
    else()
        message(WARNING "Lua not found via Homebrew, using local libraries")
        set(LUA_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/lua/include)
        set(LUA_LIBRARY ${CMAKE_SOURCE_DIR}/lua/liblua54.a)
    endif()
elseif(WIN32)
    set(LUA_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/lua/include)
    set(LUA_LIBRARY ${CMAKE_SOURCE_DIR}/lua/lua54.lib)
else()
    set(LUA_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/lua/include)
    set(LUA_LIBRARY ${CMAKE_SOURCE_DIR}/lua/liblua54.a)
endif()

# Add subdirectories
add_subdirectory(ECS)
add_subdirectory(core)
add_subdirectory(client)
add_subdirectory(server)
