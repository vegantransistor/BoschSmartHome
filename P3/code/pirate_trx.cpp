#include <stdio.h> 
#include <windows.h> 
#include <winbase.h>
#include <iostream>
#include <tchar.h>

#include "pirate_trx.h"
#include "pirate_com.h"

unsigned char TRXCOM[MAXCOM];

unsigned char PIRATE_INIT_TRX[PIRATE_INIT_TRX_LENGTH] = {

	0x02, 0x11, 0x00, 0x06, 0x03,
	0x02, 0x11, 0x01, 0x06, 0x03,
	0x02, 0x11, 0x02, 0x06, 0x03,
	0x02, 0x11, 0x03, 0x0D, 0x03,
	0x02, 0x11, 0x04, 0xE9, 0x03,
	0x02, 0x11, 0x05, 0xCA, 0x03,
	0x02, 0x11, 0x06, 0x40, 0x03, // MOD, packet length 
	0x02, 0x11, 0x07, 0x08, 0x03,
	0x02, 0x11, 0x08, 0x45, 0x03, // ORG
	0x02, 0x11, 0x0B, 0x06, 0x03,
	0x02, 0x11, 0x10, 0xC8, 0x03,
	0x02, 0x11, 0x11, 0x93, 0x03,
	0x02, 0x11, 0x12, 0x03, 0x03,
	0x02, 0x11, 0x15, 0x34, 0x03,
	0x02, 0x11, 0x17, 0x0F, 0x03, // ORG (Rx after Tx)
	0x02, 0x11, 0x18, 0x18, 0x03,
	0x02, 0x11, 0x19, 0x16, 0x03,
	0x02, 0x11, 0x1B, 0x43, 0x03,
	0x02, 0x11, 0x23, 0xE9, 0x03,
	0x02, 0x11, 0x24, 0x2A, 0x03,
	0x02, 0x11, 0x25, 0x00, 0x03,
	0x02, 0x11, 0x26, 0x1F, 0x03,
	0x02, 0x11, 0x2C, 0x81, 0x03,
	0x02, 0x11, 0x2D, 0x35, 0x03,
	0x02, 0x11, 0x2E, 0x09, 0x03,
	// FREQ
	0x02, 0x11, 0x0D, 0x21, 0x03,
	0x02, 0x11, 0x0E, 0x65, 0x03, // 868,3 MHz
	0x02, 0x11, 0x0F, 0x6A, 0x03, // 868,3 MHz
//	0x02, 0x11, 0x0E, 0x71, 0x03, // 869,5 MHz
//	0x02, 0x11, 0x0F, 0x7A, 0x03, // 869,5 MHz
	0x02, 0x11, 0x3E, 0xC0, 0x03,
	0x02, 0x10, 0x33, 0x03,
	0x02, 0x10, 0x3A, 0x03,
	0x02, 0x10, 0x3B, 0x03,
	0x02, 0x10, 0x34, 0x03,
	0x02, 0x10, 0x36, 0x03,
	0x02, 0x11, 0xF5, 0x00, 0x03
};


unsigned char PIRATE_TRXTEST[PIRATE_TRXTEST_LENGTH] = {
	0x02, 0x10, 0x35, 0x03
};



void write_tx_fifo_15_bytes(unsigned char * txcmd15)
{
	unsigned int x, i;
	unsigned char tx15[19] = {
		0x02, 0x1F, 0x7F,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00,
		0x03
	};
	for (i = 0; i < 15; i++)
	{
		tx15[i + 3] = txcmd15[i];
	}
	x = pirate_write_uart_packet(tx15, 19);
	x = pirate_read_uart_packet(TRXCOM, 19);
}

void write_tx_fifo_11_bytes(unsigned char * txcmd11)
{
	unsigned int x, i;
	unsigned char tx11[15] = {
		0x02, 0x1B, 0x7F,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00,
		0x03
	};
	for (i = 0; i < 11; i++)
	{
		tx11[i + 3] = txcmd11[i];
	}
	x = pirate_write_uart_packet(tx11, 15);
	x = pirate_read_uart_packet(TRXCOM, 15);
}

void write_tx_fifo_7_bytes(unsigned char * txcmd7)
{
	unsigned int x, i;
	unsigned char tx7[11] = {
		0x02, 0x17, 0x7F,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00,
		0x03
	};
	for (i = 0; i < 7; i++)
	{
		tx7[i + 3] = txcmd7[i];
	}
	x = pirate_write_uart_packet(tx7, 11);
	x = pirate_read_uart_packet(TRXCOM, 11);
}

void write_tx_fifo_1_byte(unsigned char txcmd1)
{
	unsigned int x, i;
	unsigned char tx1[5] = {
		0x02, 0x11, 0x7F, 0x00, 0x03
	};
	tx1[3] = txcmd1;
	x = pirate_write_uart_packet(tx1, 5);
	x = pirate_read_uart_packet(TRXCOM, 5);
}

