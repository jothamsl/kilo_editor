#include <stdio.h>
#include <unistd.h>

int main()
{
    char c;
    while (read(STDERR_FILENO, &c, 1) == 1);
    return 0;
}
