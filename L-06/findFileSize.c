#include <stdio.h>

int main()
{
    long fileSize;
    FILE *f;
    f = fopen("home/praful/bro/bro.cpp", "rb"); // rb means read in binary mode
    fseek(f, 0, SEEK_END);
    fileSize = ftell(f);
    fclose(f);
    printf("%ld\n", fileSize);
}