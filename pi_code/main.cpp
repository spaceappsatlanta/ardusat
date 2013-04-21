// main.c  -- simple serial communication test with Arduino

#include "main.h"

#define MAX_BUFLEN 128

int setupConnection(int fd)
{
    struct termios arduinoSettings;
    int ret = -1;

    ret = fcntl(fd, F_SETFL, 0);
    if (ret < 0)
        return(ret);

    // Read current settings for the serial port (fd) into variable "arduinoSettings"
    ret = tcgetattr(fd, &arduinoSettings);
    if (ret < 0)
        return(ret);

    // Set baud rates for serial input to 9600
    ret = cfsetispeed(&arduinoSettings, B9600);
    if (ret < 0)
        return(ret);

    // Set baud rates for serial output to 9600
    ret = cfsetospeed(&arduinoSettings, B9600);
    if (ret < 0)
        return(ret);

    // ------------- Control Options, set through c_cflag ----------------------

    // ignore status lines (such as carrier detect)
    arduinoSettings.c_cflag |= (CLOCAL | CREAD);

    // The Arduino uses the following (hard to change) settings for serial communication:
    // 8 data bits --  No parity -- 1 stop bit
    arduinoSettings.c_cflag &= ~CSIZE;
    arduinoSettings.c_cflag |= CS8;        // 8 data bits
    arduinoSettings.c_cflag &= ~PARENB;    // No parity bit
    arduinoSettings.c_cflag &= ~CSTOPB;    // 1 stop bit (complement of CSTOPB, otherwise 2 stop bits)

    // ------------- Local Options, set through c_lflag ----------------------
    // transfer of data line oriented \n indicates end of buffer
    arduinoSettings.c_lflag |= ICANON;

    arduinoSettings.c_lflag &= ~( ISIG | ECHO | ECHOE); // don't: enable signals, echo input

    // The settings for serial communication with Arduino will be realized through call to tcsetattr()
    // TCSANOW takes care of immediate change of settings; not wait for i/o that takes places at this moment
    ret = tcsetattr(fd, TCSANOW, &arduinoSettings);

    return (ret);
}

int main()
{
    int fd, nRead, nWrite, loopCnt = 0;
    char inTxt[MAX_BUFLEN], outTxt[MAX_BUFLEN];

    // try to open ttyUSB0, supposing this is your PC's USB-connection with your Arduino
    fd = open("/dev/cu.usbmodem1411", O_RDWR | O_NOCTTY);
    if (fd == -1)
    {
        perror("open_port: Unable to open /dev/tty.usbmodem1411 - ");
        return(-1);
    }

    if (setupConnection(fd) == -1)
    {
        close(fd);
        perror("setupConnection(): Unable initialize serial port - ");
        return(-1);
    }

    strcpy(outTxt, "Hello Arduino\n");
    nWrite = write(fd, outTxt, strlen(outTxt));
    memset (inTxt, '\0', MAX_BUFLEN);
    if (nWrite < 0)
    {
        close(fd);
        perror("1 - write: Unable to write to /dev/tty.usbmodem1411 - ");
        return(-1);
    }


    while (true)
    {
        printf("Test\n");
        nRead= read (fd, inTxt, MAX_BUFLEN-1);    // Read data from serial port (minus 1 for last 0-char)
        if(nRead == 0)
            continue;

        if(nRead < 0)
        {
            close(fd);
            perror("read: error reading from /dev/tty.usbmodem1411 - ");
            return(-1);
        }

        printf("--- Text sent by Arduino: %s ---", inTxt); // it gets a '\n' from Arduino's Serial.println()
        memset (inTxt, '\0', MAX_BUFLEN);

        sprintf(outTxt,"loopCounter on PC = %d\n", ++loopCnt);
        nWrite = write(fd, outTxt, strlen(outTxt)); // write data to serial port
        if (nWrite < 0)
        {
            close(fd);
            perror("2 - write: Unable to write to /dev/tty.usbmodem1411 - ");
            return(-1);
        }
    }

    close(fd);
    return(1);
}