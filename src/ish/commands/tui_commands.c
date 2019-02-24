#include <tui.h>
#include <tty_old.h>
#include <stdio.h>
#include <fs.h>

void tui_demo_app(int argc __attribute__((unused)), char** argv __attribute__((unused)))
{
    tui_bordered_box(0x07, 0, 0, tty_w-1, tty_h+1);
    tui_draw_text(0x07, 3, 2, "Hello, world! This is our TUI app!\nPress Q to exit");
    tui_draw_text(0x07, tty_w-35, 2, "Toggle me by pressing A [ \x07 ]");
    char c;
    uint8_t toggle = 0;
    while((c = getchar()) != 'q')
    {
        if(c == 'a')
        {
            toggle = !toggle;
            tui_draw_text(0x07, tty_w-35, 2, (toggle) ? "Toggle me by pressing A [ \xFE ]" : "Toggle me by pressing A [ \x07 ]");
        }
    }
    initTTY();
}

int tui_demo_fileviewer(int argc, char **argv)
{
    const char* program_name = argv[0];
    if(argc < 2)
    {
        fprintf(stderr, "%s: not enough arguments. Usage: %s <filename>\n", program_name, program_name);
        return -1;
    }
    if(argc > 2)
    {
        fprintf(stderr, "%s: sorry, you can open only one file now\n", program_name);
        return -1;
    }
    FILE *fp = fopen(argv[1], "r");
    tui_bordered_box(0x07, 0, 0, tty_w-1, tty_h+1);
    tui_draw_text(0x08, 2, 2, argv[1]);
    struct stat stat_results;
    if(stat(argv[1], &stat_results))
    {
        fprintf(stderr, "%s: no such file or directory: %s", program_name, argv[1]);
        return -1;
    }
    char* file_contents = malloc(stat_results.st_size);
    for(int i = 0; (file_contents[i] = fgetc(fp)) != EOF; i++)
        ;
    char* file_size_str = malloc(32);
    sprintf(file_size_str, "size: %d bytes", stat_results.st_size);
    tui_draw_text(0x08, tty_w-5-strlen(file_size_str), 2, file_size_str);
    tui_draw_lim_text(0x07, 2, 4, tty_w-8, tty_h-6, file_contents, stat_results.st_size);
    tui_draw_text(0x08, 2, tty_h-2, "Press Q to exit");
    while(getchar() != 'q');
    initTTY();
    return 0;
}
