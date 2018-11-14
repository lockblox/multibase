cmake_minimum_required(VERSION 3.1)

include("3rdparty/gate/cmake/HunterGate.cmake")
HunterGate(URL "https://github.com/ruslo/hunter/archive/v0.21.16.tar.gz"
           SHA1 "aa1faccd24f33dd2c368f85a4e9ca2e5d9c249cb"
)

project(multibase LANGUAGES CXX)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)

add_library(libmultibase
        multibase/Base.h
        multibase/Base.cpp)
target_include_directories(libmultibase PUBLIC "${multibase_SOURCE_DIR}")
set_target_properties(libmultibase PROPERTIES OUTPUT_NAME multibase)

add_executable(multibase multibase/main.cpp)
target_link_libraries(multibase libmultibase)

enable_testing()
add_subdirectory(test)
