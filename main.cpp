#include "smv.hpp"

extern "C" int MAMain()
{
	InitConsole();
	gConsoleLogging = 1;
	Moblet::run(new SMV());
	return 0;
}

/*
./adb shell
su
mount -o rw,remount rootfs /
chmod 777 /mnt/sdcard
 */
