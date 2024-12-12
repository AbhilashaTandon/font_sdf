cmake_minimum_required(VERSION 3.5)

set(PROJECT_NAME, "font_sdf")
project(font_sdf LANGUAGES CXX)

add_subdirectory(sfml)
include_directories(include)

file(GLOB all_SRCS
    "${PROJECT_SOURCE_DIR}/include/*.h"
    "${PROJECT_SOURCE_DIR}/include/*.hpp"
    "${PROJECT_SOURCE_DIR}/src/*.c"
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
)
add_compile_options(-g -Wall -Wextra -Wpedantic)

add_executable(${PROJECT_NAME} ${all_SRCS})
target_link_libraries(${PROJECT_NAME} sfml-graphics)


