# Set CMake minimum version and CMake policy required by UrhoCommon module
cmake_minimum_required (VERSION 3.2.3)
if (COMMAND cmake_policy)
    # Libraries linked via full path no longer produce linker search paths
    cmake_policy (SET CMP0003 NEW)
    # INTERFACE_LINK_LIBRARIES defines the link interface
    cmake_policy (SET CMP0022 NEW)
    # Disallow use of the LOCATION target property - so we set to OLD as we still need it
    cmake_policy (SET CMP0026 OLD)
    # MACOSX_RPATH is enabled by default
    cmake_policy (SET CMP0042 NEW)
    # Honor the visibility properties for SHARED target types only
    cmake_policy (SET CMP0063 OLD)
endif ()

# Set project name
project (GameName)

# Set CMake modules search path
set (CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMake/Modules)

# Include UrhoCommon.cmake module after setting project name
include (UrhoCommon)

# Define target name


set (GAME_ONLY_SRC
    src/game/Main.cpp src/game/Main.h
)

set (GAME_COMMON
    src/Globals.h
)


file (GLOB GAME_COMPONENTS
    "src/game/gameComponents/*.h"
    "src/game/gameComponents/*.cpp"
)

# mandatory if you want to use special features from the blender-exporter
file(GLOB SCENE_LOADER_COMPONENT_SAMPLES
    "src/tools/SceneLoader/SampleComponents/*.h"
    "src/tools/SceneLoader/SampleComponents/*.cpp"
)

file(GLOB COMMON_COMPONENTS_SOURCE
    "src/commonComponents/*.h"
    "src/commonComponents/*.cpp"
)


set (SCENE_LOADER_SRC
    # blender tools files
    src/tools/SceneLoader/LoaderTools/ComponentExporter.h
    src/tools/SceneLoader/LoaderTools/ComponentExporter.cpp
    src/tools/SceneLoader/LoaderTools/base64.h
    src/tools/SceneLoader/LoaderTools/base64.cpp

    # sample files
    src/Sample.h src/Sample.inl
    src/tools/SceneLoader/SceneLoader.h
    src/tools/SceneLoader/SceneLoader.cpp
)

set (COMMON_SOURCE_FILES
    ${SCENE_LOADER_COMPONENT_SAMPLES}
    ${COMMON_COMPONENTS_SOURCE}
    ${GAME_COMPONENTS}
    ${GAME_COMMON}
)

set (INCLUDE_DIRS src )


set (TARGET_NAME UrhoSceneLoader)

set (SOURCE_FILES
    ${COMMON_SOURCE_FILES}
    ${SCENE_LOADER_SRC}
)

# Setup target with resource copying
# compile sceneloader
setup_main_executable ()

# compile game
set (TARGET_NAME ${PROJECT_NAME})
set (SOURCE_FILES
    ${COMMON_SOURCE_FILES}
    ${GAME_ONLY_SRC}
)
setup_main_executable ()




