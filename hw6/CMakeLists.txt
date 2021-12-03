cmake_minimum_required(VERSION 3.18)

project(hw6 CUDA)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")
set (CMAKE_CUDA_COMPILER /usr/bin/nvcc)

find_package (OpenCV REQUIRED)
include_directories (${OpenCV_INCLUDE_DIRS})

set (CMAKE_CUDA_STANDARD 14)

add_executable (conv src/conv_kernel.cu)
add_executable (median src/median_kernel.cu)

target_link_libraries (conv ${OpenCV_LIBS} )
target_link_libraries (median ${OpenCV_LIBS} )