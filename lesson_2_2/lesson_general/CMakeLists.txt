cmake_minimum_required(VERSION 3.16)
project(main LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Put the executable directly in the build directory.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/*.c"
)

add_executable(main ${SOURCES})

target_include_directories(main PRIVATE
    "${CMAKE_SOURCE_DIR}/include"
    "${CMAKE_SOURCE_DIR}/src"
)

target_link_directories(main PRIVATE "${CMAKE_SOURCE_DIR}/lib")
target_link_libraries(main PRIVATE glfw GL)

target_compile_options(main PRIVATE -Wall -Wextra -g -O0)
