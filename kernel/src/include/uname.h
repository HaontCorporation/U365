
struct utsname
{
	char *system_name;
	char *hostname;
	char *release;
	char *version;
	char *machine;
	char *os_name;
};

struct utsname get_uname();