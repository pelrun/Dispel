/* 65816.c
 * 65816/6502 module for DisPel
 * James Churchill
 * Created 230900
 * Last Modified 240900
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dispel.h"

/* disasm() - disassembles a single instruction
 * Pre:  mem   - pointer to memory for disassembly
 *       pos   - "address" of the instruction
 *       inst  - pointer to string buffer
 *       flag  - current processor state
 *       tsrc  - 1 if addresses/hex dump is to be suppressed.
 * Post: inst  - disassembled instruction
 *       returns number of bytes to advance, or 0 for error.
 */

int disasm(unsigned char *mem, unsigned long pos, unsigned char *flag, char *inst, unsigned char tsrc)
{
	// temp buffers to hold instruction,parameters and hex
	char ibuf[5],pbuf[20],hbuf[9];
	// variables to hold the instruction increment and signed params
	int offset,sval,i;

	// Parse out instruction mnemonic

	switch (mem[0])
	{
		// ADC
	case 0x69:
	case 0x6D:
	case 0x6F:
	case 0x65:
	case 0x72:
	case 0x67:
	case 0x7D:
	case 0x7F:
	case 0x79:
	case 0x75:
	case 0x61:
	case 0x71:
	case 0x77:
	case 0x63:
	case 0x73:
		strcpy(ibuf,"adc");
		break;
		// AND
	case 0x29:
	case 0x2D:
	case 0x2F:
	case 0x25:
	case 0x32:
	case 0x27:
	case 0x3D:
	case 0x3F:
	case 0x39:
	case 0x35:
	case 0x21:
	case 0x31:
	case 0x37:
	case 0x23:
	case 0x33:
		strcpy(ibuf,"and");
		break;
		// ASL
	case 0x0A:
	case 0x0E:
	case 0x06:
	case 0x1E:
	case 0x16:
		strcpy(ibuf,"asl");
		break;
		// BCC
	case 0x90:
		strcpy(ibuf,"bcc");
		break;
		// BCS
	case 0xB0:
		strcpy(ibuf,"bcs");
		break;
		// BEQ
	case 0xF0:
		strcpy(ibuf,"beq");
		break;
		// BNE
	case 0xD0:
		strcpy(ibuf,"bne");
		break;
		// BMI
	case 0x30:
		strcpy(ibuf,"bmi");
		break;
		// BPL
	case 0x10:
		strcpy(ibuf,"bpl");
		break;
		// BVC
	case 0x50:
		strcpy(ibuf,"bvc");
		break;
		// BVS
	case 0x70:
		strcpy(ibuf,"bvs");
		break;
		// BRA
	case 0x80:
		strcpy(ibuf,"bra");
		break;
		// BRL
	case 0x82:
		strcpy(ibuf,"brl");
		break;
		// BIT
	case 0x89:
	case 0x2C:
	case 0x24:
	case 0x3C:
	case 0x34:
		strcpy(ibuf,"bit");
		break;
		// BRK
	case 0x00:
		strcpy(ibuf,"brk");
		break;
		// CLC
	case 0x18:
		strcpy(ibuf,"clc");
		break;
		// CLD
	case 0xD8:
		strcpy(ibuf,"cld");
		break;
		// CLI
	case 0x58:
		strcpy(ibuf,"cli");
		break;
		// CLV
	case 0xB8:
		strcpy(ibuf,"clv");
		break;
		// SEC
	case 0x38:
		strcpy(ibuf,"sec");
		break;
		// SED
	case 0xF8:
		strcpy(ibuf,"sed");
		break;
		// SEI
	case 0x78:
		strcpy(ibuf,"sei");
		break;
		// CMP
	case 0xC9:
	case 0xCD:
	case 0xCF:
	case 0xC5:
	case 0xD2:
	case 0xC7:
	case 0xDD:
	case 0xDF:
	case 0xD9:
	case 0xD5:
	case 0xC1:
	case 0xD1:
	case 0xD7:
	case 0xC3:
	case 0xD3:
		strcpy(ibuf,"cmp");
		break;
		// COP
	case 0x02:
		strcpy(ibuf,"cop");
		break;
		// CPX
	case 0xE0:
	case 0xEC:
	case 0xE4:
		strcpy(ibuf,"cpx");
		break;
		// CPY
	case 0xC0:
	case 0xCC:
	case 0xC4:
		strcpy(ibuf,"cpy");
		break;
		// DEC
	case 0x3A:
	case 0xCE:
	case 0xC6:
	case 0xDE:
	case 0xD6:
		strcpy(ibuf,"dec");
		break;
		// DEX
	case 0xCA:
		strcpy(ibuf,"dex");
		break;
		// DEY
	case 0x88:
		strcpy(ibuf,"dey");
		break;
		// EOR
	case 0x49:
	case 0x4D:
	case 0x4F:
	case 0x45:
	case 0x52:
	case 0x47:
	case 0x5D:
	case 0x5F:
	case 0x59:
	case 0x55:
	case 0x41:
	case 0x51:
	case 0x57:
	case 0x43:
	case 0x53:
		strcpy(ibuf,"eor");
		break;
		// INC
	case 0x1A:
	case 0xEE:
	case 0xE6:
	case 0xFE:
	case 0xF6:
		strcpy(ibuf,"inc");
		break;
		// INX
	case 0xE8:
		strcpy(ibuf,"inx");
		break;
		// INY
	case 0xC8:
		strcpy(ibuf,"iny");
		break;
		// JMP
	case 0x4C:
	case 0x6C:
	case 0x7C:
	case 0x5C:
	case 0xDC:
		strcpy(ibuf,"jmp");
		break;
		// JSR
	case 0x22:
	case 0x20:
	case 0xFC:
		strcpy(ibuf,"jsr");
		break;
		// LDA
	case 0xA9:
	case 0xAD:
	case 0xAF:
	case 0xA5:
	case 0xB2:
	case 0xA7:
	case 0xBD:
	case 0xBF:
	case 0xB9:
	case 0xB5:
	case 0xA1:
	case 0xB1:
	case 0xB7:
	case 0xA3:
	case 0xB3:
		strcpy(ibuf,"lda");
		break;
		// LDX
	case 0xA2:
	case 0xAE:
	case 0xA6:
	case 0xBE:
	case 0xB6:
		strcpy(ibuf,"ldx");
		break;
		// LDY
	case 0xA0:
	case 0xAC:
	case 0xA4:
	case 0xBC:
	case 0xB4:
		strcpy(ibuf,"ldy");
		break;
		// LSR
	case 0x4A:
	case 0x4E:
	case 0x46:
	case 0x5E:
	case 0x56:
		strcpy(ibuf,"lsr");
		break;
		// MVN
	case 0x54:
		strcpy(ibuf,"mvn");
		break;
		// MVP
	case 0x44:
		strcpy(ibuf,"mvp");
		break;
		// NOP
	case 0xEA:
		strcpy(ibuf,"nop");
		break;
		// ORA
	case 0x09:
	case 0x0D:
	case 0x0F:
	case 0x05:
	case 0x12:
	case 0x07:
	case 0x1D:
	case 0x1F:
	case 0x19:
	case 0x15:
	case 0x01:
	case 0x11:
	case 0x17:
	case 0x03:
	case 0x13:
		strcpy(ibuf,"ora");
		break;
		// PEA
	case 0xF4:
		strcpy(ibuf,"pea");
		break;
		// PEI
	case 0xD4:
		strcpy(ibuf,"pei");
		break;
		// PER
	case 0x62:
		strcpy(ibuf,"per");
		break;
		// PHA
	case 0x48:
		strcpy(ibuf,"pha");
		break;
		// PHP
	case 0x08:
		strcpy(ibuf,"php");
		break;
		// PHX
	case 0xDA:
		strcpy(ibuf,"phx");
		break;
		// PHY
	case 0x5A:
		strcpy(ibuf,"phy");
		break;
		// PLA
	case 0x68:
		strcpy(ibuf,"pla");
		break;
		// PLP
	case 0x28:
		strcpy(ibuf,"plp");
		break;
		// PLX
	case 0xFA:
		strcpy(ibuf,"plx");
		break;
		// PLY
	case 0x7A:
		strcpy(ibuf,"ply");
		break;
		// PHB
	case 0x8B:
		strcpy(ibuf,"phb");
		break;
		// PHD
	case 0x0B:
		strcpy(ibuf,"phd");
		break;
		// PHK
	case 0x4B:
		strcpy(ibuf,"phk");
		break;
		// PLB
	case 0xAB:
		strcpy(ibuf,"plb");
		break;
		// PLD
	case 0x2B:
		strcpy(ibuf,"pld");
		break;
		// REP
	case 0xC2:
		strcpy(ibuf,"rep");
		break;
		// ROL
	case 0x2A:
	case 0x2E:
	case 0x26:
	case 0x3E:
	case 0x36:
		strcpy(ibuf,"rol");
		break;
		// ROR
	case 0x6A:
	case 0x6E:
	case 0x66:
	case 0x7E:
	case 0x76:
		strcpy(ibuf,"ror");
		break;
		// RTI
	case 0x40:
		strcpy(ibuf,"rti");
		if(tsrc&0x2)
			strcat(ibuf,"\n");
		break;
		// RTL
	case 0x6B:
		strcpy(ibuf,"rtl");
		if(tsrc&0x2)
			strcat(ibuf,"\n");
		break;
		// RTS
	case 0x60:
		strcpy(ibuf,"rts");
		if(tsrc&0x2)
			strcat(ibuf,"\n");
		break;
		// SBC
	case 0xE9:
	case 0xED:
	case 0xEF:
	case 0xE5:
	case 0xF2:
	case 0xE7:
	case 0xFD:
	case 0xFF:
	case 0xF9:
	case 0xF5:
	case 0xE1:
	case 0xF1:
	case 0xF7:
	case 0xE3:
	case 0xF3:
		strcpy(ibuf,"sbc");
		break;
		// SEP
	case 0xE2:
		strcpy(ibuf,"sep");
		break;
		// STA
	case 0x8D:
	case 0x8F:
	case 0x85:
	case 0x92:
	case 0x87:
	case 0x9D:
	case 0x9F:
	case 0x99:
	case 0x95:
	case 0x81:
	case 0x91:
	case 0x97:
	case 0x83:
	case 0x93:
		strcpy(ibuf,"sta");
		break;
		// STP
	case 0xDB:
		strcpy(ibuf,"stp");
		break;
		// STX
	case 0x8E:
	case 0x86:
	case 0x96:
		strcpy(ibuf,"stx");
		break;
		// STY
	case 0x8C:
	case 0x84:
	case 0x94:
		strcpy(ibuf,"sty");
		break;
		// STZ
	case 0x9C:
	case 0x64:
	case 0x9E:
	case 0x74:
		strcpy(ibuf,"stz");
		break;
		// TAX
	case 0xAA:
		strcpy(ibuf,"tax");
		break;
		// TAY
	case 0xA8:
		strcpy(ibuf,"tay");
		break;
		// TXA
	case 0x8A:
		strcpy(ibuf,"txa");
		break;
		// TYA
	case 0x98:
		strcpy(ibuf,"tya");
		break;
		// TSX
	case 0xBA:
		strcpy(ibuf,"tsx");
		break;
		// TXS
	case 0x9A:
		strcpy(ibuf,"txs");
		break;
		// TXY
	case 0x9B:
		strcpy(ibuf,"txy");
		break;
		// TYX
	case 0xBB:
		strcpy(ibuf,"tyx");
		break;
		// TCD
	case 0x5B:
		strcpy(ibuf,"tcd");
		break;
		// TDC
	case 0x7B:
		strcpy(ibuf,"tdc");
		break;
		// TCS
	case 0x1B:
		strcpy(ibuf,"tcs");
		break;
		// TSC
	case 0x3B:
		strcpy(ibuf,"tsc");
		break;
		// TRB
	case 0x1C:
	case 0x14:
		strcpy(ibuf,"trb");
		break;
		// TSB
	case 0x0C:
	case 0x04:
		strcpy(ibuf,"tsb");
		break;
		// WAI
	case 0xCB:
		strcpy(ibuf,"wai");
		break;
		// WDM
	case 0x42:
		strcpy(ibuf,"wdm");
		break;
		// XBA
	case 0xEB:
		strcpy(ibuf,"xba");
		break;
		// XCE
	case 0xFB:
		strcpy(ibuf,"xce");
		break;
	default:
		// Illegal
		printf("Unhandled instruction: %02X\n",mem[0]);
		exit(1);
	};

	// Parse out parameter list
	switch(mem[0]){
		// Absolute
	case 0x0C:
	case 0x0D:
	case 0x0E:
	case 0x1C:
	case 0x20:
	case 0x2C:
	case 0x2D:
	case 0x2E:
	case 0x4C:
	case 0x4D:
	case 0x4E:
	case 0x6D:
	case 0x6E:
	case 0x8C:
	case 0x8D:
	case 0x8E:
	case 0x9C:
	case 0xAC:
	case 0xAD:
	case 0xAE:
	case 0xCC:
	case 0xCD:
	case 0xCE:
	case 0xEC:
	case 0xED:
	case 0xEE:
		sprintf(pbuf,"$%04X",mem[1]+mem[2]*256);
		offset=3;
		break;
		// Absolute Indexed Indirect
	case 0x7C:
	case 0xFC:
		sprintf(pbuf,"($%04X,X)",mem[1]+mem[2]*256);
		offset=3;
		break;
		// Absolute Indexed, X
	case 0x1D:
	case 0x1E:
	case 0x3C:
	case 0x3D:
	case 0x3E:
	case 0x5D:
	case 0x5E:
	case 0x7D:
	case 0x7E:
	case 0x9D:
	case 0x9E:
	case 0xBC:
	case 0xBD:
	case 0xDD:
	case 0xDE:
	case 0xFD:
	case 0xFE:
		sprintf(pbuf,"$%04X,X",mem[1]+mem[2]*256);
		offset=3;
		break;
		// Absolute Indexed, Y
	case 0x19:
	case 0x39:
	case 0x59:
	case 0x79:
	case 0x99:
	case 0xB9:
	case 0xBE:
	case 0xD9:
	case 0xF9:
		sprintf(pbuf,"$%04X,Y",mem[1]+mem[2]*256);
		offset=3;
		break;
		// Absolute Indirect
	case 0x6C:
		sprintf(pbuf,"($%04X)",mem[1]+mem[2]*256);
		offset=3;
		break;
		// Absolute Indirect Long
	case 0xDC:
		sprintf(pbuf,"[$%04X]",mem[1]+mem[2]*256);
		offset=3;
		break;
		// Absolute Long
	case 0x0F:
	case 0x22:
	case 0x2F:
	case 0x4F:
	case 0x5C:
	case 0x6F:
	case 0x8F:
	case 0xAF:
	case 0xCF:
	case 0xEF:
		sprintf(pbuf,"$%06X",mem[1]+mem[2]*256+mem[3]*65536);
		offset=4;
		break;
		// Absolute Long Indexed, X
	case 0x1F:
	case 0x3F:
	case 0x5F:
	case 0x7F:
	case 0x9F:
	case 0xBF:
	case 0xDF:
	case 0xFF:
		sprintf(pbuf,"$%06X,X",mem[1]+mem[2]*256+mem[3]*65536);
		offset=4;
		break;
		// Accumulator
	case 0x0A:
	case 0x1A:
	case 0x2A:
	case 0x3A:
	case 0x4A:
	case 0x6A:
		sprintf(pbuf,"A");
		offset=1;
		break;
		// Block Move
	case 0x44:
	case 0x54:
		sprintf(pbuf,"$%02X,$%02X",mem[1],mem[2]);
		offset=3;
		break;
		// Direct Page
	case 0x04:
	case 0x05:
	case 0x06:
	case 0x14:
	case 0x24:
	case 0x25:
	case 0x26:
	case 0x45:
	case 0x46:
	case 0x64:
	case 0x65:
	case 0x66:
	case 0x84:
	case 0x85:
	case 0x86:
	case 0xA4:
	case 0xA5:
	case 0xA6:
	case 0xC4:
	case 0xC5:
	case 0xC6:
	case 0xE4:
	case 0xE5:
	case 0xE6:
		sprintf(pbuf,"$%02X",mem[1]);
		offset=2;
		break;
		// Direct Page Indexed, X
	case 0x15:
	case 0x16:
	case 0x34:
	case 0x35:
	case 0x36:
	case 0x55:
	case 0x56:
	case 0x74:
	case 0x75:
	case 0x76:
	case 0x94:
	case 0x95:
	case 0xB4:
	case 0xB5:
	case 0xD5:
	case 0xD6:
	case 0xF5:
	case 0xF6:
		sprintf(pbuf,"$%02X,X",mem[1]);
		offset=2;
		break;
		// Direct Page Indexed, Y
	case 0x96:
	case 0xB6:
		sprintf(pbuf,"$%02X,Y",mem[1]);
		offset=2;
		break;
		// Direct Page Indirect
	case 0x12:
	case 0x32:
	case 0x52:
	case 0x72:
	case 0x92:
	case 0xB2:
	case 0xD2:
	case 0xF2:
		sprintf(pbuf,"($%02X)",mem[1]);
		offset=2;
		break;
		// Direct Page Indirect Long
	case 0x07:
	case 0x27:
	case 0x47:
	case 0x67:
	case 0x87:
	case 0xA7:
	case 0xC7:
	case 0xE7:
		sprintf(pbuf,"[$%02X]",mem[1]);
		offset=2;
		break;
		// Direct Page Indexed Indirect, X
	case 0x01:
	case 0x21:
	case 0x41:
	case 0x61:
	case 0x81:
	case 0xA1:
	case 0xC1:
	case 0xE1:
		sprintf(pbuf,"($%02X,X)",mem[1]);
		offset=2;
		break;
		// Direct Page Indirect Indexed, Y
	case 0x11:
	case 0x31:
	case 0x51:
	case 0x71:
	case 0x91:
	case 0xB1:
	case 0xD1:
	case 0xF1:
		sprintf(pbuf,"($%02X),Y",mem[1]);
		offset=2;
		break;
		// Direct Page Indirect Long Indexed, Y
	case 0x17:
	case 0x37:
	case 0x57:
	case 0x77:
	case 0x97:
	case 0xB7:
	case 0xD7:
	case 0xF7:
		sprintf(pbuf,"[$%02X],Y",mem[1]);
		offset=2;
		break;
		// Stack (Pull)
	case 0x28:
	case 0x2B:
	case 0x68:
	case 0x7A:
	case 0xAB:
	case 0xFA:
		// Stack (Push)
	case 0x08:
	case 0x0B:
	case 0x48:
	case 0x4B:
	case 0x5A:
	case 0x8B:
	case 0xDA:
		// Stack (RTL)
	case 0x6B:
		// Stack (RTS)
	case 0x60:
		// Stack/RTI
	case 0x40:
		// Implied
	case 0x18:
	case 0x1B:
	case 0x38:
	case 0x3B:
	case 0x58:
	case 0x5B:
	case 0x78:
	case 0x7B:
	case 0x88:
	case 0x8A:
	case 0x98:
	case 0x9A:
	case 0x9B:
	case 0xA8:
	case 0xAA:
	case 0xB8:
	case 0xBA:
	case 0xBB:
	case 0xC8:
	case 0xCA:
	case 0xCB:
	case 0xD8:
	case 0xDB:
	case 0xE8:
	case 0xEA:
	case 0xEB:
	case 0xF8:
	case 0xFB:
		pbuf[0] = 0;
		offset = 1;
		break;
		// Program Counter Relative
	case 0x10:
	case 0x30:
	case 0x50:
	case 0x70:
	case 0x80:
	case 0x90:
	case 0xB0:
	case 0xD0:
	case 0xF0:
		// Calculate the signed value of the param
		sval = (mem[1]>127) ? (mem[1]-256) : mem[1];
		sprintf(pbuf, "$%04lX", (pos+sval+2) & 0xFFFF);
		offset = 2;
		break;
		// Stack (Program Counter Relative Long)
	case 0x62:
		// Program Counter Relative Long
	case 0x82:
		// Calculate the signed value of the param
		sval = mem[1] + mem[2]*256;
		sval = (sval>32767) ? (sval-65536) : sval;
		sprintf(pbuf, "$%04lX", (pos+sval+3) & 0xFFFF);
		offset = 3;
		break;
		// Stack Relative Indirect Indexed, Y
	case 0x13:
	case 0x33:
	case 0x53:
	case 0x73:
	case 0x93:
	case 0xB3:
	case 0xD3:
	case 0xF3:
		sprintf(pbuf, "($%02X,S),Y", mem[1]);
		offset = 2;
		break;
		// Stack (Absolute)
	case 0xF4:
		sprintf(pbuf, "$%04X", mem[1] + mem[2]*256);
		offset = 3;
		break;
		// Stack (Direct Page Indirect)
	case 0xD4:
		sprintf(pbuf,"($%02X)",mem[1]);
		offset = 2;
		break;
		offset = 3;
		break;
		// Stack Relative
	case 0x03:
	case 0x23:
	case 0x43:
	case 0x63:
	case 0x83:
	case 0xA3:
	case 0xC3:
	case 0xE3:
		sprintf(pbuf,"$%02X,S",mem[1]);
		offset = 2;
		break;
		// WDM mode
	case 0x42:
		// Stack/Interrupt
	case 0x00:
	case 0x02:
		sprintf(pbuf,"$%02X",mem[1]);
		offset = 2;
		break;
		// Immediate (Invariant)
	case 0xC2:
		// REP following
		*flag=*flag&~mem[1];
		sprintf(pbuf,"#$%02X",mem[1]);
		offset = 2;
		break;
	case 0xE2:
		// SEP following
		*flag = *flag|mem[1];
		sprintf(pbuf, "#$%02X", mem[1]);
		offset = 2;
		break;
		// Immediate (A size dependent)
	case 0x09:
	case 0x29:
	case 0x49:
	case 0x69:
	case 0x89:
	case 0xA9:
	case 0xC9:
	case 0xE9:
		if (*flag & 0x20)
		{
			sprintf(pbuf, "#$%02X", mem[1]);
			offset = 2;
		}
		else
		{
			sprintf(pbuf,"#$%04X",mem[1]+mem[2]*256);
			offset = 3;
		}
		break;
		// Immediate (X/Y size dependent)
	case 0xA0:
	case 0xA2:
	case 0xC0:
	case 0xE0:
		if ((*flag) & 0x10)
		{
			sprintf(pbuf,"#$%02X",mem[1]);
			offset = 2;
		}
		else
		{
			sprintf(pbuf,"#$%04X",mem[1]+mem[2]*256);
			offset = 3;
		}
		break;
	default:
		printf("Unhandled Addressing Mode: %02X\n",mem[0]);
		exit(1);
	};

	// Generate hex output
	for (i=0; i<offset; i++)
	{
		sprintf(hbuf+i*2,"%02X",mem[i]);
	}
	for (i=offset*2; i<8; i++)
	{
		hbuf[i]=0x20;
	}
	hbuf[8]=0;

	// Generate whole disassembly line
	if(!(tsrc & 1))
	{
		sprintf(inst, "%02lX/%04lX:\t%s\t%s %s", (pos >> 16) & 0xFF, pos&0xFFFF, hbuf, ibuf, pbuf);
	}
	else
	{
		sprintf(inst, "%s %s", ibuf, pbuf);
	}

	return offset;
}
