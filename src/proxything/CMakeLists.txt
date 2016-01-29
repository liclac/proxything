# Build a library out of the non-main sources; allows easy code sharing with unit tests
set(proxything_SOURCES
	app.cpp
	server.cpp
)
add_library(proxythinglib ${proxything_SOURCES})

# Build an executable and link it to the core library
add_executable(proxything main.cpp)
target_link_libraries(proxything proxythinglib Threads::Threads ${Boost_LIBRARIES})