void write_tx_fifo(unsigned char * txcmd, unsigned int l)
{
	unsigned int li = l;
	unsigned int written_bytes = 0;

	while (li >= 15)
	{
		write_tx_fifo_15_bytes(txcmd + written_bytes);
		li = li - 15;
		written_bytes = written_bytes + 15;
	}
	while (li >= 11)
	{
		write_tx_fifo_11_bytes(txcmd + written_bytes);
		li = li - 11;
		written_bytes = written_bytes + 11;
	}
	while (li >= 7)
	{
		write_tx_fifo_7_bytes(txcmd + written_bytes);
		li = li - 7;
		written_bytes = written_bytes + 7;
	}
	while (li > 0)
	{
		write_tx_fifo_1_byte(txcmd[written_bytes]);
		li = li - 1;
		written_bytes = written_bytes + 1;
	}
}


void read_15_bytes(unsigned char * buf)
{
	unsigned int x, i;

	unsigned char read15[19] = {
		0x02, 0x1F, 0xFF, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03
	};
	x = pirate_write_uart_packet(read15, 19);
	x = pirate_read_uart_packet(TRXCOM, 19);
	for (i = 0; i < 15; i++)
		buf[i] = TRXCOM[i + 3];
}

void read_11_bytes(unsigned char * buf)
{
	unsigned int x, i;

	unsigned char read11[15] = {
		0x02, 0x1B, 0xFF, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x03
	};
	x = pirate_write_uart_packet(read11, 15);
	x = pirate_read_uart_packet(TRXCOM, 15);
	for (i = 0; i < 11; i++)
		buf[i] = TRXCOM[i + 3];
}

void read_7_bytes(unsigned char * buf)
{
	unsigned int x, i;

	unsigned char read7[11] = {
		0x02, 0x17, 0xFF, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x03
	};
	x = pirate_write_uart_packet(read7, 11);
	x = pirate_read_uart_packet(TRXCOM, 11);
	for (i = 0; i < 7; i++)
		buf[i] = TRXCOM[i + 3];
}

void read_3_bytes(unsigned char * buf)
{
	unsigned int x, i;

	unsigned char read3[7] = {
		0x02, 0x13, 0xFF, 0x00, 0x00, 0x00, 0x03
	};
	x = pirate_write_uart_packet(read3, 7);
	x = pirate_read_uart_packet(TRXCOM, 7);
	for (i = 0; i < 3; i++)
		buf[i] = TRXCOM[i + 3];
}

unsigned char read_1_byte()
{
	unsigned int x, i;

	unsigned char read1[5] = { 0x02, 0x11, 0xBF, 0x00, 0x03 };
	x = pirate_write_uart_packet(read1, 5);
	x = pirate_read_uart_packet(TRXCOM, 5);
	return TRXCOM[3];
}

void read_rx_frame(unsigned char * buf, unsigned int l)
{

	unsigned int li = l;
	unsigned int read_bytes = 0;
	unsigned char c;

	while (li >= 15)
	{
		read_15_bytes(buf + read_bytes);
		li = li - 15;
		read_bytes += 15;
	}
	while (li >= 11)
	{
		read_11_bytes(buf + read_bytes);
		li = li - 11;
		read_bytes += 11;
	}
	while (li >= 7)
	{
		read_7_bytes(buf + read_bytes);
		li = li - 7;
		read_bytes += 7;
	}
	while (li > 0)
	{
		c = read_1_byte();
		buf[read_bytes] = c;
		li = li - 1;
		read_bytes += 1;
	}

}

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

unsigned int read_rxbytes()
{
	unsigned int x;

	unsigned char readrxbytes_o[5] = { 0x02, 0x11, 0xFB, 0x00, 0x03 };

	x = pirate_write_uart_packet(readrxbytes_o, 5);
	x = pirate_read_uart_packet(TRXCOM, 5);

	return TRXCOM[3];
}

unsigned int read_txbytes()
{
	unsigned int x;

	unsigned char readtxbytes_o[5] = { 0x02, 0x11, 0xFA, 0x00, 0x03 };

	x = pirate_write_uart_packet(readtxbytes_o, 5);
	x = pirate_read_uart_packet(TRXCOM, 5);

	return TRXCOM[3];
}

unsigned char read_trx_state()
{
	unsigned int x;

	unsigned char readrxbytes_o[5] = { 0x02, 0x11, 0xF5, 0x00, 0x03 };

	x = pirate_write_uart_packet(readrxbytes_o, 5);
	x = pirate_read_uart_packet(TRXCOM, 5);

	return TRXCOM[3];
}

