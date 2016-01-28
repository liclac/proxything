include_directories("${CMAKE_SOURCE_DIR}/vendor/catch/include")

file(GLOB test_SOURCES test_*.cpp)
foreach(file ${test_SOURCES})
	get_filename_component(target ${file} NAME_WE)
	add_executable(${target} stub.cpp ${file})
	add_test(NAME ${target} COMMAND ${target})
endforeach()
