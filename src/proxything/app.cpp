#include <proxything/app.h>
#include <iostream>

using namespace proxything;

app::app() { }

app::~app() { }

int app::run(int argc, char **argv)
{
	std::cout << "Lorem ipsum dolor sit amet" << std::endl;
	
	return 0;
}
