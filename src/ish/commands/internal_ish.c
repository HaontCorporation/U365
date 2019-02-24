#include <string.h>
#include <stdio.h>
#include <ish.h>

void emptyCmd(){} //Called when empty command is written.

int help(int argc, char **argv) //Displaying command list or help for specific command.
{
    int j;
    if(argc>1) //If we have some arguments...
    {
        //Don't display all command list and display only specified command names and description.
        for(j=1; j<argc; j++)
        {
            int i;
            for(i=0; i<=256 && strcmp(cmds[i].name, argv[j])!=0; i++) //Find command's index in command table.
                ;
            printf("%s\t\t%s\n", cmds[i].name, cmds[i].description);  //Print command name and description to the screen.
        }
        return 0; //And just exit the "program".
    }
    printf(
        "ISH 1.0\n"
        "U365 Kernel Integrated Shell. Known commands:\n"); //Display welcome message.

    int maxlen = 0;
    int currlen;
    int i;

    for(i = 0; i < cmdNum; ++i)
    {
        currlen = strlen(cmds[i].name);
        if(maxlen < currlen)
        {
            maxlen = currlen;
        }
    }
    for(i = 0; i < cmdNum; ++i)
    {
        printf("[%-*s]    %s\n", maxlen, cmds[i].name, cmds[i].description);
    }
    return 0;
}
