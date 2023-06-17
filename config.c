#include <stdio.h>

int
main (int argc, char const* argv[])
{
        FILE* fp;
        fp = fopen (argv[1], "w");
        if (fp == NULL) {
                fprintf (stderr, "MAKE CONFIG 错误: 参数不正确\n");
                return 1;
        }
        fprintf (fp,
                 "#ifndef SNAKE_CONFIG_H\n#define SNAKE_CONFIG_H\n\n#define "
                 "SNAKE_RESOURCE_PATH \"%s\"\n\n#endif",
                 argv[2]);
        fclose (fp);
        return 0;
}
