cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(omp_demo)

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
else()
    message(SEND_ERROR "Open MP not found")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

file(GLOB APP_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.c")
foreach(SRC_FILE ${APP_SOURCES})
    get_filename_component(SRC_FILE_NAME ${SRC_FILE} NAME_WE)
    add_executable(${SRC_FILE_NAME} ${SRC_FILE})
endforeach(SRC_FILE ${APP_SOURCES})
