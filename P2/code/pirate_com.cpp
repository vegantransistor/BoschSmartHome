#include <stdio.h> 
#include <windows.h> 
#include <winbase.h>
#include <iostream>
#include <tchar.h>

#include "pirate_com.h"

HANDLE pirate_com_handle;

int pirate_init_com(int com_nb, int baudrate)
{
	WCHAR port_name[10];

	if (com_nb < 10) {
		wcscpy_s(port_name, L"\\\\.\\COMx");
		port_name[7] = com_nb + 0x30;
	}
	else if (com_nb < 100) {
		wcscpy_s(port_name, L"\\\\.\\COMxx");
		port_name[7] = (com_nb % 10) + 0x30;
		port_name[8] = (com_nb / 10) + 0x30;
	}
	else {
		printf("Invalid COM!");
		return -1;
	}
	COMMTIMEOUTS cto;


	DCB config = {0};
    
	pirate_com_handle = CreateFile(port_name,
         GENERIC_READ|GENERIC_WRITE,//access ( read and write)
        0,    //(share) 0:cannot share the COM port                        
        0,    //security  (None)                
        OPEN_EXISTING,// creation : open_existing
        0,    // we dont want overlapped operation
        0// no templates file for COM port...
        );

    config.DCBlength = sizeof(config);

	if (pirate_com_handle == INVALID_HANDLE_VALUE)
	{
		printf("INVALID COM PORT!\n");
		return 0;
	}

    if((GetCommState(pirate_com_handle, &config) == 0))
    {
        printf("Get configuration port has a problem.\n");
		return 0;
    }

    config.BaudRate = baudrate;
    config.StopBits = ONESTOPBIT;
    config.Parity = PARITY_NONE; 
    config.ByteSize = DATABITS_8;
    config.fDtrControl = 0;
    config.fRtsControl = 0;

    if (!SetCommState(pirate_com_handle, &config))
    {
        printf( "Failed to Set Comm State Reason: %d\n",GetLastError());
		return 0;
    }
	printf("COM %d opened succesfully...", com_nb);
    printf("Current Settings Baud Rate %d\n", config.BaudRate);

	// set timeout
	cto.ReadTotalTimeoutConstant = 1;
	cto.ReadTotalTimeoutMultiplier = 100;
	SetCommTimeouts(pirate_com_handle, &cto);

	// flush
	pirate_flushrx_uart();

	return 1;

}

int pirate_flushrx_uart()
{
	unsigned char buffer_char[2];
	DWORD dwBytesRead;
	unsigned buffer_ind = 0;
	int r, i, x;

	buffer_char[0] = 0;
	buffer_ind = 0;

	dwBytesRead = 1;
	while (dwBytesRead != 0)
	{
		r = ReadFile(pirate_com_handle, buffer_char, 1, &dwBytesRead, NULL);
	}

	return 1;

}


int pirate_close_com()
{
	CloseHandle(pirate_com_handle);
	return 1;
}

int pirate_read_uart_packet(unsigned char *uart_command_rx, unsigned int length)
{
	int r;
	DWORD dwBytesRead;
	if (length > MAXCOM) return 0;
	r = ReadFile(pirate_com_handle, uart_command_rx, length, &dwBytesRead, NULL);
	return dwBytesRead;
}

int pirate_write_uart_packet(unsigned char *uart_command_tx, unsigned int length)
{
	int r;
	DWORD dwBytesWritten;
	if (length > MAXCOM) return 0;
	r = WriteFile(pirate_com_handle, uart_command_tx, length, &dwBytesWritten, NULL);
	return dwBytesWritten;
}
