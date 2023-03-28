// simple 868 MHz smart home fuzzer

#include "stdafx.h"
#include "pirate_com.h"
#include "pirate_trx.h"
#include <time.h>
#include <stdlib.h>

// number of trials
#define NBTRY 1

// Experimental stuff for smoke detector working on another freuqnecy
//#define LONG_PREAMB
//#define PREAMB_T 1200
//#define F869_5 // default is 868,0MHz

// Mode: either SNIFFER or TXM_REPLAY
#define SNIFFER
//#define TXM_REPLAY

// enables RX and/or TX
#define RXAPP
#ifndef SNIFFER
#define TXAPP
#endif

// if true random is needed, otherwise deterministic start
//#define TRUERANDOM
#define RAND_N 0

// enables scrambling and descrambling functions
#define DESCRAMBLING
#define SCRAMBLING

// timeouts
#define TIMEOUT_TX 1.1

#ifndef SNIFFER
#define TIMEOUT_RX .05
#else
#define TIMEOUT_RX 1000000
#endif

#ifndef SNIFFER
#define RXSNIFFCNT 1
#else
#define RXSNIFFCNT 1000000
#endif

//#define TX_VERBOSE
#define RX_VERBOSE

// add some sleep time
//#define SLEEP_BETWEEN_TR
#define SLEEP_BT 5

// write some log in file
#define WRITE_LOG


#define TXM_MAX 63
unsigned char TXM[TXM_MAX];

#define MAXLOG 1024
unsigned char LOG_BYTES[MAXLOG];
unsigned int SW_SEQ[41] = { 
	3, 13, 18, 6, 14, 19, 16, 15, 
	11,	8, 11, 7, 10, 10, 2, 12, 
	18, 7, 21, 19, 5, 3, 16, 8, 
	22, 6, 10, 22, 2, 9, 6, 6, 
	10, 4, 17, 10, 20, 10, 17, 23, 
	20
};

FILE * F1, * F2;

#define RX_BUF_MAX_LENGTH 256
unsigned char RX_BUF[RX_BUF_MAX_LENGTH];

#define PIRATE_TRX_TX_L 256

