cmake_minimum_required (VERSION 2.8 FATAL_ERROR)

project(matpower)

# set (BLAS_HEADERS_PATH /usr/include/x86_64-linux-gnu)
# set (BLAS_LIB_PATH /usr/lib/x86_64-linux-gnu/blas)

set (BLAS_HEADERS_PATH /usr/include/x86_64-linux-gnu/openblas-openmp)
set (BLAS_LIB_PATH /usr/lib/x86_64-linux-gnu/openblas-openmp)


find_package (OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS} -O3")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS} -O3")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
else ()
    message (SEND_ERROR "Open MP not found")
endif ()


set (CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")


include_directories (${BLAS_HEADERS_PATH})

link_directories(${BLAS_LIB_PATH})

add_executable (matpower src/matpower.c src/matmul.c src/main.c)

target_link_libraries (matpower blas)
