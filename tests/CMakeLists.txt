
include_directories(${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/src)

set(TEST tests.cc)
add_executable(utest ${TEST})
add_dependencies(utest bcimpl)
target_link_libraries(utest bcimpl Catch2WithMain)