cmake_minimum_required(VERSION 3.2)
project(proxything VERSION 0.0.1)

# Put executables and libraries in /bin and /lib respectively
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

# Default to building in C++11 mode
set(CMAKE_CXX_STANDARD 11)

# Include project headers
include_directories("${CMAKE_SOURCE_DIR}/include")

# Find Boost
add_definitions(-DBOOST_ALL_DYN_LINK=1)
find_package(Boost 1.59 REQUIRED COMPONENTS program_options log filesystem thread system exception)
include_directories(${Boost_INCLUDE_DIRS})

# Find the system thread library
find_package(Threads)

# Build sources
add_subdirectory(src)

# Add tests
enable_testing()
add_subdirectory(test)

# Build documentation if Doxygen is installed
find_package(Doxygen)
if(DOXYGEN_FOUND)
	configure_file(${CMAKE_SOURCE_DIR}/Doxyfile.in ${CMAKE_BINARY_DIR}/Doxyfile)
	add_custom_target(docs ALL
		COMMAND ${DOXYGEN_EXECUTABLE} VERBATIM
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		DEPENDS proxything
	)
endif()
