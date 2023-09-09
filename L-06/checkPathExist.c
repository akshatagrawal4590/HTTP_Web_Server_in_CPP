#include <stdio.h>
#include <sys/stat.h>

int main()
{
    int x;
    struct stat s;

    // For every valid path the stat function return 0
    // For every invalid path the stat function return -1

    // If 's.st_mode & S_IFDIR' is non-zero then the specified path is of a folder
    // If 's.st_mode & S_IFDIR' is zero then the specified path is of a file

    x = stat("/home", &s);
    printf("%d\n", x); // output = 0
    printf("%d\n", s.st_mode & S_IFDIR); // output = 16384
    x = stat("/home/praful", &s);
    printf("%d\n", x); // output = 0
    printf("%d\n", s.st_mode & S_IFDIR);
    x = stat("/home/praful/bro/bro.cpp", &s); // output = 16384
    printf("%d\n", x); // output = 0
    printf("%d\n", s.st_mode & S_IFDIR); // output = 0
    x = stat("/cartoon", &s);
    printf("%d\n", x); // output = -1
    x = stat("/home/praful/whatever.xyz", &s);
    printf("%d\n", x); // output = -1
}