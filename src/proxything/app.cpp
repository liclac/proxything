#include <proxything/app.h>
#include <iostream>
#include <string>

using namespace proxything;

app::app()
{
	cli_options.add_options()
		("help,?", "print a help message")
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
	std::cerr << cli_options;
}

int app::run(po::variables_map args)
{
	if (args.count("help")) {
		print_help();
		return 0;
	}
	
	std::cout << "Run on " << args["host"].as<std::string>() << ":" << args["port"].as<unsigned short>() << std::endl;
	
	return 0;
}
