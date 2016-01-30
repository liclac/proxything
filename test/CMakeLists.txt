include_directories("${CMAKE_SOURCE_DIR}/vendor/catch/include")

set(proxything_TESTS
	test_test
	test_client_connection
)

foreach(target ${proxything_TESTS})
	add_executable(${target} stub.cpp "${target}.cpp")
	target_link_libraries(${target} proxythinglib Threads::Threads ${Boost_LIBRARIES})
	add_test(NAME ${target} COMMAND ${target})
endforeach()
