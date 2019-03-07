
#include <compare.h>
#include <uname.h>
#include <env.h>

#define OSVER amd64-1.0

struct utsname get_uname()
{
	struct utsname ret;
	ret.system_name = "U365";
	//ret.hostname = (char*) get_env_variable("HOSTNAME");
	ret.version = QUOT(OSVER);
	ret.release = __DATE__ " " __TIME__;
	ret.machine = "x86";
	ret.os_name = "U365";
	return ret;
}