void enable_rx()
{
	unsigned int x;

	unsigned char enablerx_o[4] = { 0x02, 0x10, 0x34, 0x03 };

	x = pirate_write_uart_packet(enablerx_o, 4);
	x = pirate_read_uart_packet(TRXCOM, 4);
}

void set_variable_length_mode()
{
	unsigned int x;

	unsigned char s_o[5] = { 0x02, 0x11, 0x08, 0x45, 0x03 };

	x = pirate_write_uart_packet(s_o, 5);
	x = pirate_read_uart_packet(TRXCOM, 5);
}

void set_fixed_length_mode()
{
	unsigned int x;

	unsigned char s_o[5] = { 0x02, 0x11, 0x08, 0x44, 0x03 };

	x = pirate_write_uart_packet(s_o, 5);
	x = pirate_read_uart_packet(TRXCOM, 5);
}

void set_packet_length(unsigned char l)
{
	unsigned int x;

	unsigned char s_o[5] = { 0x02, 0x11, 0x06, 0x40, 0x03 };

	s_o[3] = l;

	x = pirate_write_uart_packet(s_o, 5);
	x = pirate_read_uart_packet(TRXCOM, 5);
}

void trxcal()
{
	unsigned int x;

	unsigned char trxcal_o[4] = { 0x02, 0x10, 0x33, 0x03 };

	x = pirate_write_uart_packet(trxcal_o, 4);
	x = pirate_read_uart_packet(TRXCOM, 4);
}

void enable_tx()
{
	unsigned int x;

	unsigned char enablerx_o[4] = { 0x02, 0x10, 0x35, 0x03 };

	x = pirate_write_uart_packet(enablerx_o, 4);
	x = pirate_read_uart_packet(TRXCOM, 4);
}

void disable_trx()
{
	unsigned int x;

	unsigned char disabletrx_o[4] = { 0x02, 0x10, 0x36, 0x03 };

	x = pirate_write_uart_packet(disabletrx_o, 4);
	x = pirate_read_uart_packet(TRXCOM, 4);
}


void reset_trx()
{
	unsigned int x;

	unsigned char resettrx_o[4] = { 0x02, 0x10, 0x30, 0x03 };

	x = pirate_write_uart_packet(resettrx_o, 4);
	x = pirate_read_uart_packet(TRXCOM, 4);
}

void flush_txfifo()
{
	unsigned int x;

	unsigned char flush_txfifo_o[4] = { 0x02, 0x10, 0x3B, 0x03 };

	x = pirate_write_uart_packet(flush_txfifo_o, 4);
	x = pirate_read_uart_packet(TRXCOM, 4);
}

void flush_rxfifo()
{
	unsigned int x;

	unsigned char flush_rxfifo_o[4] = { 0x02, 0x10, 0x3A, 0x03 };

	x = pirate_write_uart_packet(flush_rxfifo_o, 4);
	x = pirate_read_uart_packet(TRXCOM, 4);
}

void init_trx()
{
	unsigned int x;
	// init trx
	reset_trx();
	x = pirate_write_uart_packet(PIRATE_INIT_TRX, PIRATE_INIT_TRX_LENGTH);
	x = pirate_read_uart_packet(TRXCOM, PIRATE_INIT_TRX_LENGTH);
	trxcal();
	disable_trx();
}

void tx_after_tx_mode()
{
	unsigned int x;

	unsigned char s_o[5] = { 0x02, 0x11, 0x17, 0x0E, 0x03 };

	x = pirate_write_uart_packet(s_o, 5);
	x = pirate_read_uart_packet(TRXCOM, 5);

}

void rx_after_tx_mode()
{
	unsigned int x;

	unsigned char s_o[5] = { 0x02, 0x11, 0x17, 0x0F, 0x03 };

	x = pirate_write_uart_packet(s_o, 5);
	x = pirate_read_uart_packet(TRXCOM, 5);

}

void set_868_3mhz()
{
	unsigned int x;

	unsigned char s_o1[5] = { 0x02, 0x11, 0x0E, 0x65, 0x03 };
	unsigned char s_o2[5] = { 0x02, 0x11, 0x0F, 0x6A, 0x03 };
	x = pirate_write_uart_packet(s_o1, 5);
	x = pirate_read_uart_packet(TRXCOM, 5);
	x = pirate_write_uart_packet(s_o2, 5);
	x = pirate_read_uart_packet(TRXCOM, 5);

}

void set_869_5mhz()
{
	unsigned int x;

	unsigned char s_o1[5] = { 0x02, 0x11, 0x0E, 0x71, 0x03 };
	unsigned char s_o2[5] = { 0x02, 0x11, 0x0F, 0x7A, 0x03 };
	x = pirate_write_uart_packet(s_o1, 5);
	x = pirate_read_uart_packet(TRXCOM, 5);
	x = pirate_write_uart_packet(s_o2, 5);
	x = pirate_read_uart_packet(TRXCOM, 5);

}
