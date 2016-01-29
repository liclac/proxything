#include <proxything/app.h>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include <iostream>
#include <string>

using namespace proxything;

app::app()
{
	m_options.add_options()
		("help,?", "print a help message")
		("quiet,q", "show less output")
		("verbose,v", "show more output")
		("debug,V", "show even more output")
		("host,h", po::value<std::string>()->default_value("127.0.0.1"), "host to bind to")
		("port,p", po::value<unsigned short>()->default_value(12345), "port to bind to")
	;
}

app::~app() { }



int app::run(int argc, char **argv)
{
	po::variables_map args;
	
	try {
		args = parse_args(argc, argv);
	} catch(po::error &e) {
		// It's possible to catch specialized exceptions to write custom messages for each kind of
		// error, but for now, the generic error's what() will suffice (it's just crudely worded)
		std::cerr << e.what() << std::endl;
		return 1;
	}
	
	return run(args);
}



po::variables_map app::parse_args(int argc, char **argv)
{
	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, m_options), vm);
	po::notify(vm);
	
	return vm;
}

void app::print_help()
{
	std::cerr << "Usage: proxything [options]" << std::endl;
	std::cerr << std::endl;
	std::cerr << "Available options:" << std::endl;
	std::cerr << m_options;
}

void app::init_logging(po::variables_map args)
{
	auto level = boost::log::trivial::info;
	
	if (args.count("debug")) {
		level = boost::log::trivial::trace;
	} else if (args.count("verbose")) {
		level = boost::log::trivial::debug;
	} else if (args.count("quiet")) {
		level = boost::log::trivial::warning;
	}
	
	boost::log::core::get()->set_filter(boost::log::trivial::severity >= level);
}

int app::run(po::variables_map args)
{
	if (args.count("help")) {
		print_help();
		return 0;
	}
	
	init_logging(args);
	
	BOOST_LOG_TRIVIAL(info) << "Run on " << args["host"].as<std::string>() << ":" << args["port"].as<unsigned short>();
	BOOST_LOG_TRIVIAL(debug) << "Debug information goes here";
	BOOST_LOG_TRIVIAL(trace) << "This isn't interesting to anyone else";
	BOOST_LOG_TRIVIAL(error) << "ERROR! Nothing is implemented!";
	
	return 0;
}
