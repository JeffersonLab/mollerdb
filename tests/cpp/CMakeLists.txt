# C++ Tests for mollerdb

# Test executable for Database tests
add_executable(test_database test_database.cpp)

target_link_libraries(test_database
    PRIVATE
    mollerdb_core
    GTest::gtest_main
)

target_include_directories(test_database
    PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../../include"
)

# Add tests to CTest
include(GoogleTest)
gtest_discover_tests(test_database)
