project(asgard)
cmake_minimum_required(VERSION 3.0)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules/")
find_package(Protoc)
SET(VALHALLA_FIND_REQUIRED true)
find_package(Valhalla)

#get git revision as the version number
include(GetGitRevisionDescription)
git_describe(GIT_REVISION)

IF(NOT CMAKE_BUILD_TYPE)
    #https://cmake.org/pipermail/cmake/2009-June/030311.html
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel." FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)

IF(CMAKE_COMPILER_IS_GNUCXX)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra -std=c++14 -Wno-unused-parameter -rdynamic -fuse-ld=gold")
    SET(CMAKE_CXX_FLAGS_PROFILE "${CMAKE_CXX_FLAG} --coverage -fprofile-arcs -ftest-coverage -g")
    SET(CMAKE_EXE_LINKER_FLAGS_PROFILE "${CMAKE_EXE_LINKER_FLAGS} --coverage")
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer")

# Load in the needed CMake modules
include(UseBoost)
include(ThirdParty)


# Handle sanitizers
option(ENABLE_SANITIZERS "Use all the integrated sanitizers for Debug build" OFF)
include(SanitizerOptions)
IF(ENABLE_SANITIZERS)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libasan")
ENDIF(ENABLE_SANITIZERS)

# Handle ENABLE_THREAD_SAFE_TILE_REF_COUNT in Valhalla v3.1.0
# https://github.com/valhalla/valhalla/blob/3.1.0/valhalla/baldr/graphtile.h#L45
# Since this flag is in the header file, we must define the flag when compiling asgard too
option(ENABLE_VAHLALLA_THREAD_SAFE_TILE_REF_COUNT "Activate thread safe tile ref counter when compiling asgard" ON)
IF(ENABLE_VAHLALLA_THREAD_SAFE_TILE_REF_COUNT)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DENABLE_THREAD_SAFE_TILE_REF_COUNT")
ENDIF(ENABLE_VAHLALLA_THREAD_SAFE_TILE_REF_COUNT)

#add current compilation dir to include path to handle config.h
include_directories("${CMAKE_SOURCE_DIR}")
include_directories("${CMAKE_BINARY_DIR}")
include_directories(SYSTEM "${VALHALLA_INCLUDES}")

link_directories(${Boost_LIBRARY_DIRS})

add_subdirectory(asgard)

include(LoadConfig)
