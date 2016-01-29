#include <proxything/app.h>
#include <iostream>
#include <string>

using namespace proxything;

app::app()
{
	cli_options.add_options()
		("help,?", "print a help message")
		("quiet,q", "show less output")
		("verbose,v", "show more output")
		("debug,V", "show lots of output")
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
	po::store(po::parse_command_line(argc, argv, cli_options), vm);
	po::notify(vm);
	
	return vm;
}

void app::print_help()
{
	std::cerr << "Usage: proxything [options]" << std::endl;
	std::cerr << std::endl;
	std::cerr << "Available options:" << std::endl;
	std::cerr << cli_options;
}

void app::init_logging(po::variables_map args)
{
	int verbosity = 1;
	if (args.count("debug")) {
		verbosity = 3;
	} else if (args.count("verbose")) {
		verbosity = 2;
	} else if (args.count("quiet")) {
		verbosity = 0;
	}
	
	std::cout << verbosity << std::endl;
}

int app::run(po::variables_map args)
{
	if (args.count("help")) {
		print_help();
		return 0;
	}
	
	init_logging(args);
	
	std::cout << "Run on " << args["host"].as<std::string>() << ":" << args["port"].as<unsigned short>() << std::endl;
	
	return 0;
}
