#ifndef PORT_H
#define PORT_H

#define BAUDRATE B9600
#define PORTNAME "/dev/ttyACM0"

#include <errno.h>
#include <fcntl.h> 
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include "port.c"

int set_interface_attribs(int fd);

#endif /* __PORT_H__ */
