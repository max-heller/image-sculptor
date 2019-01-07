cmake_minimum_required(VERSION 3.13)
project(sculptor)

set(CMAKE_CXX_STANDARD 11)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)

include_directories(include src libs)

add_executable(sculptor src/main.cpp src/computations.cpp src/seam.cpp)
add_executable(test tests/main.cpp tests/tests.cpp src/seam.cpp src/computations.cpp)

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