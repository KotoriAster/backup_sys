cmake_minimum_required(VERSION 3.10)

# set the project name
project(BcupSys)

# add the executable

file(GLOB_RECURSE DIR_SRC "src/*.cpp")
file(GLOB_RECURSE HDR_SRC "src/*.hpp")
include_directories(src)
include_directories(lib)
add_subdirectory(tests)
add_subdirectory(src)
add_subdirectory(lib)

add_compile_options(-g3 -std=c++17 -Wall)

add_executable(BcupSys ${DIR_SRC})

include(formatting.cmake)
clang_format(format ${DIR_SRC} ${HDR_SRC})
clang_format_check(format_check ${DIR_SRC} ${HDR_SRC})
