cmake_minimum_required(VERSION 3.16...3.16.2)
project(SREngine VERSION 0.0.7 LANGUAGES CXX DESCRIPTION "SpaRcle Engine - cross-platform 2D/3D game engine in C++.")

set_property(GLOBAL PROPERTY SR_ENGINE_VERSION_PROPERTY ${PROJECT_VERSION})
set_property(GLOBAL PROPERTY SR_ENGINE_MAJOR_VERSION_PROPERTY ${PROJECT_VERSION_MAJOR})
set_property(GLOBAL PROPERTY SR_ENGINE_MINOR_VERSION_PROPERTY ${PROJECT_VERSION_MINOR})
set_property(GLOBAL PROPERTY SR_ENGINE_PATCH_VERSION_PROPERTY ${PROJECT_VERSION_PATCH})

if (CMAKE_SIZEOF_VOID_P EQUAL 4)
    message(FATAL_ERROR "SpaRcle Engine supports only x64 systems!")
endif()

message(STATUS "SpaRcle Engine: cmake version is ${CMAKE_VERSION}")

set(SR_CMAKE_ROOT_BUILD_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
message(STATUS "SpaRcle Engine: root build directory is ${SR_CMAKE_ROOT_BUILD_DIRECTORY}")

set(CMAKE_CXX_STANDARD 20)

if (UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcompare-debug-second -w -Wno-deprecated -fPIC")
endif()

if (UNIX AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-pie -no-pie -g3")
endif()

if (WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNOMINMAX")
endif()

if (SR_DISABLE_EXCEPTIONS)
    # TODO: not working
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions")
        message(STATUS "Using GNU compiler: -fno-exceptions enabled")
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHs")
        message(STATUS "Using MSVC compiler: /EHs enabled")
    else()
        message(WARNING "Unknown compiler, exceptions handling settings may not be correct")
    endif()
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DSR_DLL_EXPORTS -DSR_UTILS_ASSIMP")

set(SR_PHYSICS_USE_BULLET3 OFF)
set(SR_PHYSICS_USE_PHYSX ON)
set(SR_PHYSICS_USE_BOX2D OFF)

set(SR_UTILS_STATIC_LIBRARY ON)
set(SR_AUDIO_STATIC_LIBRARY ON)
set(SR_PHYSICS_STATIC_LIBRARY ON)
set(SR_GRAPHICS_STATIC_LIBRARY ON)
set(SR_SCRIPTING_STATIC_LIBRARY ON)

set(EVO_VULKAN_STATIC_LIBRARY ON)
set(EVO_SCRIPT_STATIC_LIBRARY ON)

set(SR_TRACY_ENABLE ON)
set(SR_ICU ON)

set(CMAKE_BUILD_PARALLEL_LEVEL 0)

set(CMAKE_SHARED_LINKER_FLAGS_CHECKED "")
set(CMAKE_SHARED_LINKER_FLAGS_PROFILE "")
set(CMAKE_EXE_LINKER_FLAGS_PROFILE "")
set(CMAKE_EXE_LINKER_FLAGS_CHECKED "")

add_subdirectory(Engine)
