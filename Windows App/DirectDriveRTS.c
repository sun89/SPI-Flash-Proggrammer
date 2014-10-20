#include<stdio.h>
#include<windows.h>
HANDLE hSerial;
void serial_init()
{
	DCB dcbSerialParams = {0};
	hSerial = CreateFile("COM6",GENERIC_READ | GENERIC_WRITE,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
	if(hSerial==INVALID_HANDLE_VALUE)
	{
		//some other error occurred. Inform user.
		printf("Open COM port fail.\n");
		if(GetLastError()==ERROR_FILE_NOT_FOUND)
		{
			//serial port does not exist. Inform user.
			printf("COM port not found.\n");
		}		
	}
	

	
	dcbSerialParams.DCBlength=sizeof(dcbSerialParams);
	if (!GetCommState(hSerial, &dcbSerialParams)) 
	{
		//error getting state
		printf("GetCommState Error\n");
	}
	dcbSerialParams.BaudRate=CBR_115200;
	dcbSerialParams.ByteSize=8;
	dcbSerialParams.StopBits=ONESTOPBIT;
	dcbSerialParams.Parity=NOPARITY;
	if(!SetCommState(hSerial, &dcbSerialParams))
	{
		printf("SetCommState Error\n");
	}
	if (!SetupComm(hSerial,2048,2048))
	{
		printf("Set Buffer size Error\n");
	}
	//SetupComm(hSerial, 1024, 1024);
}

int main()
{
	serial_init();
	EscapeCommFunction(hSerial, SETRTS);
	Sleep(2);
	EscapeCommFunction(hSerial, CLRRTS);
	Sleep(2);
	return 0;
}
