#include "smv.hpp"

extern "C" int MAMain()
{
	InitConsole();
	gConsoleLogging = 1;
	Moblet::run(new SMV());
	return 0;
}
