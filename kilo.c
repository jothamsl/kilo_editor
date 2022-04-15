// INCLUDES
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

// DATA
struct termios orgin_termios;

// TERMINAL
void die(const char *s) {
  perror(s);
  exit(1);
}

void disableRawMode() {
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orgin_termios) == -1)
    die("tcsetattr");
}

void enableRawMode() {

  // Get current terminal attributes from "orgin_termios" struct
  if (tcgetattr(STDIN_FILENO, &orgin_termios) == -1)
    die("tcgetattr");

  // When exit is called, restore initial terminal state
  atexit(disableRawMode);

  // Create struct to hold terminal attributes
  struct termios raw = orgin_termios;

  // The local state (c_lflag) and input state (c_iflag) are represented as a
  // sequence of bits. To modify certain terminal attributes (bitflags), we need
  // to apply some bitwise operations.
  //
  // It's like modifying the settings of an application. We can set a feature on
  // or off.
  //
  // Here, we're changing the value of the "local flags" (Dumping
  // ground for another terminal state), "input flags", "output flags" and other
  // attributes by disabling IXON, ECHO, ICANON & ISIG.
  //
  // ~(ECHO)   - Disables output when typing
  // ~(ICANON) - Disables reading input line by line (reads byte by byte)
  // ~(ISIG)   - Disables ctrl+c and ctrl+z signals
  // ~(IXON)   - Disables ctrl+s and ctrl+q signals
  // ~(IEXTEN) - Disables ctrl+v and ctrl+o signals
  // ~(ICRNL)  - Disables ctrl+m (disables term conversion of carriage return
  // "\r" to newline "\n")
  // ~(OPOST)  - Disables all output processing features (especially terminal
  // addition of "\r\n")
  //
  // The remaining bitflags are miscallaneous changes
  raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
  raw.c_oflag &= ~(OPOST);
  raw.c_cflag |= ~(CS8);
  raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);

  // [VMIN] - Minimum amount of bytes to be read before "read()" can return
  // [VTIME] - Maximum amount of time to wait before "read()" returns
  raw.c_cc[VMIN] = 10;
  raw.c_cc[VTIME] = 1;

  // Update terminal attributes using new state of "raw" struct
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
    die("tcsetattr");
}

// INIT

int main() {
  enableRawMode(); // Disables printing to terminal (Disables echo)
  while (1) {
    char c = '\0';
    // errno is a global variable which holds the error value when an error
    // occurs
    if (read(STDERR_FILENO, &c, 1) == -1 && errno != EAGAIN)
      die("read");

    // Check if character is a control character (0-31 on ASCII Table)
    if (iscntrl(c)) {
      // Print carriage return and newline each time (OPOST disabled)
      printf("%d\r\n", c);
    } else
      printf("%d ('%c')\r\n", c, c);
    if (c == 'q')
      break;
  }
  return 0;
}
