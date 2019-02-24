#ifndef CMDS_H
#define CMDS_H

int help(int,char**);
int sysinfo(int,char**);
int atable();
int emptyCmd();
int kotiki();
int gpf();
int smb_data();
int cmd_cat(int,char**);
int cmd_date();
int cmd_lscpu();
int cmd_echo(int, char**);
int cmd_vfs_test();
int cmd_errno_test();
int cmd_all_test();
int cmd_reboot();
int showlogo();
int ish2();
int bfntest();
int surface_test(int, char**);
int gradient_test(int, char**);
int mouse_test();
int cmd_elf_test();
int cmd_ls(int, char**);
int cmd_cd(int, char **);
int cmd_uname(int, char **);
int cmd_pwd();
int cmd_print_debug_info();
int font_viewer();
int cmd_write_ttf_str();
int ls_dev();
int pen_main();
int mount_main(int, char**);
int export_main(int, char**);
int tui_demo_app(int, char**);
int tui_demo_fileviewer(int, char **);

#endif // CMDS_H
