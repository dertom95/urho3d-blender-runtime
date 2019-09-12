# Set project name
project (ProjectTemplate)

# Set minimum version
cmake_minimum_required (VERSION 2.8.6)
if (COMMAND cmake_policy)
    cmake_policy (SET CMP0003 NEW)
    if (CMAKE_VERSION VERSION_GREATER 2.8.12 OR CMAKE_VERSION VERSION_EQUAL 2.8.12)
        # INTERFACE_LINK_LIBRARIES defines the link interface
        cmake_policy (SET CMP0022 NEW)
    endif ()
    if (CMAKE_VERSION VERSION_GREATER 3.0.0 OR CMAKE_VERSION VERSION_EQUAL 3.0.0)
        # Disallow use of the LOCATION target property - therefore we set to OLD as we still need it
        cmake_policy (SET CMP0026 OLD)
        # MACOSX_RPATH is enabled by default
        cmake_policy (SET CMP0042 NEW)
    endif ()
endif ()

# Set CMake modules search path
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/CMake/Modules)

# Include Urho3D Cmake common module
# include (UrhoCommon)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)

# Find Urho3D library
# find_package (Urho3D REQUIRED)
# include_directories (${URHO3D_INCLUDE_DIRS})

# Define target name
set (TARGET_NAME ProjectTemplate)

#define_source_files (GLOB_CPP_PATTERNS *.c* GLOB_H_PATTERNS *.h* RECURSE GROUP)

hunter_add_package(ZeroMQ)

find_package(ZeroMQ CONFIG REQUIRED)


INCLUDE_DIRECTORIES(
    3rd/libzmq/include
    3rd/cppzmq
    )

file (GLOB_RECURSE GAME_FILES
    "game/gameComponents/*.h"
    "game/gameComponents/*.cpp"
    "game/gameObjects/*.h"
    "game/gameObjects/*.cpp"
)


set (GAME_COMMON
    game/SaalisEvents.h
    game/SaalisGlobals.h
)

set (GAME_CLIENT
    game/Saalis.h
    game/Saalis.cpp
)

set (GAME_SERVER_STARTER
    game/SaalisServerStarter.h
    game/SaalisServerStarter.cpp)


file (GLOB GAME_COMPONENTS
    "game/gameComponents/*.h"
    "game/gameComponents/*.cpp"
)

file(GLOB GAME_SERVER
    "game/serverObjects/*.h"
    "game/serverObjects/*.cpp"
)

# mandatory if you want to use special features from the blender-exporter
file(GLOB SCENE_LOADER_COMPONENT_SAMPLES
    "tools/SceneLoader/SampleComponents/*.h"
    "tools/SceneLoader/SampleComponents/*.cpp"
)

file(GLOB COMMON_COMPONENTS_SOURCE
    "commonComponents/*.h"
    "commonComponents/*.cpp"
)

file(GLOB COMMON_OBJECTS_SOURCE
    "commonObjects/*.h"
    "commonObjects/*.cpp"
)
set (COMMON_SOURCE_FILES
    ${SCENE_LOADER_COMPONENT_SAMPLES}
    ${COMMON_COMPONENTS_SOURCE}
    ${GAME_COMPONENTS}
    src/Globals.h
)

set (INCLUDE_DIRS . )


set (SOURCE_FILES
    ${COMMON_SOURCE_FILES}
    ${SCENE_LOADER_SRC}
)

#set(INCLUDE_DIRS include rapidjson/include)
#set(INCLUDE_DIRS include)

# Setup target with resource copying
set (SOURCE_FILES
    ${COMMON_COMPONENTS_SOURCE}
    ${GAME_FILES}
    ${GAME_COMMON}
    ${GAME_CLIENT}
)
setup_main_executable ()
target_link_libraries(${TARGET_NAME} ZeroMQ::libzmq-static)
message(info "LINK TARGET ${TARGET_NAME}")

ADD_DEFINITIONS(
    -std=c++11 # Or -std=c++0x
    # Other flags
)
