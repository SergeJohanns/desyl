cmake_minimum_required(VERSION 3.26)  # CMake version check
project(
    desyl
    VERSION 0.1
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
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Werror")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0 -fsanitize=address,undefined")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -O3 -g -fsanitize=address,undefined")
endif()

add_subdirectory(src)
add_subdirectory(apps)
