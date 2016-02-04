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
				entry.async_close();
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
					
					entry.async_close();
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
		
		WHEN("it's written to")
		{
			fs_entry entry(service);
			boost::system::error_code open_ec, write_ec;
			std::size_t write_size;
			
			std::string buf_str("This is a test string.");
			std::vector<char> buf(buf_str.begin(), buf_str.end());
			
			entry.async_open(file.path(), std::ios_base::out, [&](const boost::system::error_code &ec) {
				open_ec = ec;
				if (ec) { return; }
				
				async_write(entry, buffer(buf), [&](const boost::system::error_code &ec, std::size_t size) {
					write_ec = ec;
					write_size = size;
					
					entry.async_close();
				});
			});
			service.run();
			
			THEN("it shouldn't error")
			{
				REQUIRE_FALSE(open_ec);
				REQUIRE_FALSE(write_ec);
			}
			
			THEN("it should prepend the data properly")
			{
				REQUIRE(write_size == 22);
				std::ifstream f(file.path());
				std::string content((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
				REQUIRE(content == "This is a test string.");
			}
		}
	}
	
	GIVEN("a path")
	{
		std::string path = util::tmp_path();
		
		WHEN("it's atomically written")
		{
			fs_entry entry(service);
			boost::system::error_code open_ec, write_ec, close_ec;
			bool exists_1, exists_2, exists_3;
			
			std::string buf_str("This is a test string.");
			std::vector<char> buf(buf_str.begin(), buf_str.end());
			
			entry.async_open_atomic(path, [&](const boost::system::error_code &ec) {
				open_ec = ec;
				if (ec) { return; }
				
				exists_1 = fs::exists(path);
				
				async_write(entry, buffer(buf), [&](const boost::system::error_code &ec, std::size_t size) {
					write_ec = ec;
					
					exists_2 = fs::exists(path);
					
					entry.async_close([&](const boost::system::error_code &ec) {
						close_ec = ec;
						
						exists_3 = fs::exists(path);
					});
				});
			});
			service.run();
			
			THEN("it shouldn't error")
			{
				REQUIRE_FALSE(open_ec);
				REQUIRE_FALSE(write_ec);
				REQUIRE_FALSE(close_ec);
			}
			
			THEN("it should not exist until it's closed")
			{
				CHECK_FALSE(exists_1);
				CHECK_FALSE(exists_2);
				CHECK(exists_3);
			}
		}
		
		try {
			fs::remove(path);
		} catch(fs::filesystem_error &e) {
			// Do nothing
		}
	}
}