unsigned char PIRATE_TRX_TX[PIRATE_TRX_TX_L] = {
	0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// specific scrambling function for the smart home device
int packet_scrambling(unsigned char * packet, unsigned int l)
{
	unsigned int i;
	// first byte length, min. 2 bytes payload
	if (l < 3) return -1;

	packet[1] = packet[1] ^ 0x76;
	packet[l - 1] = packet[l - 1] ^ packet[2];

		for (i = 2; i <= l - 2; i++)
			packet[i] = packet[i] ^ ((packet[i-1] + 0xDC) & 0xFF);
}

// specific descrambling function for the smart home device
int packet_descrambling(unsigned char * packet, unsigned int l)
{
	unsigned int i;
	unsigned char c,d;

	// first byte length, min. 2 bytes payload
	if (l < 3) return -1;

	c = packet[1];
	packet[1] = c ^ 0x76;

	for (i = 2; i <= l - 2; i++)
	{
		d = ((c + 0xDC) & 0xFF);
		c = packet[i];
		packet[i] = d ^ c;
	}

	packet[l - 1] = packet[l - 1] ^ packet[2];
}

// read log
unsigned int read_log()
{
	unsigned int i = 1;
	unsigned int c = 0;

	while (i == 1)
	{
		i = fscanf_s(F1, "%02X", LOG_BYTES+c);
		c++;
	}
	return c-1;
}

// read TXM file
unsigned int read_txm()
{
	unsigned int i = 1;
	unsigned int c = 0;

	while ((i == 1) && (c < TXM_MAX))
	{
		i = fscanf_s(F2, "%02X", TXM + c);
		c++;
	}
	return c-1;
}

int _tmain(int argc, _TCHAR* argv[])
{
	unsigned int x, i, j;
	unsigned int rx_packet_cnt, rx_frame_length;
	unsigned int log_length, txm_length, timeout;
	unsigned char s;

	clock_t start, end;
	double cpu_time_used;
	
	printf("Welcome to the SMART HOME FUZZER!\n");
	printf("*********************************\n");


#ifdef TRUERANDOM
	/* initialize random seed: */
	srand(time(NULL));
#endif

	unsigned char r = rand() % 256;      
	printf("\nINIT RNG = %02X\n", r);

	// init random
	for (i = 0; i < RAND_N; i++)
		x = rand();

	// txm handling
	fopen_s(&F2, "txm.txt", "r");
	txm_length = read_txm();
	printf("TXM length %d\n", txm_length);

#ifdef WRITE_LOG
	fopen_s(&F1, "log.txt", "w");
#else
	fopen_s(&F1, "log.txt", "r");
	log_length = read_log();
	printf("log length %d\n", log_length);
#endif

	init_pirate();
	init_trx();

#ifdef F869_5
	set_869_5mhz();
#endif

	printf("Press a key to start...");
	getchar();


#ifdef RXAPP
	enable_rx();
#endif


	// main loop
	for (j = 0; j < NBTRY; j++) 
	{
		printf(" >>> %i <<< ", j);
#ifdef WRITE_LOG
		fprintf(F1, " >>> %i <<< ", j);
#endif

#ifdef TXAPP
#ifdef TXM_REPLAY

		for (i = 0; i < txm_length;i++)
			PIRATE_TRX_TX[i] = TXM[i];

#else
		
		// ### BEGIN MUTATIONS
		
		
		// ### END MUTATIONS

#endif

#ifdef SCRAMBLING
		packet_scrambling(PIRATE_TRX_TX, PIRATE_TRX_TX[0]+1);
#endif

#ifdef LONG_PREAMB
			printf("!PRE");
			enable_tx();
			Sleep(PREAMB_T);
			printf("! ");
#endif

		// write data in tx fifo
		write_tx_fifo(PIRATE_TRX_TX, PIRATE_TRX_TX[0]+1);

		// read tx fifo bytes
		x = read_txbytes();
		printf("TX>%02d< ", x);

#ifdef WRITE_LOG
		fprintf(F1, "TX>%02d< ", x);
#endif

#ifdef TX_VERBOSE
		for (i = 0; i < PIRATE_TRX_TX[0] + 1; i++)
			printf("%02X ", PIRATE_TRX_TX[i]);
		printf(" --- ");
#endif

#ifdef WRITE_LOG
		for (i = 0; i < PIRATE_TRX_TX[0]+1; i++)
			fprintf(F1, "%02X ", PIRATE_TRX_TX[i]);
		fprintf(F1, " --- ");
#endif
		// send
		enable_tx();

		// first wait until tx fifo is empty
		timeout = 0;
		start = clock();
		x = read_txbytes();

		while ((x != 0) && (timeout == 0))
		{
			x = read_txbytes();
			end = clock();
			cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
			if (cpu_time_used > TIMEOUT_TX)
				timeout = 1; 
		}
		
		if (timeout == 1)
		{
			printf("TIMEOUT TX (1) ! ");
#ifdef WRITE_LOG
			fprintf(F1, "TIMEOUT TX (1) %d ! ",x);
#endif
			disable_trx();
			flush_txfifo();
#ifdef RXAPP
			enable_rx();
#endif
		}
#ifdef LONG_PREAMB
		disable_trx();
		flush_txfifo();
#ifdef RXAPP
		enable_rx();
#endif
#else
		// then wait until tx is finished (trx not in tx state any more)
		timeout = 0;
		start = clock();
		x = read_trx_state();

		while ((x == TRX_TX) && (timeout == 0))
		{
			x = read_trx_state();

			end = clock();
			cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
			if (cpu_time_used > TIMEOUT_TX)
				timeout = 1; 
		}

		if (timeout == 1)
		{
			printf("TIMEOUT TX (2) %02X ! ", x);
#ifdef WRITE_LOG
			fprintf(F1, "TIMEOUT TX (2) ! ");
#endif
			disable_trx();
			flush_txfifo();

#ifdef RXAPP
			enable_rx();
#endif
		}
#endif
		
		
#endif
		// tx state >>> rx state (automatic)
#ifdef RXAPP

		rx_packet_cnt = 0;
		
		while (rx_packet_cnt < RXSNIFFCNT)
		{
			timeout = 0;
			start = clock();
			x = read_rxbytes();

			while (x == 0)
			{
				end = clock();
				cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
				if (cpu_time_used > TIMEOUT_RX)
				{
					timeout = 1;
#ifdef WRITE_LOG
					fprintf(F1, "TIMEOUT RX! ");
#endif
					printf("TIMEOUT RX! ");
					disable_trx();
					flush_rxfifo();
					enable_rx();
					break;
				}
				x = read_rxbytes();
			}
			if (timeout == 0)
			{
				// read length (first byte)
				rx_frame_length = read_1_byte();
				RX_BUF[0] = rx_frame_length;

				printf("RX>%02d< ", rx_frame_length+1);
#ifdef WRITE_LOG
				fprintf(F1, "RX>%02d< ", rx_frame_length+1);
#endif

				// wait until complete frame is received
				while(read_rxbytes() < rx_frame_length); // ### TIMEOUT???

				// copy frame data from RX FIFO
				read_rx_frame(RX_BUF + 1, rx_frame_length);

#ifdef DESCRAMBLING
				packet_descrambling(RX_BUF, rx_frame_length+1);
#endif
				// log & display
				for (i = 0; i < rx_frame_length + 1; i++)
				{
					printf("%02X ", RX_BUF[i]);
#ifdef WRITE_LOG
					fprintf(F1, "%02X ", RX_BUF[i]);
#endif
				}
			}
			rx_packet_cnt++;

			// check for overflow
			s = read_trx_state();
			if (s == TRX_RXFIFO_OVERFLOW)
			{
				printf("RX OVERFLOW!\n");
#ifdef WRITE_LOG
				fprintf(F1, "RX OVERFLOW!\n");
#endif
				disable_trx();
				flush_rxfifo();
				enable_rx();
			}

			printf("\n");
#ifdef WRITE_LOG
			fprintf(F1, "\n");
#endif

		}
#endif
#ifdef SLEEP_BETWEEN_TR
		Sleep(SLEEP_BT);
		disable_trx();
		getchar();
		enable_rx();
#endif
	}
	disable_trx();

	pirate_close_com();
	fclose(F1);
	fclose(F2);
	printf("\nPress enter...\n");
	getchar();
	return 0;
}

