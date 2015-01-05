#ifndef _ISHIWATA_SERIALPORT_H
#define _ISHIWATA_SERIALPORT_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/wait.h>
#include <sys/ioctl.h>

// シリアルポートクラス。
class SerialPort
{
private:
	int fd;
	struct termios oldTermio;
	struct termios newTermio;
public:
	SerialPort(char* devicePath);
	~SerialPort();
	int begin(int bps);
	int end();
	int writeb(uint8_t byte);
	uint8_t readb();
	int writeBytes(char *data, int bytes);
};

#endif