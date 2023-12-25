cmake_minimum_required(VERSION 3.10)

# set the project name
project(BcupSys)

# add the executable

file(GLOB_RECURSE DIR_SRC "src/*.cpp")
include_directories(src)

add_compile_options(-g3 -std=c++20 -Wall)

add_executable(BcupSys ${DIR_SRC})

include(formatting.cmake)
clang_format(format ${DIR_SRC} ${HDR_SRC})
clang_format_check(format_check ${DIR_SRC} ${HDR_SRC})
