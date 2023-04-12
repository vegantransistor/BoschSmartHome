#ifndef PIRATE_TRX_H			/* prevent circular inclusions */
#define PIRATE_TRX_H			/* by using protection macros */

#define MAXCOM 1024
#define MAX_READ_BUF 64

#define TRX_SLEEP 0x00
#define TRX_IDLE 0x01
#define TRX_XOFF 0x02
#define TRX_VCOON_MC 0x03
#define TRX_REGON_MC 0x04
#define TRX_MANCAL 0x05
#define TRX_VCOON 0x06
#define TRX_REGON 0x07
#define TRX_STARTCAL 0x08
#define TRX_BWBOOST 0x09
#define TRX_FS_LOCK 0x0A
#define TRX_IFADCON 0x0B
#define TRX_ENDCAL 0x0C
#define TRX_RX 0x0D
#define TRX_RX_END 0x0E
#define TRX_RX_RST 0x0F
#define TRX_TXRX_SWITCH 0x10
#define TRX_RXFIFO_OVERFLOW 0x11
#define TRX_FSTXON 0x12
#define TRX_TX 0x13
#define TRX_TX_END 0x14
#define TRX_RXTX_SWITCH 0x15
#define TRX_TXFIFO_UNDERFLOW 0x16

//#define PIRATE_INIT_TRX_LENGTH 165
#define PIRATE_INIT_TRX_LENGTH 170

#define PIRATE_TRXTEST_LENGTH 4


void print_buf_hex(int l);
void print_buf(int l);

void read_rx_frame(unsigned char * buf, unsigned int l);
unsigned char read_1_byte();
void read_3_bytes(unsigned char * buf);
void read_7_bytes(unsigned char * buf);
void read_11_bytes(unsigned char * buf);
void read_15_bytes(unsigned char * buf);
void write_tx_fifo(unsigned char * txcmd, unsigned int l);
void write_tx_fifo_1_byte(unsigned char txcmd1);
void write_tx_fifo_7_bytes(unsigned char * txcmd7);
void write_tx_fifo_11_bytes(unsigned char * txcmd11);
void write_tx_fifo_15_bytes(unsigned char * txcmd15);

unsigned int read_rxbytes();
unsigned int read_txbytes();
unsigned char read_trx_state();
void enable_rx();
void set_variable_length_mode();
void set_fixed_length_mode();
void set_packet_length(unsigned char l);
void trxcal();
void enable_tx();
void disable_trx();
void reset_trx();
void flush_txfifo();
void flush_rxfifo();
void init_trx();
void tx_after_tx_mode();
void rx_after_tx_mode();
void set_868_3mhz();
void set_869_5mhz();

#endif