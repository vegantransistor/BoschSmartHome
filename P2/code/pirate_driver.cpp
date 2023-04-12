// bus pirate driver

#include "stdlib.h"
#include "pirate_com.h"

#define COM 7
unsigned char TRXCOM[MAXCOM];


void print_buf(int l)
{
	int i;
	for (i = 0; i < l; i++)
		printf("%c", TRXCOM[i]);
	printf(" ");
}

void print_buf_hex(int l)
{
	int i;
	for (i = 0; i < l; i++)
		printf("%02X ", TRXCOM[i]);
	printf(" ");
}
unsigned int close_pirate()
{
	pirate_close_com();
	return 0;
}

unsigned int init_pirate()
{
	unsigned int x;
	unsigned char pirateinit1[1] = { 0x00 };
	unsigned char pirateinit2[20] = {
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00
	};
	unsigned char pirateinit3[1] = { 0x01 };
	unsigned char pirateinit4[3] = { 0x49, 0x64, 0x8A };
	unsigned char i1[2] = { 'b', '\n' };
	unsigned char i2[3] = { '1', '0', '\n' };
	unsigned char i3[2] = { '3', '\n' };
	unsigned char i4[2] = { ' ', '\n' };

	// first try in full speed (already in spi binary mode)
	pirate_init_com(COM, PIRATE_SPEED_BAUDRATE);

	// try to send reset, if already in binary mode, answer BBIO1
	x = pirate_write_uart_packet(pirateinit1, 1);
	x = pirate_read_uart_packet(TRXCOM, 5);

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
		x = pirate_read_uart_packet(TRXCOM, 5);
	}
	if (x != 5)
	{
		printf("PIRATE INIT ERROR!\n");
		getchar();
		return 0;
	}

	print_buf(5);

	// goto spi mode
	x = pirate_write_uart_packet(pirateinit3, 1);
	x = pirate_read_uart_packet(TRXCOM, 4);
	print_buf(4);

	// init spi mode
	x = pirate_write_uart_packet(pirateinit4, 3);
	x = pirate_read_uart_packet(TRXCOM, 3);
	print_buf_hex(3);
	printf("\n");

	return 1;
}

void set_aux_low()
{
	unsigned int x;

	unsigned char sal[1] = { 0x48 };

	x = pirate_write_uart_packet(sal, 1);
	x = pirate_read_uart_packet(TRXCOM, 1);
}

void set_aux_high()
{
	unsigned int x;

	unsigned char sah[1] = { 0x4A };

	x = pirate_write_uart_packet(sah, 1);
	x = pirate_read_uart_packet(TRXCOM, 1);
}
