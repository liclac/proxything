cmake_minimum_required(VERSION 3.2)
project(proxything VERSION 0.0.1)

# Put executables and libraries in /bin and /lib respectively
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

# Default to building in C++11 mode
set(CMAKE_CXX_STANDARD 11)

# Include project headers
include_directories("${CMAKE_SOURCE_DIR}/include")

# Build sources
add_subdirectory(src)

# Add tests
enable_testing()
add_subdirectory(test)
