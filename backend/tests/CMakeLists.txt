FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest
        GIT_TAG v1.18.0
)

FetchContent_MakeAvailable(googletest)

add_executable(busfinder_tests
        unit/test_stopdata_conversion.cpp
        unit/test_node_ordering.cpp
        unit/test_time_formatting.cpp
        unit/test_api_parse_time.cpp
        unit/test_csv_parsing.cpp)

target_link_libraries(busfinder_tests PRIVATE
        busfinder_core_lib
        GTest::gtest_main)

# making discoverable by CTest

include(GoogleTest)
gtest_discover_tests(busfinder_tests WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
