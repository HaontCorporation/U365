extern int main(int argc, char** argv);
extern unsigned int syscall(unsigned int, ...);
void init_stdio();

int _start(int argc, char** argv)
{
	init_stdio();
	return main(argc, argv);
}
