// EM-FI loop with RIGOL signal gen control and JTAG test via SEGGER J-Link

#include "stdafx.h"
#include "visa.h"
#include "rigol.h"
#include "pirate_driver.h"
#include "windows.h"

// halt if JTAG is open
#define HALT_ON_BINGO

// number of trials per delay
#define TRY_PER_DEL 5

// comment if no target is present
#define TARGET 

// comment if no bus pirate is present
#define BUSPIRATE

// infinite loop
#define INFINITE

// DELAY in ns
// Warning: Pulse gen reacts to FALLING EDGE, so substract pulse width 10000 ns

//#define INIT_DELAY 169000
#define INIT_DELAY 180000
#define DELAY_DELTA 10
//#define END_DELAY 174000
#define END_DELAY 183000

#define JTAG_LOG_LINE_NUMBER 41
#define JTAG_LOG_NB_OF_CHARACTERS 13
#define JTAG_LOG_MAX_NB_OF_CHARACTERS 1000

FILE * F1;


// this function checks the JTAG connection by reading the jlink log file and test some strings. Shall be adapted.
// returns 0  if JTAG is accessible, -1 otherwise

int check_connection()
{
	FILE* file;
	char tc[JTAG_LOG_MAX_NB_OF_CHARACTERS];
	unsigned int i;
	char jlink_error[JTAG_LOG_NB_OF_CHARACTERS] = { ' ', ' ', '*', '*', '*', '*', '*', ' ', 'E', 'r', 'r', 'o', 'r' };

	// open log file
	if (fopen_s(&file, "jlinklog.txt", "r") != 0)
	{
		printf("FILE ERROR!\n");
		return -1; // Return error code to indicate file open error
	}

	// go to line where error is indicated
	for (i = 0; i < JTAG_LOG_LINE_NUMBER; i++)
	{
		if (fgets(tc, JTAG_LOG_MAX_NB_OF_CHARACTERS, file) == NULL)
		{
			fclose(file);
			printf("ERROR: Unable to read from file!\n");
			return -1; // Return error code to indicate file read error
		}
	}

	fclose(file);

	// compare line
	if (strncmp(tc, jlink_error, JTAG_LOG_NB_OF_CHARACTERS) == 0)
	{
		printf("BINGO\n");
		return -1; // Return -1 to indicate error
	}
	else
	{
		return 0; // Return 0 to indicate success
	}
}

int main(int argc, char* argv[])
{
	unsigned int i, r;
	unsigned int trial, delay;

	// RIGOL generator sometimes does not apply configuration correctly, check before launching!
	printf("!!! WARNING! CHECK IF PULSE NEG IS 0V !!!\n");
	getchar();

#ifdef BUSPIRATE
	init_pirate();
	set_aux_high();
#endif

	rigol_open();
	rigol_out_on();
	rigol_set_delay_ns(INIT_DELAY);

#ifdef INFINITE
	while (1)
#endif
	{
		Beep(500, 500);
		for (delay = INIT_DELAY; delay < END_DELAY; delay += DELAY_DELTA)
		{
			rigol_set_delay_ns(delay);
			Sleep(100);

			for (trial = 0; trial < TRY_PER_DEL; trial++)
			{
				printf("T%d D%d\n", trial, delay);

#ifdef BUSPIRATE
				// reset
				Sleep(100);
				set_aux_high();
				Sleep(100);
#endif

#ifdef TARGET
				// launch jlink, try to connect
				system("jlink -device EFM32G210F128 -if SWD -speed 4000 -CommanderScript jlinkbatch.txt >> jtest.txt");

				// check
				r = check_connection();
				if (r == 0)
				{
#ifdef HALT_ON_BINGO
					Beep(1000, 100000);
					getchar();
#endif
				}
				system("del jtest.txt");
#endif

#ifdef BUSPIRATE
				set_aux_low();
#endif

			}
		}
	}

	rigol_out_off();
	rigol_close();

#ifdef BUSPIRATE
	set_aux_low();
	close_pirate();
#endif

	printf("Press any key...");
	getchar();
	return 0;
}
