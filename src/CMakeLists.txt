include_directories(${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/src)

set(LIB_BCUPSYS_IMPL linux_copy.cpp  )
add_library(bcimpl STATIC ${LIB_BCUPSYS_IMPL})