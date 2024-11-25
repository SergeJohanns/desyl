cmake_minimum_required(VERSION 3.26)  # CMake version check
project(
    desyl
    VERSION 1.0
    DESCRIPTION "A C++ library for deductive synthesis of programs using Separation Logic"
    LANGUAGES CXX
)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set(CMAKE_CXX_STANDARD 20)            # Enable c++20 standard
    set(CMAKE_CXX_STANDARD_REQUIRED ON)   # Enable c++20 standard
    set(CMAKE_CXX_EXTENSIONS OFF)         # Disable compiler extensions

    # Let's nicely support folders in IDEs
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)

    set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # Generate compile_commands.json

    # Set flags
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -Wno-cast-function-type -Wno-error=cast-function-type -Wno-error=unused-parameter")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0 -fsanitize=address,undefined")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -O3 -g -fsanitize=address,undefined")
endif()

include(FetchContent)

# Include lexing/parsing library
FetchContent_Declare(lexy URL https://lexy.foonathan.net/download/lexy-src.zip)

# Include command line flags library
FetchContent_Declare(gflags
	GIT_REPOSITORY	https://github.com/gflags/gflags.git
	GIT_TAG			master
)
FetchContent_GetProperties(gflags)
if(NOT gflags_POPULATED)
	FetchContent_Populate(gflags)
	cmake_policy(SET CMP0069 NEW)
	add_subdirectory(${gflags_SOURCE_DIR} ${gflags_BINARY_DIR})
endif()

# YDF needs abseil-cpp
add_subdirectory(abseil-cpp)
# add_subdirectory(yggdrasil-decision-forests)
include_directories(yggdrasil-decision-forests)

FetchContent_MakeAvailable(lexy gflags)

add_subdirectory(src)
add_subdirectory(apps)
