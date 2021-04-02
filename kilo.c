#include <stdio.h>
#include <unistd.h>
#include <termios.h>

void enableRawMode();

int main()
{
    enableRawMode(); // Disables printing to terminal (Disables echo)

    char c;
    // Read function takes 1 byte input from stdio and returns number of bytes read
    // It returns 0 when it reaches the end of a file
    while (read(STDERR_FILENO, &c, 1) == 1 && c != 'q'); 
    return 0;
}

void enableRawMode()
{
    struct termios raw; 

    tcgetattr(STDIN_FILENO, &raw);
    raw.c_lflag &= ~(ECHO);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}
