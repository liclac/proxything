#include <catch.hpp>
#include <proxything/util.h>
#include <boost/filesystem.hpp>
#include <iostream>

using namespace proxything;
namespace fs = boost::filesystem;

SCENARIO("temporary files work")
{
	WHEN("a temporary file is created")
	{
		auto f = std::make_shared<util::tmp_file>();
		
		THEN("the file should exist")
		{
			REQUIRE(fs::exists(f->path()));
			
			WHEN("the object is destroyed")
			{
				std::string path = f->path();
				f.reset();
				
				THEN("the file should be as well")
				{
					REQUIRE_FALSE(fs::exists(path));
				}
			}
		}
	}
}
