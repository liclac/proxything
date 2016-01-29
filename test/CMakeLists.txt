include_directories("${CMAKE_SOURCE_DIR}/vendor/catch/include")

file(GLOB test_SOURCES test_*.cpp)
foreach(file ${test_SOURCES})
	get_filename_component(target ${file} NAME_WE)
	add_executable(${target} stub.cpp ${file})
	target_link_libraries(${target} proxythinglib Threads::Threads ${Boost_LIBRARIES})
	add_test(NAME ${target} COMMAND ${target})
endforeach()
