#include <catch.hpp>
#include <proxything/util.h>
#include <boost/filesystem.hpp>
#include <iostream>
#include <functional>
#include <thread>

using namespace proxything;
using namespace boost::asio;
namespace fs = boost::filesystem;

struct work_bound_test_helper
{
	work_bound_test_helper():
		service(), work(new io_service::work(service)),
		thread([&]{ service.run(); }) { }
	
	~work_bound_test_helper()
	{
		delete work;
		thread.join();
	}
	
	io_service service;
	io_service::work *work;
	std::thread thread;
};

SCENARIO("work-bound handlers work")
{
	io_service service;
	work_bound_test_helper helper;
	
	// If you dispatch a task to an ASIO Service, then from it start an
	// asynchronous process that dispatches a handler back onto the IO service
	// when done, the IO service will think it's out of work and shut down,
	// never allowing the handler to fire
	auto triple_dispatch = [=, &service, &helper](std::function<void(int, int)> cb, int a, int b){
		service.dispatch([=, &service, &helper] {
			helper.service.dispatch([=, &service] {
				service.dispatch(std::bind(cb, a, b));
			});
		});
	};
	
	GIVEN("a callback")
	{
		int result = 0;
		std::function<void(int,int)> cb = [&](int a, int b){
			result = a + b;
		};
		
		WHEN("it's triple-dispatched normally")
		{
			triple_dispatch(cb, 1, 2);
			service.run();
			
			THEN("it's never run")
			{
				REQUIRE(result == 0);
			}
		}
		
		WHEN("it's triple-dispatched work-bound")
		{
			triple_dispatch(util::work_bound(service, cb), 1, 2);
			service.run();
			
			THEN("it's run properly")
			{
				REQUIRE(result == 3);
			}
		}
	}
}

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
