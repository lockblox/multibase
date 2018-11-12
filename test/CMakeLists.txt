enable_testing()

# Compile and run all unit tests
hunter_add_package(GTest)
find_package(GTest CONFIG REQUIRED)
add_executable(MultibaseTest MultibaseTest.cpp main.cpp)
target_link_libraries(MultibaseTest PUBLIC libmultibase PUBLIC GTest::main)
set_target_properties(MultibaseTest PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                      ${CMAKE_CURRENT_BINARY_DIR})
add_test(NAME MultibaseTest
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMAND ${CMAKE_CURRENT_BINARY_DIR}/MultibaseTest)
