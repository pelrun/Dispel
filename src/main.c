/* main.c
 * DisPel 65816 Disassembler
 * James Churchill
 * Created 20000924
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __APPLE__
#include <sys/uio.h>
#endif
#ifdef _WIN32
#include <io.h>
#endif

#include "dispel.h"

void usage(void)
{
	printf("\nDisPel v1 by James Churchill/pelrun (C)2001-2011\n"
		"65816/SNES Disassembler\n"
		"Usage: dispel [-n] [-t] [-h] [-l] [-s] [-i] [-a] [-x] [-e] [-p]\n"
		"              [-b <bank>|-r <startaddr>-<endaddr>] [-g <origin>]\n"
		"              [-d <width>] [-o <outfile>] <infile>\n\n"
		"Options: (numbers are hex-only, no prefixes)\n"
		" -n                Skip $200 byte SMC header\n"
		" -t                Don't output addresses/hex dump.\n"
		" -h/-l             Force HiROM/LoROM memory mapping.\n"
		" -s/-i             Force enable/disable shadow ROM addresses (see readme.)\n"
		" -a                Start in 8-bit accumulator mode. Default is 16-bit.\n"
		" -x                Start in 8-bit X/Y mode. Default is 16-bit.\n"
		" -e                Turn off bank-boundary enforcement. (see readme.)\n"
		" -p                Split subroutines by placing blank lines after RTS,RTL,RTI\n"
		" -b <bank>         Disassemble bank <bank> only. Overrides -r.\n"
		" -r <start>-<end>  Disassemble block from <start> to <end>.\n"
		"                     Omit -<end> to disassemble to end of file.\n"
		" -g <origin>       Set origin of disassembled code (see readme.)\n"
		" -d <width>        No disassembly - produce a hexdump with <width> bytes/line.\n"
		" -o <outfile>      Set file to redirect output to. Default is stdout.\n"
		" <infile>          File to disassemble.\n");
}

/* Snes9x Hi/LoROM autodetect code */

int AllASCII(unsigned char *b, int size)
{
	int i;
	for (i = 0; i < size; i++)
	{
		if (b[i] < 32 || b[i] > 126)
		{
			return 0;
		}
	}
	return 1;
}

int ScoreHiROM(unsigned char *data)
{
	int score = 0;

	if ((data[0xFFDC] + data[0xFFDD]*256 + data[0xFFDE] + data[0xFFDF]*256) == 0xFFFF)
	{
		score += 2;
	}
	
	if (data[0xFFDA] == 0x33)
	{
		score += 2;
	}
	if ((data[0xFFD5] & 0xf) < 4)
	{
		score += 2;
	}
	if (!(data[0xFFFD] & 0x80))
	{
		score -= 4;
	}
	if ((1 << (data[0xFFD7] - 7)) > 48)
	{
		score -= 1;
	}
	if (!AllASCII(&data[0xFFB0], 6))
	{
		score -= 1;
	}
	if (!AllASCII(&data[0xFFC0], 20))
	{
		score -= 1;
	}
	
	return (score);
}

int ScoreLoROM(unsigned char *data)
{
	int score = 0;

	if ((data[0x7FDC] + data[0x7FDD]*256 + data[0x7FDE] + data[0x7FDF]*256) == 0xFFFF)
	{
		score += 2;
	}
	if (data[0x7FDA] == 0x33)
	{
		score += 2;
	}
	if ((data[0x7FD5] & 0xf) < 4)
	{
		score += 2;
	}
	if (!(data[0x7FFD] & 0x80))
	{
		score -= 4;
	}
	if ((1 << (data[0x7FD7] - 7)) > 48)
	{
		score -= 1;
	}
	if (!AllASCII(&data[0x7FB0], 6))
	{
		score -= 1;
	}
	if (!AllASCII(&data[0x7FC0], 20))
	{
		score -= 1;
	}

	return (score);
}

int hexdump(unsigned char *data,unsigned long pos,unsigned long rpos,
			unsigned long len,char *inst, unsigned char dwidth)
{
	int i;

	sprintf(inst, "%02lX/%04lX:\t", (pos >> 16) & 0xFF, pos & 0xFFFF);
	for(i=0; i<dwidth && i+rpos<len; i++)
	{
		sprintf(inst + i*2 + 9, "%02X", data[rpos+i]);
	}
	return dwidth;
}

