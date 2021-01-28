#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[])
{
    pid_t pID = getpid();
    fprintf(stderr, "%d\n\n", pID);

    FILE *file = fopen(argv[1], "r");

    char *envVal = getenv("CATMATCH_PATTERN");

    while (1)
    {
        if (feof(file))
        {
            break;
        }
        char line[100];
        fgets(line, 100, file);
        char *substring = strstr(line, envVal);
        int isTrue = 0;
        if (substring)
        {
            isTrue = 1;
        }
        printf("%d %s", isTrue, line);
    }

    //I COMPLETED THE TMUX EXERCISE!
}