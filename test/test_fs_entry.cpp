#include <catch.hpp>
#include <proxything/fs_entry.h>
#include <iostream>

using namespace proxything;
using namespace boost::asio;

SCENARIO("files can be opened")
{
	io_service service;
	
	GIVEN("a path")
	{
		std::string path("/tmp/proxything_test.txt");
		
		WHEN("it's opened")
		{
			fs_entry entry(service);
			entry.async_open(path, [=](const boost::system::error_code &ec) {
				std::cout << ec << std::endl;
			});
			service.run();
		}
	}
}
