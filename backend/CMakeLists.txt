cmake_minimum_required(VERSION 3.20)
project(busfinder_backend)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

FetchContent_Declare(
        libpqxx
        GIT_REPOSITORY https://github.com/jtv/libpqxx
        GIT_TAG 8.0.1
)

set(BUILD_TESTING OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(libpqxx)

add_executable(backend
        src/transporttable.cpp
        src/main.cpp
#        src/routing.cpp
)

target_include_directories(backend PRIVATE include)

target_link_libraries(backend PRIVATE pqxx)

set_target_properties(backend PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
)

# copy the .env file to the build folder
configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/.env
        ${CMAKE_CURRENT_BINARY_DIR}/.env
        COPYONLY
)
