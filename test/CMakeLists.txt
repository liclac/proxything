include_directories("${CMAKE_SOURCE_DIR}/vendor/catch/include")

set(proxything_TESTS
	test_test
	test_client_connection
	test_fs_entry
	test_util
)

add_library(test_stub stub.cpp)

foreach(target ${proxything_TESTS})
	add_executable(${target} "${target}.cpp")
	target_link_libraries(${target} test_stub proxythinglib Threads::Threads ${Boost_LIBRARIES})
	add_test(NAME ${target} COMMAND ${target})
endforeach()
