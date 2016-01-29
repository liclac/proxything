file(GLOB_RECURSE proxything_SOURCES *.cpp)
add_executable(proxything ${proxything_SOURCES})
target_link_libraries(proxything ${Boost_LIBRARIES})
