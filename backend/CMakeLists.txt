cmake_minimum_required(VERSION 3.20)
project(busfinder_backend)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

# fetching dependencies

FetchContent_Declare(
        asio
        GIT_REPOSITORY https://github.com/chriskohlhoff/asio
        GIT_TAG asio-1-38-2
)

FetchContent_MakeAvailable(asio)

set(ASIO_INCLUDE_DIR "${asio_SOURCE_DIR}/asio/include" CACHE PATH "Path to Asio")

FetchContent_Declare(
        crow
        GIT_REPOSITORY https://github.com/crowcpp/crow
        GIT_TAG v1.3.3
)

FetchContent_Declare(
        libpqxx
        GIT_REPOSITORY https://github.com/jtv/libpqxx
        GIT_TAG 8.0.1
)

FetchContent_Declare(
        nanoflann
        GIT_REPOSITORY https://github.com/jlblancoc/nanoflann.git
        GIT_TAG v1.6.0
)

set(JSON_ImplicitConversions OFF CACHE BOOL "" FORCE)

# turning off build tests for dependencies for faster compile times

set(NANOFLANN_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(NANOFLANN_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(CROW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(CROW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_TEST OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(libpqxx nanoflann crow)

# creating a static lib for backend

add_library(busfinder_core_lib STATIC
        src/transporttable.cpp
        src/routing.cpp
        src/stopfinder.cpp
        src/translator.cpp
        src/api.cpp
)

target_include_directories(busfinder_core_lib PUBLIC include)

target_link_libraries(busfinder_core_lib PUBLIC
        pqxx
        nanoflann::nanoflann
        Crow::Crow
)

# setting so all of the targets that link against this lib get c++20

target_compile_features(
        busfinder_core_lib
        PUBLIC
        cxx_std_20
)

# creating the backend executable

add_executable(backend
        src/main.cpp
)


target_link_libraries(backend PRIVATE busfinder_core_lib)

set_target_properties(backend PROPERTIES
        CXX_EXTENSIONS OFF
)

#build option for tests

option(BUSFINDER_BUILD_TESTS "Build BusFinder tests" OFF)

#temporarily turning the test build option on for development
#set(BUSFINDER_BUILD_TESTS ON)

if (BUSFINDER_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif ()
