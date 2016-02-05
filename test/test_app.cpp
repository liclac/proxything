#include <catch.hpp>
#include <proxything/app.h>
#include <iostream>

using namespace proxything;
namespace po = boost::program_options;

/**
 * Helper to make argc/argv pairs from a string array.
 * 
 * This makes me feel dirty, but it's the only way to accurately test the full
 * parser code path; it uses boost's default CLI parser, which takes argc+argv.
 */
struct args_helper
{
	args_helper(const std::vector<std::string> &args)
	{
		argc = args.size() + 1;
		argv = new char*[argc];
		argv[0] = strdup("proxything");
		for (int i = 1; i < argc; i++) {
			argv[i] = strdup(args[i - 1].data());
		}
	}
	
	~args_helper()
	{
		for (int i = 0; i < argc; i++) {
			free(argv[i]);
		}
		delete[] argv;
	}
	
	int argc;
	char **argv;
};

SCENARIO("argument parsing works")
{
	app a;
	
	GIVEN("no arguments")
	{
		args_helper helper({});
		
		THEN("argc + argv should be correct")
		{
			REQUIRE(helper.argc == 1);
			CHECK(std::string(helper.argv[0]) == "proxything");
		}
		
		WHEN("it's parsed")
		{
			auto args = a.parse_args(helper.argc, helper.argv);
			
			THEN("the arg map should have only default values in it")
			{
				REQUIRE(args.size() == 3);
				
				CHECK(args["threads"].as<unsigned int>() == 1);
				CHECK(args["host"].as<std::string>() == "127.0.0.1");
				CHECK(args["port"].as<unsigned short>() == 12345);
			}
		}
	}
	
	GIVEN("an unknown parameter")
	{
		args_helper helper({"--gibberish"});
		
		THEN("argc + argv should be correct")
		{
			REQUIRE(helper.argc == 2);
			CHECK(std::string(helper.argv[0]) == "proxything");
			CHECK(std::string(helper.argv[1]) == "--gibberish");
		}
		
		THEN("parsing should fail")
		{
			CHECK_THROWS_AS(a.parse_args(helper.argc, helper.argv), boost::program_options::error);
		}
	}
	
	GIVEN("a long-form flag")
	{
		args_helper helper({"--help"});
		
		THEN("it should be present")
		{
			CHECK(a.parse_args(helper.argc, helper.argv).count("help") == 1);
		}
	}
	
	GIVEN("a short-form flag")
	{
		args_helper helper({"-?"});
		
		THEN("the flag should be present")
		{
			CHECK(a.parse_args(helper.argc, helper.argv).count("help") == 1);
		}
	}
}
