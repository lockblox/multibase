include(CTest)

if(BUILD_TESTING)
  find_package(benchmark CONFIG REQUIRED)
  add_executable(multibase_benchmark)
  target_link_libraries(
    multibase_benchmark PRIVATE libmultibase benchmark::benchmark
                                benchmark::benchmark_main)
  enable_testing()
  find_package(GTest MODULE REQUIRED)
  add_executable(multibase_test)
  target_link_libraries(multibase_test PRIVATE libmultibase GTest::gtest_main GTest::gtest GTest::gmock)
  set_target_properties(multibase_test PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                                                  ${CMAKE_CURRENT_BINARY_DIR})
  target_compile_options(
    multibase_test
    PUBLIC $<$<CXX_COMPILER_ID:Clang>:${CLANG_COMPILE_OPTIONS}>
           $<$<CXX_COMPILER_ID:GNU>:${GNU_COMPILE_OPTIONS}>
           $<$<CXX_COMPILER_ID:MSVC>:${MSVC_COMPILE_OPTIONS}>)

  gtest_discover_tests(multibase_test)
endif()

add_subdirectory(src)
