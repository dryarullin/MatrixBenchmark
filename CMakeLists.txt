cmake_minimum_required(VERSION 3.20 FATAL_ERROR)
project(MatrixBenchmark)

# optional, you can remove it. 
include(cmake/conan.cmake)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON) 

option(WITH_VECTORIZE_SSE "SSE vectorization" OFF)
option(WITH_VECTORIZE_AVX "AVX vectorization" OFF)
option(WITH_VECTORIZE_NATIVE "Native vectorization" OFF)
option(WITH_USE_QVEC_REPORT "MSVC Qvec-report:2" OFF)
option(WITHOUT_VECTORIZATION "Don't vectorize" ON)
option(WITH_MKL "Use MKL engine" OFF)
option(WITH_MATRIX_MULTIPLICATION "Matrix multiplication benchmark" OFF)
option(WITH_MATRIX_ADDITION "Matrix addition benchmark" OFF)

if (WITH_VECTORIZE_SSE)
    add_definitions(-DVECTORIZE_SSE)
elseif (WITH_VECTORIZE_AVX)
    add_definitions(-DVECTORIZE_AVX)
elseif (WITH_MKL)
    add_definitions(-DUSE_MKL)
else()
    add_definitions(-DDONT_VECTORIZE)
endif()

if (WITH_MATRIX_MULTIPLICATION)
    add_definitions(-DMATRIX_MULTIPLICATION)
elseif (WITH_MATRIX_ADDITION)
    add_definitions(-DMATRIX_ADDITION)
endif()

option(DEBUG_INFO "Build with debug info symbols in MSVC release" OFF)
if (MSVC AND DEBUG_INFO)
    string(REPLACE /O2 "/Od /Zi" CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE})
    string(REPLACE /Ob2 "" CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE})
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /DEBUG")
    set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /DEBUG")
endif()

find_package(MKL CONFIG REQUIRED)
find_package(Eigen3 CONFIG REQUIRED)
find_package(benchmark REQUIRED)

add_executable(${PROJECT_NAME} main.cpp simple_matrix.h expression_matrix.h)

if (MSVC)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /FORCE:Multiple")
    if (WITH_USE_QVEC_REPORT)
        target_compile_options(${PROJECT_NAME} PUBLIC "/Qvec-report:2") # /Qvec-report:1
    endif()
else()
    if (WITH_VECTORIZE_SSE)
        target_compile_options(${PROJECT_NAME} PUBLIC "-msse -msse2")
    elseif (WITH_VECTORIZE_AVX)
        target_compile_options(${PROJECT_NAME} PUBLIC "-mavx -mavx2")
    elseif (WITH_VECTORIZE_NATIVE)
        target_compile_options(${PROJECT_NAME} PUBLIC "-march=native")
    endif()
endif()

target_link_libraries(${PROJECT_NAME} PUBLIC benchmark::benchmark MKL::MKL Eigen3::Eigen)
