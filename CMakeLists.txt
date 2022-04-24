cmake_minimum_required(VERSION 3.20)
project(Allie-Cart)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

include(FetchContent)

FetchContent_Declare(SFML GIT_REPOSITORY "https://github.com/SFML/SFML.git" GIT_TAG "2.5.1")
FetchContent_MakeAvailable(SFML)

add_subdirectory(src)

add_executable(cart WIN32 ${CART_SOURCES})
target_link_libraries(cart sfml-audio sfml-graphics sfml-main)
