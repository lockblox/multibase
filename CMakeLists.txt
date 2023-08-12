cmake_minimum_required(VERSION 3.15)

set(CMAKE_TOOLCHAIN_FILE
        "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "Vcpkg toolchain file")

project(multibase VERSION 1.0 LANGUAGES CXX)

option(BUILD_TESTING "Build unit tests" ON)
option(BUILD_COVERAGE "Build with coverage" OFF)
option(BUILD_BENCHMARK "Build benchmark" OFF)

find_package(Boost REQUIRED)
find_package(range-v3 REQUIRED)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(libmultibase
        multibase/basic_algorithm.cpp
        multibase/basic_algorithm.h
        multibase/encoding.cpp
        multibase/encoding.h
        multibase/codec.cpp
        multibase/codec.h
        multibase/identity_algorithm.cpp
        multibase/identity_algorithm.h
        multibase/algorithm.cpp
        multibase/algorithm.h)

target_include_directories(libmultibase PUBLIC
        $<BUILD_INTERFACE:${multibase_SOURCE_DIR}>
        $<BUILD_INTERFACE:${Boost_INCLUDE_DIRS}>
        $<INSTALL_INTERFACE:include>)
set_target_properties(libmultibase PROPERTIES OUTPUT_NAME multibase)

add_executable(multibase multibase/main.cpp)
target_link_libraries(multibase libmultibase)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
        "${multibase_BINARY_DIR}/multibaseConfigVersion.cmake"
        VERSION ${PACKAGE_VERSION}
        COMPATIBILITY AnyNewerVersion)
install(TARGETS libmultibase
        EXPORT multibaseTargets
        INCLUDES DESTINATION include
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
install(DIRECTORY multibase DESTINATION include
        FILES_MATCHING PATTERN "*.h")
configure_package_config_file(
        "${multibase_SOURCE_DIR}/cmake/multibaseConfig.cmake"
        "${multibase_BINARY_DIR}/multibaseConfig.cmake"
        INSTALL_DESTINATION share/cmake/multibase
)
install(EXPORT multibaseTargets DESTINATION share/cmake/multibase)
install(FILES "${multibase_BINARY_DIR}/multibaseConfigVersion.cmake"
        "${multibase_BINARY_DIR}/multibaseConfig.cmake"
        DESTINATION share/cmake/multibase)

if (MSVC)
    target_compile_options(libmultibase PRIVATE /W4 /WX /MP)
else ()
    if (BUILD_COVERAGE)
        if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
            set(COVERAGE_FLAGS "--coverage")
        endif ()
    endif ()
    target_compile_options(libmultibase PRIVATE -Wall -Wextra -pedantic -Werror
            ${COVERAGE_FLAGS})
endif ()

if (BUILD_BENCHMARK)
    find_package(benchmark CONFIG REQUIRED)
    add_executable(multibase_benchmark benchmark/main.cpp)
    target_link_libraries(multibase_benchmark PRIVATE
            libmultibase benchmark::benchmark
            benchmark::benchmark_main ${COVERAGE_FLAGS})
endif ()

if (BUILD_TESTING)
    enable_testing()
    find_package(GTest MODULE REQUIRED)
    add_executable(multibase_test test/multibase_test.cpp)
    target_link_libraries(multibase_test PRIVATE libmultibase GTest::gmock_main)
    set_target_properties(multibase_test PROPERTIES RUNTIME_OUTPUT_DIRECTORY
            ${CMAKE_CURRENT_BINARY_DIR})
    gtest_discover_tests(multibase_test)
    if (MSVC)
        target_compile_options(multibase_test PRIVATE /W4 /WX /MP)
    else ()
        target_compile_options(multibase_test PRIVATE -Wall -Wextra -pedantic
                -Werror ${COVERAGE_FLAGS})
        target_link_libraries(multibase_test PRIVATE ${COVERAGE_FLAGS})
    endif ()
endif ()
