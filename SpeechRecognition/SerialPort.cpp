#include "SerialPort.h"

SerialPort::SerialPort(char* devicePath)
{
	fd = -1;

	// デバイスをオープン。
	fd = open(devicePath, O_RDWR | O_NOCTTY);
	if (fd < 0)
	{
		// デバイスオープンに失敗。
		perror(devicePath);
		exit(-1);
	}

	// 既存の設定を取得・保存。
	tcgetattr(fd, &oldTermio);

	// new_term_ioの制御文字の初期化。
	newTermio.c_iflag = 0;
	newTermio.c_oflag = 0;
	newTermio.c_cflag = 0;
	newTermio.c_lflag = 0;
	newTermio.c_line = 0;

	memset(newTermio.c_cc, '\0', sizeof(newTermio.c_cc));
	
	// ポートの設定。
	//  B9600 = 9600bps （とりあえず9600bps）
	//  CS8 = 8bit, no parity, 1 stopbit
	//  CLOCAL = ローカル接続（モデム制御なし）
	//  CREAD = 受信文字有効。
	newTermio.c_cflag = B9600 | CS8 | CLOCAL | CREAD;


	// パリティデータエラーは無視に設定。
	newTermio.c_iflag = IGNPAR;

	// Rawモード出力。
	newTermio.c_oflag = 0;
	// 入力モード設定。non-canonical, no echo,
	newTermio.c_lflag = 0;
	// inter-character timer
	newTermio.c_cc[VTIME] = 0;
	// 1文字来るまで読み込みをブロック。
	newTermio.c_cc[VMIN] = 1;
	// モデムラインのクリア。
	tcflush(fd, TCIFLUSH);
	
	// 新しい設定を適用。
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
		// 新しい設定を適用。
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
