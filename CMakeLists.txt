cmake_minimum_required(VERSION 3.10)

project(multibase VERSION 1.0 LANGUAGES CXX)

option(BUILD_TESTING "Build unit tests" ON)

find_package(chunx CONFIG REQUIRED)
find_package(Boost REQUIRED)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
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
        )
target_include_directories(libmultibase PUBLIC
        $<BUILD_INTERFACE:${multibase_SOURCE_DIR}>
        $<BUILD_INTERFACE:${Boost_INCLUDE_DIRS}>
        $<INSTALL_INTERFACE:include>)
set_target_properties(libmultibase PROPERTIES OUTPUT_NAME multibase)
target_link_libraries(libmultibase PUBLIC libchunx)

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

if (BUILD_TESTING)
    enable_testing()
    add_subdirectory(test)
endif ()
