# CMake settings

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.0)

# About this project
PROJECT(IOTKIT-COMM-C-Library)

MESSAGE("Building the root source C Folder")

SET(CMAKE_BUILD_TYPE "Coverage")

MESSAGE("Building the root source C Folder")

# support for adding tests, adds another build target 'test'
ENABLE_TESTING()

# Adds some testing specific build targets e.g. make Experimental
INCLUDE(CTest)

SET(CMAKE_CXX_FLAGS_COVERAGE "-g -O0 -Wall -W -Wshadow -Wunused-variable -Wunused-parameter -Wunused-function -Wunused -Wno-system-headers -Wno-deprecated -Woverloaded-virtual -Wwrite-strings -fprofile-arcs -ftest-coverage")

SET(CMAKE_C_FLAGS_COVERAGE "-g -O0 -Wall -W -fprofile-arcs -ftest-coverage")

SET(CMAKE_EXE_LINKER_FLAGS_COVERAGE "-fprofile-arcs -ftest-coverage")

# add the sub-directories
ADD_SUBDIRECTORY(lib)
ADD_SUBDIRECTORY(examples)
ADD_SUBDIRECTORY(tests)

SET (CTEST_BINARY_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/output/bin" )
MESSAGE (STATUS "Ctest Binary Directory  set to: ${CTEST_BINARY_DIRECTORY}")
