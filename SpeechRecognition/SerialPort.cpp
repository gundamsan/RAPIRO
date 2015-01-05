#include "SerialPort.h"

SerialPort::SerialPort(char* devicePath)
{
	fd = -1;

	// �f�o�C�X���I�[�v���B
	fd = open(devicePath, O_RDWR | O_NOCTTY);
	if (fd < 0)
	{
		// �f�o�C�X�I�[�v���Ɏ��s�B
		perror(devicePath);
		exit(-1);
	}

	// �����̐ݒ���擾�E�ۑ��B
	tcgetattr(fd, &oldTermio);

	// new_term_io�̐��䕶���̏������B
	newTermio.c_iflag = 0;
	newTermio.c_oflag = 0;
	newTermio.c_cflag = 0;
	newTermio.c_lflag = 0;
	newTermio.c_line = 0;

	memset(newTermio.c_cc, '\0', sizeof(newTermio.c_cc));
	
	// �|�[�g�̐ݒ�B
	//  B9600 = 9600bps �i�Ƃ肠����9600bps�j
	//  CS8 = 8bit, no parity, 1 stopbit
	//  CLOCAL = ���[�J���ڑ��i���f������Ȃ��j
	//  CREAD = ��M�����L���B
	newTermio.c_cflag = B9600 | CS8 | CLOCAL | CREAD;


	// �p���e�B�f�[�^�G���[�͖����ɐݒ�B
	newTermio.c_iflag = IGNPAR;

	// Raw���[�h�o�́B
	newTermio.c_oflag = 0;
	// ���̓��[�h�ݒ�Bnon-canonical, no echo,
	newTermio.c_lflag = 0;
	// inter-character timer
	newTermio.c_cc[VTIME] = 0;
	// 1��������܂œǂݍ��݂��u���b�N�B
	newTermio.c_cc[VMIN] = 1;
	// ���f�����C���̃N���A�B
	tcflush(fd, TCIFLUSH);
	
	// �V�����ݒ��K�p�B
	tcsetattr(fd, TCSANOW, &newTermio);
}

SerialPort::~SerialPort()
{
	end();
}

int SerialPort::begin(int bps)
{
	int rtn = -1;

	if (fd < 0)	return -2;

	switch (bps)
	{
	case 300:
		newTermio.c_cflag = B300 | CS8 | CLOCAL | CREAD;
		rtn = bps;
		break;
	case 1200:
		newTermio.c_cflag = B1200 | CS8 | CLOCAL | CREAD;
		rtn = bps;
		break;
	case 2400:
		newTermio.c_cflag = B2400 | CS8 | CLOCAL | CREAD;
		rtn = bps;
		break;
	case 4800:
		newTermio.c_cflag = B4800 | CS8 | CLOCAL | CREAD;
		rtn = bps;
		break;
	case 9600:
		newTermio.c_cflag = B9600 | CS8 | CLOCAL | CREAD;
		rtn = bps;
		break;
	case 19200:
		newTermio.c_cflag = B19200 | CS8 | CLOCAL | CREAD;
		rtn = bps;
		break;
	case 38400:
		newTermio.c_cflag = B38400 | CS8 | CLOCAL | CREAD;
		rtn = bps;
		break;
	case 57600:
		newTermio.c_cflag = B57600 | CS8 | CLOCAL | CREAD;
		rtn = bps;
		break;
	case 115200:
		newTermio.c_cflag = B115200 | CS8 | CLOCAL | CREAD;
		rtn = bps;
		break;
	default:
		break;
	}

	if (rtn != -1)
	{
		// �V�����ݒ��K�p�B
		tcsetattr(fd, TCSANOW, &newTermio);
	}

	sleep(3);

	return rtn;
}

int SerialPort::end()
{
	int rtn = -1;

	if (fd >= 0)
	{
		tcsetattr(fd, TCSANOW, &oldTermio);
		close(fd);
		rtn = 0;
	}

	return rtn;
}

int SerialPort::writeb(uint8_t byte)
{
	ssize_t res;

	res = write(fd, &byte, 1);

	return res;
}

uint8_t SerialPort::readb()
{
	unsigned char c;
	ssize_t res;

	res = read(fd, (char *)&c, 1);

	return c;
}

int SerialPort::writeBytes(char *data, int bytes)
{
	return write(fd, data, bytes);
}