int main(int argc, char *argv[])
{
	FILE *fin,*fout;
	char infile[BUFSIZ],outfile[BUFSIZ],inst[521];
	unsigned char dmem[4],flag=0,*data;
	unsigned long len,pos=0,origin=0x1000000,start=0,end=0,rpos;
	unsigned char opt,skip=0,hirom=2,shadow=2,bound=1,tsrc=0;
	unsigned int offset,bank=0x100,i,tmp,dwidth=0;
	int hiscore,loscore;

	outfile[0]=0;

	// Parse the commandline

	if (argc < 2)
	{
		usage();
		exit(1);
	}

	for (i=1; i<(argc-1); i++)
	{
		if (sscanf(argv[i], "-%c", &opt) == 0)
		{
			usage();
			exit(1);
		}
		
		switch(opt)
		{
		case 'n':
			skip = 1;
			break;
		case 't':
			tsrc |= 1;
			break;
		case 'h':
			hirom = 1;
			break;
		case 'l':
			hirom = 0;
			break;
		case 's':
			shadow = 1;
			break;
		case 'i':
			shadow = 0;
			break;
		case 'a':
			flag |= 0x20;
			break;
		case 'x':
			flag |= 0x10;
			break;
		case 'e':
			bound = 0;
			break;
		case 'p':
			tsrc |= 2;
			break;
		case 'd':
			i++;
			if ((sscanf(argv[i], "%2X", &dwidth) == 0) || dwidth==0)
			{
				usage();
				printf("\n-d requires a hex value between 01 and FF after it.\n");
				exit(1);
			}
			break;
		case 'b':
			i++;
			if (sscanf(argv[i], "%2X", &bank) == 0)
			{
				usage();
				printf("\n-b requires a 1-byte hex value after it.\n");
				exit(1);
			}
			break;
		case 'r':
			i++;
			if (sscanf(argv[i], "%6lX-%6lX", &start, &end) == 0)
			{
				usage();
				printf("\n-a requires at least one hex value after it.\n");
				exit(1);
			}
			break;
		case 'g':
			i++;
			if (sscanf(argv[i], "%6lX", &origin) == 0)
			{
				usage();
				printf("\n-r requires one hex value after it.\n");
				exit(1);
			}
			break;
		case 'o':
			i++;
			strcpy(outfile, argv[i]);
			break;
		default:
			usage();
			printf("\nUnknown option: -%c\n", opt);
			exit(1);
		}
	}

	// Get the input filename and open it
	strcpy(infile, argv[i]);
	fin = fopen(infile, "rb");
	if (!fin)
	{
		printf("Cannot open %s for reading.\n", infile);
		exit(1);
	}

	// Set up the output
	if (outfile[0] == 0)
	{
		strcpy(outfile,"STDOUT");
		fout = stdout;
	}
	else
	{
		fout = fopen(outfile, "w");
		if (!fout)
		{
			printf("Cannot open %s for writing.\n",outfile);
			exit(1);
		}
	}

	// Read the file into memory
#ifndef _WIN32
	fseek(fin, 0L, SEEK_END);
	len = ftell(fin);
	fseek(fin, 0L, SEEK_SET);
#else
	len = filelength(fileno(fin));
#endif

	// Make sure the image is big enough

	if (len < 0x8000 || (skip == 1 && len < 0x8200))
	{
		printf("This file looks too small to be a legitimate rom image.\n");
	}

	// Allocate mem for file. Extra 3 bytes to prevent segfault during memcpy
	if ((data = malloc(len+3)) == NULL)
	{
		printf("Cant alloc %ld bytes.\n", len+3);
		exit(1);
	}
	if (skip)
	{
		len -= 0x200;
		fread(data, 0x200, 1, fin);
	}
	
	fread(data, len, 1, fin);
	fclose(fin);

	// Autodetect the HiROM/LoROM state

	if (hirom==2)
	{
		hiscore = ScoreHiROM(data);
		loscore = ScoreLoROM(data);
		if (hiscore>loscore)
		{
//			fprintf(stderr,"Autodetected HiROM image.\n");
			hirom = 1;
		}
		else
		{
//			fprintf(stderr,"Autodetected LoROM image.\n");
			hirom = 0;
		}
	}

	// Unmangle the address options

	pos = start;

	// If shadow addresses given, convert to unshadowed and set shadow on.
	if ((bank == 0x100 && start & 0x800000) | (bank & 0x80))
	{
		shadow = 1;
	}
	
	// If HiROM addresses given, convert to normal and set hirom on.
	if ((bank == 0x100 && start & 0x400000) | (bank & 0x40))
	{
		hirom=1;
	}
	bank &= 0x13F;
	start &= 0x3FFFFF;
	end &= 0x3FFFFF;

	// Autodetect shadow

	if(shadow == 2)
	{
//		fprintf(stderr,"%02X\n",data[hirom?0xFFD5:0x7FD5]);
		if(data[hirom?0xFFD5:0x7FD5] & 0x30)
		{
			shadow=1;
		}
		else
		{
			shadow=0;
		}
	}

	// If the bank byte is set, apply it to the address range
	if (bank < 0x100)
	{
		if(hirom)
		{
			pos = bank << 16;
			start = pos;
			end = start | 0xFFFF;
		}
		else
		{
			pos = (bank << 16) + 0x8000;
			start = bank * 0x8000;
			end = start + 0x7FFF;
		}
	}
	else
	{
		if(!hirom)
		{
			// Convert the addresses to offsets
			if ((start & 0xFFFF) < 0x8000)
			{
				start += 0x8000;
			}
			pos = start;
			start = ((start >> 16) & 0xFF) * 0x8000 + (start & 0x7FFF);
			end = ((end >> 16) & 0xFF) * 0x8000 + (end & 0x7FFF);
		}
	}

	// If end isn't after start, set end to end-of-file.
	if(end <= start)
	{
		end = len-1;
	}

	// If new origin set, apply it.
	if (origin<0x1000000)
	{
		pos=origin;
	}

	// If shadow set, apply it
	if (shadow)
	{
		pos |= 0x800000;
	}

	// If hirom, apply the mapping
	if (hirom)
	{
		pos |= 0x400000;
	}

#ifdef _DEBUG
	fprintf(stderr,"Start: $%06X End: $%06X Pos: $%06X\n", start, end, pos);
	fprintf(stderr,"Input: %s\nOutput: %s\n", infile, outfile);
	if(shadow)
	{
		fprintf(stderr,"Autodetected FastROM.\n");
	}
	else
	{
		fprintf(stderr,"Autodetected SlowROM.\n");
	}
#endif

	// Begin disassembly

	rpos = start;

	while (rpos < len && rpos <= end)
	{
		// copy some data to the staging area
		memcpy(dmem, data+rpos, 4);

		// disassemble one instruction, or produce one line of hexdump
		if (dwidth == 0)
		{
			offset = disasm(dmem, pos, &flag, inst, tsrc);
		}
		else
		{
			offset = hexdump(data, pos, rpos, len, inst, dwidth);
		}

		// Check for a file/block overrun
		if ((rpos + offset) > len || (rpos + offset) > (end+1))
		{
			// print out remaining bytes and finish
			fprintf(fout,"%02lX/%04lX:\t", (pos >> 16) & 0xFF, pos & 0xFFFF);
			for (i=rpos; i<len && i<=end; i++)
			{
				fprintf(fout,"%02X", data[rpos]);
			}
			fprintf(fout,"\n");
			break;
		}

		// Check for a bank overrun
		if (bound && ((pos & 0xFFFF) + offset) > 0x10000)
		{
			// print out remaining bytes
			fprintf(fout, "%02lX/%04lX:\t",(pos >> 16) & 0xFF, pos & 0xFFFF);
			tmp = 0x10000 - (pos & 0xFFFF);
			for (i=0; i<tmp; i++)
			{
				fprintf(fout, "%02X", data[rpos+i]);
			}
			fprintf(fout, "\n");
			// Move to next bank
			if(!hirom)
			{
				pos = (pos & 0xFF0000) + 0x18000;
			}
			else
			{
				pos += tmp;
			}
			rpos += tmp;
			continue;
		}

		fprintf(fout, "%s\n", inst);
		
		// Move to next instruction
		if (!hirom && ((pos & 0xFFFF) + offset) > 0xFFFF)
		{
			pos = (pos & 0xFF0000) + 0x18000;
		}
		else
		{
			pos += offset;
		}
		rpos+=offset;
	}

	fclose(fout);

	return 0;
}

