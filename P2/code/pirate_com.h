#ifndef PIRATE_COM_H			/* prevent circular inclusions */
#define PIRATE_COM_H			/* by using protection macros */


#include <stdio.h> 
#include <windows.h> 
#include <winbase.h>
#include <iostream>
#include <tchar.h>

#define MAXCOM 1024
#define PIRATE_INIT_BAUDRATE 115200
#define PIRATE_SPEED_BAUDRATE 1000000

int pirate_init_com(int com_nb, int baudrate);
int pirate_close_com();
int pirate_flushrx_uart();
int pirate_write_uart_packet(unsigned char *uart_command_tx, unsigned int length);
int pirate_read_uart_packet(unsigned char *uart_command_rx, unsigned int length);

#endif