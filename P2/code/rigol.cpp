// control of the RIGOL Signal Generator

#include "rigol.h"
#include "stdafx.h"
#include "visa.h"
#include "string.h"

#define MAX_BUF_LENGTH 1000

ViSession defaultRM, instr;
ViStatus status;
char SendBuf[MAX_BUF_LENGTH];
char RecBuf[MAX_BUF_LENGTH];
ViUInt32 retCount;

void rigol_open()
{
	unsigned int i;

	ViString expr = "?*";
	ViPFindList findList = new unsigned long;
	ViPUInt32 retcnt = new unsigned long;
	ViChar instrDesc[MAX_BUF_LENGTH];
	char c_idn[] = "*IDN?";

	//open the VISA instrument 
	status = viOpenDefaultRM(&defaultRM);
	if (status < VI_SUCCESS)
		printf("NO VISA\n");
	else
		printf("SUCCESS\n");

	status = viFindRsrc(defaultRM, expr, findList, retcnt, instrDesc);

	status = viOpen(defaultRM, instrDesc, VI_NULL, VI_NULL, &instr);

	status = viWrite(instr, (unsigned char *)c_idn, strlen(c_idn), &retCount);

	status = viRead(instr, (unsigned char *)RecBuf, MAX_BUF_LENGTH, &retCount);
	for (i = 0; i < retCount; i++)
		printf("%c", RecBuf[i]);

}

void rigol_close()
{
	// close
	status = viClose(instr);
	status = viClose(defaultRM);
}

int rigol_set_delay_ns(unsigned long delay_ns)
{
	char c_del[] = ":SOUR1:BURS:TDEL 0000000000E-09";
	unsigned d = 0;
	unsigned int n = 0;
	unsigned int i;
	unsigned long i_delay = delay_ns;


	while (i_delay != 0)
	{	
		d = i_delay % 10;
		c_del[26 - n] = d + 48;
		i_delay = i_delay / 10;
		n++;
	}
	if (n == 0)
	{
		printf("0 not allowed!");
		return -1;
	}

//	printf("d: %s n: %d\n", c_del,n);
	status = viWrite(instr, (unsigned char *)c_del, strlen(c_del), &retCount);
	return 0;
}

void rigol_out_off()
{
	char c_outoff[] = ":OUTP1 OFF";
	status = viWrite(instr, (unsigned char *)c_outoff, strlen(c_outoff), &retCount);
}

void rigol_out_on()
{
	char c_outon[] = ":OUTP1 ON";
	status = viWrite(instr, (unsigned char *)c_outon, strlen(c_outon), &retCount);
}
