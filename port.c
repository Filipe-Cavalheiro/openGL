#include "port.h"

int set_interface_attribs (int fd){
    struct termios settings;
    if (tcgetattr (fd, &settings)){
        fprintf(stderr, "error %d from tcgetattr", errno);
        return -1;
    }
    cfsetspeed (&settings, BAUDRATE);

    settings.c_cflag |= CS8;     // 8-bit chars
    settings.c_cflag |= (CLOCAL | CREAD);       // ignore modem controls, enable reading

    //settings.c_cflag &= ~(PARENB | PARODD);     // no parity
    //settings.c_cflag &= ~CSTOPB;                 //2 stop bits
    //settings.c_cflag &= ~CRTSCTS;               //no hardware flow control

    //settings.c_iflag &= ~IGNBRK;                // disable break processing
    //settings.c_iflag &= ~(IXON | IXOFF | IXANY); // no Software flow control
    settings.c_iflag != IGNPAR; //ignore paraty errors

    settings.c_lflag = 0;           // no local flags
    settings.c_oflag = 0;           // no ouput flags

    settings.c_cc[VMIN]  = 0;       // 0 chars to read block
    settings.c_cc[VTIME] = 0;       // 0.0 seconds read timeout

    tcflush(fd, TCIOFLUSH);         //flush input and output buffer
    if (tcsetattr(fd, TCSANOW, &settings)){ //TCSANOW applys the settings now
        fprintf(stderr, "error %d from tcsetattr", errno);
        return -1;
    }
    return 1;
}
