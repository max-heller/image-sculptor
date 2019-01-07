cmake_minimum_required(VERSION 3.13)
project(sculptor)

set(CMAKE_CXX_STANDARD 11)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)

include_directories(include src libs)

file(GLOB SRC src/*)
file(GLOB TEST_SRC src/*)
list(FILTER TEST_SRC EXCLUDE REGEX "main.cpp")
file(GLOB TESTS tests/*)

add_executable(sculptor ${SRC})
add_executable(test ${TESTS} ${TEST_SRC})

find_package(X11 REQUIRED)
include_directories(${X11_INCLUDE_DIR})
target_link_libraries(sculptor ${X11_LIBRARIES})
target_link_libraries(test ${X11_LIBRARIES})

find_package(JPEG REQUIRED)
include_directories(${JPEG_INCLUDE_DIR})
target_link_libraries(sculptor ${JPEG_LIBRARY})
target_link_libraries(test ${JPEG_LIBRARY})

find_package(Threads REQUIRED)
target_link_libraries(sculptor Threads::Threads)
target_link_libraries(test Threads::Threads)