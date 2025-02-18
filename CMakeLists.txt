cmake_minimum_required(VERSION 2.8.7)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules")
include(AppendCompilerFlags)

project(repair-sampling)
set(PROJECT_VENDOR "Adrián Gómez Brandón")
set(CMAKE_PROJECT_NAME "repair-sampling")
set(PROJECT_CONTACT "adrian.gbrandon@udc.es")


#Change to "Debug" when you need it
set(CMAKE_BUILD_TYPE "Debug")

# C++11 compiler Check
if(NOT CMAKE_CXX_COMPILER_VERSION) # work around for cmake versions smaller than 2.8.10
    execute_process(COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE CMAKE_CXX_COMPILER_VERSION)
endif()
if(CMAKE_CXX_COMPILER MATCHES ".*clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_COMPILER_IS_CLANGXX 1)
endif()
if( (CMAKE_COMPILER_IS_GNUCXX AND ${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 4.7) OR
(CMAKE_COMPILER_IS_CLANGXX AND ${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 3.2))
    message(FATAL_ERROR "Your C++ compiler does not support C++11. Please install g++ 4.7 (or greater) or clang 3.2 (or greater)")
else()
    message(STATUS "Compiler is recent enough to support C++11.")
endif()
if( CMAKE_COMPILER_IS_GNUCXX )
    append_cxx_compiler_flags("-std=c++11 -Wall -Wextra " "GCC" CMAKE_CXX_FLAGS)
    append_cxx_compiler_flags("-msse4.2 -O3 -ffast-math -funroll-loops -fno-omit-frame-pointer -g" "GCC" CMAKE_CXX_FLAGS_RELEASE)
else()
    append_cxx_compiler_flags("-std=c++11" "CLANG" CMAKE_CXX_FLAGS)
    append_cxx_compiler_flags("-stdlib=libc++" "CLANG" CMAKE_CXX_FLAGS)
    append_cxx_compiler_flags("-msse4.2 -O3  -ffast-math -funroll-loops -DNDEBUG" "CLANG" CMAKE_CXX_FLAGS_RELEASE)
endif()
include(CheckSSE)
FindSSE ()
if( SSE4_2_FOUND )
    if( CMAKE_COMPILER_IS_GNUCXX )
        append_cxx_compiler_flags("-msse4.2" "GCC" CMAKE_CXX_FLAGS)
    else()
        append_cxx_compiler_flags("-msse4.2" "CLANG" CMAKE_CXX_FLAGS)
    endif()
    message(STATUS "CPU does support SSE4.2.")
else()
    message(STATUS "CPU does NOT support SSE4.2")
endif()

add_subdirectory(external/sdsl-lite)
add_subdirectory(external/repair_gn)

include_directories("${PROJECT_SOURCE_DIR}/external/googletest/include"
        ${CMAKE_HOME_DIRECTORY}/include
        ${CMAKE_BINARY_DIR}/external/sdsl-lite/include
        ${CMAKE_BINARY_DIR}/external/sdsl-lite/external/libdivsufsort/include/
        ${CMAKE_HOME_DIRECTORY}/external/googletest/include
        ${CMAKE_HOME_DIRECTORY}/external/sdsl-lite/external/FastPFor/headers/
        ${CMAKE_HOME_DIRECTORY}/external/repair_gn
        ${STXXL_INCLUDE_DIRS}
        )

add_executable(decompress src/decompress.cpp)
target_link_libraries(decompress libRepairGN sdsl divsufsort divsufsort64)

add_executable(compress src/compress.cpp)
target_link_libraries(compress libRepairGN sdsl divsufsort divsufsort64)

add_executable(build src/build.cpp)
target_link_libraries(build libRepairGN sdsl divsufsort divsufsort64)

add_executable(build_dac src/build_dac.cpp)
target_link_libraries(build_dac libRepairGN sdsl divsufsort divsufsort64)

add_executable(build_directory src/build_directory.cpp)
target_link_libraries(build_directory libRepairGN sdsl divsufsort divsufsort64)

add_executable(build_dac_directory src/build_dac_directory.cpp)
target_link_libraries(build_dac_directory libRepairGN sdsl divsufsort divsufsort64)

add_executable(access src/access.cpp)
target_link_libraries(access libRepairGN sdsl divsufsort divsufsort64)

add_executable(access_dac src/access_dac.cpp)
target_link_libraries(access_dac libRepairGN sdsl divsufsort divsufsort64)

add_executable(extremes src/extremes.cpp)
target_link_libraries(extremes libRepairGN sdsl divsufsort divsufsort64)

add_executable(extremes_dac src/extremes_dac.cpp)
target_link_libraries(extremes_dac libRepairGN sdsl divsufsort divsufsort64)

add_executable(times src/times.cpp)
target_link_libraries(times libRepairGN sdsl divsufsort divsufsort64)

add_executable(similarity src/similarity.cpp)
target_link_libraries(similarity libRepairGN sdsl divsufsort divsufsort64)

add_executable(similarity_extract src/similarity_extract.cpp)
target_link_libraries(similarity_extract libRepairGN sdsl divsufsort divsufsort64)

add_executable(similarity_opt src/similarity_opt.cpp)
target_link_libraries(similarity_opt libRepairGN sdsl divsufsort divsufsort64)

add_executable(similarity_dac src/similarity_dac.cpp)
target_link_libraries(similarity_dac libRepairGN sdsl divsufsort divsufsort64)

add_executable(similarity_levels src/similarity_levels.cpp)
target_link_libraries(similarity_levels libRepairGN sdsl divsufsort divsufsort64)

add_executable(repair_sampling_offsets_test test/test_offsets.cpp)
target_link_libraries(repair_sampling_offsets_test libRepairGN sdsl divsufsort divsufsort64 gtest)

add_executable(vector_generator test/vector_generator.cpp)
target_link_libraries(vector_generator libRepairGN sdsl divsufsort divsufsort64)

add_executable(statitistics src/statistics.cpp)
target_link_libraries(statitistics libRepairGN sdsl divsufsort divsufsort64)
