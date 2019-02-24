#include <stdio.h>

int main()
{
	struct dev_ps2m_mouse_packet {
		int16_t movement_x;
		int16_t movement_y;
		uint8_t button_l;
		uint8_t button_m;
		uint8_t button_r;
	} ps2m_buffer;
	//printf("ready ok\n");
	FILE *fp = fopen("/dev/mouse", "r");
	if(!fp)
	{
		printf("Failed to open file\n");
		return 0;
	}
	//printf("opened a file ok\n");
	while(1){
		//printf("reading from a buffer ok\n");
		for(int i=0; i<7; i++)
		{
			((uint8_t*)&ps2m_buffer)[i] = fgetc(fp);
		}
		//printf("results ok\n");
		if(ps2m_buffer.button_l)
			printf("\rpressed");
		else
			printf("\rreleased");
	}
	return 0;
}
