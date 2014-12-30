cmake_minimum_required( VERSION 2.6 )
project( CL_Book )

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

find_package( OpenCL REQUIRED )

include_directories( ${OPENCL_INCLUDE_DIRS} )
include_directories( "${CMAKE_SOURCE_DIR}/khronos" )

add_executable( demo demo.cpp )
target_link_libraries( demo ${OPENCL_LIBRARIES} )

configure_file(demo.cl ${CMAKE_CURRENT_BINARY_DIR}/demo.cl COPYONLY)
