cmake_minimum_required(VERSION 3.15)

set(CMAKE_TOOLCHAIN_FILE
    "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake"
    CACHE STRING "Vcpkg toolchain file")

project(
  multibase
  VERSION 1.0
  LANGUAGES CXX)

option(BUILD_TESTING "Build unit tests" ON)

find_package(Microsoft.GSL CONFIG REQUIRED)
find_package(range-v3 CONFIG REQUIRED)
find_package(magic_enum CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(
  libmultibase
  multibase/basic_algorithm.cpp
  multibase/basic_algorithm.hpp
  multibase/encoding.cpp
  multibase/encoding.hpp
  multibase/codec.cpp
  multibase/codec.hpp
  multibase/encoding_case.hpp
  multibase/encoding_case.cpp
  multibase/encoding_metadata.hpp
  multibase/encoding_metadata.cpp
  multibase/encoding_traits.hpp
  multibase/encoding_traits.cpp
  multibase/log.hpp
  multibase/log.cpp)

target_include_directories(
  libmultibase PUBLIC $<BUILD_INTERFACE:${multibase_SOURCE_DIR}>
                      $<INSTALL_INTERFACE:include>)
set_target_properties(libmultibase PROPERTIES OUTPUT_NAME multibase)
target_link_libraries(libmultibase magic_enum::magic_enum range-v3::range-v3
                      Microsoft.GSL::GSL fmt::fmt-header-only)

set(MSVC_COMPILE_OPTIONS /W4 /WX /MP /permissive- /analyze /w14640)
set(CLANG_COMPILE_OPTIONS -Werror -Weverything -Wno-padded -Wno-c++98-compat
                          -Wno-c++98-compat-pedantic -Wno-unknown-pragmas)
set(GNU_COMPILE_OPTIONS
    -Wall
    -Wextra
    -Wshadow
    -Wnon-virtual-dtor
    -pedantic
    -Werror
    -Wold-style-cast
    -Wcast-align
    -Wunused
    -Woverloaded-virtual
    -Wpedantic
    -Wconversion
    -Wsign-conversion
    -Wmisleading-indentation
    -Wduplicated-cond
    -Wduplicated-branches
    -Wlogical-op
    -Wnull-dereference
    -Wuseless-cast
    -Wdouble-promotion
    -Wformat=2
    -Wno-unknown-pragmas)

target_compile_options(
  libmultibase
  PUBLIC $<$<CXX_COMPILER_ID:Clang>:${CLANG_COMPILE_OPTIONS}>
         $<$<CXX_COMPILER_ID:GNU>:${GNU_COMPILE_OPTIONS}>
         $<$<CXX_COMPILER_ID:MSVC>:${MSVC_COMPILE_OPTIONS}>)

add_executable(multibase multibase/main.cpp)
target_link_libraries(multibase libmultibase)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  "${multibase_BINARY_DIR}/multibaseConfigVersion.cmake"
  VERSION ${PACKAGE_VERSION}
  COMPATIBILITY AnyNewerVersion)
install(
  TARGETS libmultibase
  EXPORT multibaseTargets
  INCLUDES
  DESTINATION include
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)
install(
  DIRECTORY multibase
  DESTINATION include
  FILES_MATCHING
  PATTERN "*.h")
configure_package_config_file(
  "${multibase_SOURCE_DIR}/cmake/multibaseConfig.cmake"
  "${multibase_BINARY_DIR}/multibaseConfig.cmake"
  INSTALL_DESTINATION share/cmake/multibase)
install(EXPORT multibaseTargets DESTINATION share/cmake/multibase)
install(FILES "${multibase_BINARY_DIR}/multibaseConfigVersion.cmake"
              "${multibase_BINARY_DIR}/multibaseConfig.cmake"
        DESTINATION share/cmake/multibase)

if(BUILD_TESTING)
  find_package(benchmark CONFIG REQUIRED)
  add_executable(multibase_benchmark benchmark/main.cpp)
  target_link_libraries(
    multibase_benchmark PRIVATE libmultibase benchmark::benchmark
                                benchmark::benchmark_main)
  enable_testing()
  find_package(GTest MODULE REQUIRED)
  add_executable(multibase_test test/multibase_test.cpp)
  target_link_libraries(multibase_test PRIVATE libmultibase GTest::gmock_main)
  set_target_properties(multibase_test PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                                                  ${CMAKE_CURRENT_BINARY_DIR})
  target_compile_options(
    multibase_test
    PUBLIC $<$<CXX_COMPILER_ID:Clang>:${CLANG_COMPILE_OPTIONS}>
           $<$<CXX_COMPILER_ID:GNU>:${GNU_COMPILE_OPTIONS}>
           $<$<CXX_COMPILER_ID:MSVC>:${MSVC_COMPILE_OPTIONS}>)

  gtest_discover_tests(multibase_test)
endif()

include(CTest)
