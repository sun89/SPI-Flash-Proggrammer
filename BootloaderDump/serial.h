#include<windows.h>
HANDLE hSerial2;
void serial_init(HANDLE *hSerial, char *port)
{
	DCB dcbSerialParams = {0};
	hSerial = CreateFile(port,GENERIC_READ | GENERIC_WRITE,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
	if(hSerial==INVALID_HANDLE_VALUE)
	{
		//some other error occurred. Inform user.
		printf("Open COM port fail.\n");
		if(GetLastError()==ERROR_FILE_NOT_FOUND)
		{
			//serial port does not exist. Inform user.
			printf("COM port not found.\n");
		}		
		exit(0);
	}
	

	
	dcbSerialParams.DCBlength=sizeof(dcbSerialParams);
	if (!GetCommState(hSerial, &dcbSerialParams)) 
	{
		//error getting state
		printf("GetCommState Error\n");
		exit(0);
	}
	dcbSerialParams.BaudRate=CBR_115200;
	dcbSerialParams.ByteSize=8;
	dcbSerialParams.StopBits=ONESTOPBIT;
	dcbSerialParams.Parity=NOPARITY;
	if(!SetCommState(hSerial, &dcbSerialParams))
	{
		printf("SetCommState Error\n");
		exit(0);
	}
	if (!SetupComm(hSerial,2048,2048))
	{
		printf("Set Buffer size Error\n");
		exit(0);
	}
	//SetupComm(hSerial, 1024, 1024);
	hSerial2 = hSerial;
}

void serial_putchar(unsigned char ch)
{
	DWORD bw;
	WriteFile(hSerial2, &ch, 1, &bw, NULL);	
}
void serial_putbytes(unsigned char *buf, int len)
{
	DWORD bw;
	WriteFile(hSerial2, buf, len, &bw, NULL);
}
unsigned char serial_getchar()
{
	DWORD br;
	char str[100];
	do
	{
		ReadFile(hSerial2, &str[0], 1, &br, NULL);
	}
	while(br < 1);
	
	return str[0];
}
