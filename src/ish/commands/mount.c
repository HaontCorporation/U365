#include <mtab.h>
#include <stdio.h>

int mount_main(int argc, char** argv)
{
	if(argc < 2)
	{
		ls_mnt();
		return 0;
	}
	if(argc == 3)
	{
		// mount dev argv[1] to the path argv[2]
		printf("Device \"%s\" shall now be mount to the path \"%s\", but mount is still not functional. We are sorry.\n", argv[1], argv[2]);
	}
	return 0;
}
