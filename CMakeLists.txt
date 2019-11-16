cmake_minimum_required(VERSION 3.10)

project(multibase VERSION 1.0 LANGUAGES CXX)

option(BUILD_TESTING "Build unit tests" ON)

find_path(GSL_INCLUDE_DIR gsl/gsl)
if (GSL_INCLUDE_DIR STREQUAL "GSL_INCLUDE_DIR-NOTFOUND")
    message(FATAL_ERROR "Could not find Guidelines Support Library.")
endif ()

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(libmultibase
        multibase/multibase.cpp
        multibase/multibase.h
        multibase/basic_codec.cpp
        multibase/basic_codec.h
        multibase/codec.cpp
        multibase/codec.h
        multibase/encoding.cpp
        multibase/encoding.h
        multibase/detail/codec_impl.cpp
        multibase/detail/codec_impl.h)
target_include_directories(libmultibase PUBLIC
        $<BUILD_INTERFACE:${multibase_SOURCE_DIR}>
        $<BUILD_INTERFACE:${GSL_INCLUDE_DIR}>
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
install(FILES ${HEADER_FILES} DESTINATION include/multibase)
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
