#include <catch.hpp>
#include <proxything/fs_entry.h>
#include <proxything/util.h>

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
			boost::system::error_code ec;
			entry.async_open(path, [&](const boost::system::error_code &ec_) {
				ec = ec_;
			});
			service.run();
			
			REQUIRE(!ec);
		}
	}
}
