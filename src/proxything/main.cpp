#include <proxything/app.h>

/**
 * Main entry point.
 * 
 * Simply defers to proxything::app.
 * 
 * @param  argc Argument count
 * @param  argv Argument values
 * @return      Exit code
 */
int main(int argc, char **argv) {
	proxything::app a;
	return a.run(argc, argv);
}
