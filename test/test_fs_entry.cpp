#include <catch.hpp>
#include <proxything/fs_entry.h>
#include <proxything/util.h>

using namespace proxything;
using namespace boost::asio;

SCENARIO("files can be opened")
{
	io_service service;
	
	GIVEN("a file")
	{
		util::tmp_file file("Lorem ipsum dolor sit amet");
		
		WHEN("it's opened")
		{
			fs_entry entry(service);
			boost::system::error_code ec;
			entry.async_open(file.path(), [&](const boost::system::error_code &ec_) {
				ec = ec_;
			});
			service.run();
			
			THEN("it shouldn't error")
			{
				REQUIRE_FALSE(ec);
			}
		}
	}
}
