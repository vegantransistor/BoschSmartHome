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
		port_name[7] = com_nb+0x30;
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
	if (length > MAXCOMLENGTH) return 0;
	r = ReadFile(pirate_com_handle, uart_command_rx, length, &dwBytesRead, NULL);
	return dwBytesRead;
}

int pirate_write_uart_packet(unsigned char *uart_command_tx, unsigned int length)
{
	int r;
	DWORD dwBytesWritten;
	if (length > MAXCOMLENGTH) return 0;
	r = WriteFile(pirate_com_handle, uart_command_tx, length, &dwBytesWritten, NULL);
	return dwBytesWritten;
}

unsigned int init_pirate()
{
	unsigned int x;
	unsigned int i;
	unsigned char pirateinit1[1] = { 0x00 };
	unsigned char pirateinit2[20] = {
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00
	};
	unsigned char pirateinit3[1] = { 0x01 };
	// SPI 2.6 MHz, UART 1 Mbit/s
	unsigned char pirateinit4[3] = { 0x49, 0x64, 0x8A };
	unsigned char i1[2] = { 'b', '\n' };
	unsigned char i2[3] = { '1', '0', '\n' };
	unsigned char i3[2] = { '3', '\n' };
	unsigned char i4[2] = { ' ', '\n' };
	unsigned char piratebuf[16];

	// first try in full speed (already in spi binary mode)
	pirate_init_com(COM, PIRATE_SPEED_BAUDRATE);

	// try to send reset, if already in binary mode, answer BBIO1
	x = pirate_write_uart_packet(pirateinit1, 1);
	x = pirate_read_uart_packet(piratebuf, 5);

	if (x != 5) // not in binary mode, go back to init speed and change speed before going into raw binary mode
	{
		// init com with initial baudrate
		pirate_close_com();
		pirate_init_com(COM, PIRATE_INIT_BAUDRATE);

		// first init pirate baudrate
		x = pirate_write_uart_packet(i1, 2);
		pirate_flushrx_uart();

		x = pirate_write_uart_packet(i2, 3);
		pirate_flushrx_uart();

		x = pirate_write_uart_packet(i3, 2);
		pirate_flushrx_uart();

		pirate_close_com();
		pirate_init_com(COM, PIRATE_SPEED_BAUDRATE);

		x = pirate_write_uart_packet(i4, 2);
		pirate_flushrx_uart();

		x = pirate_write_uart_packet(pirateinit2, 20);
		x = pirate_read_uart_packet(piratebuf, 5);
	}
	if (x != 5)
	{
		printf("PIRATE INIT ERROR!\n");
		getchar();
		return 0;
	}

	for (i = 0; i < 5; i++)
		printf("%c", piratebuf[i]);
	printf(" ");

	// goto spi mode
	x = pirate_write_uart_packet(pirateinit3, 1);
	x = pirate_read_uart_packet(piratebuf, 4);

	for (i = 0; i < 4; i++)
		printf("%c", piratebuf[i]);
	printf(" ");

	// init spi mode
	x = pirate_write_uart_packet(pirateinit4, 3);
	x = pirate_read_uart_packet(piratebuf, 3);

	for (i = 0; i < 3; i++)
		printf("%02X", piratebuf[i]);
	printf(" ");

	printf("\n");

	return 1;
}
