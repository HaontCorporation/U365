#include <compare.h>
#include <uname.h>
#include <env.h>
struct utsname get_uname()
{
	struct utsname ret;
	ret.system_name = "U365";
	ret.hostname = (char*) get_env_variable("HOSTNAME");
	ret.release = QUOT(OSVER);
	ret.version = __DATE__ " " __TIME__;
	ret.machine = "x86";
	ret.os_name = "U365";
	return ret;
}