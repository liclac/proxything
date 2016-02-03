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
		
		WHEN("it's read from")
		{
			fs_entry entry(service);
			boost::system::error_code open_ec, read_ec;
			std::size_t read_size;
			std::vector<char> buf(1337);
			entry.async_open(file.path(), [&](const boost::system::error_code &ec) {
				open_ec = ec;
				if (ec) { return; }
				
				async_read(entry, buffer(buf), [&](const boost::system::error_code &ec, std::size_t size) {
					read_ec = ec;
					read_size = size;
				});
			});
			service.run();
			
			THEN("it shouldn't error")
			{
				REQUIRE_FALSE(open_ec);
				REQUIRE(read_ec == error::eof);
			}
			
			THEN("it should read the correct data")
			{
				REQUIRE(read_size == 26);
				std::string content(buf.begin(), buf.begin() + read_size);
				REQUIRE(content == "Lorem ipsum dolor sit amet");
			}
		}
	}
}
