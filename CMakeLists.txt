cmake_minimum_required(VERSION 3.21)
set(SOURCE_DIR "./src/")
# find_library(LIBRARY_SDL sdl)

# set(CMAKE_CXX_EXTENSIONS OFF)

project(chip8)

# c++11 standards
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find SDL  
find_package(SDL2 REQUIRED)

# Set include dirs
set(INCLUDE_DIRS ${SDL2_INCLUDE_DIRS} include)

# Include local 'include' directory
include_directories(${INCLUDE_DIRS})

# SET LIBS
set(LIBRARIES ${SDL2_LIBRARIES})
# Search for all .cpp files in source directory
file(GLOB SOURCES ${SOURCE_DIR}/*.cpp)
add_executable(chip8 ${SOURCES})


# set(CMAKE_CXX_COMPILER g++-11)
set(CMAKE_CXX_COMPILER clang++)


# This is set incase on MAC and using homebrew to install libraries such as SDL
# This needs to be removed, and an option set to check homebrew directories for libraries instead
if(APPLE)
add_definitions("-I/usr/local/include") 
endif()
# set(CMAKE_CXX_FLAGS "-I include -I/usr/local/include")
set (CMAKE_SHARED_LINKER_FLAGS "-L/usr/local/opt/llvm/lib -Wl,-rpath,/usr/local/opt/llvm/lib")

target_compile_options(chip8 PRIVATE -Wall -Wpedantic -Wextra)
target_link_libraries(chip8 ${LIBRARIES})

# Echo out passed flags for verification
message("CMAKE_BUILD_TYPE is ${CMAKE_BUILD_TYPE}")
message("CXX Flags are ${CMAKE_CXX_FLAGS}")