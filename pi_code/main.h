// main.h required includes for simple serial communication test with Arduino
#ifndef main_h
#define main_h

#define LASTLOOP 50 // Increase/decrease  for a longer/shorter test-time

#include <stdio.h>
#include <string.h>
#include <unistd.h>    // UNIX standard function definitions
#include <fcntl.h>       // File control definitions
#include <errno.h>    // Error number definitions
#include <termios.h> // POSIX terminal control

extern int setupConnection(int fd);

#endif