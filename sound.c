
#include <stdio.h>

#include "sound.h"

static short sbinited = 0, sbport = 0x220, sbdma = 1;
static unsigned char inst[256][32], databuf[512];
static unsigned char chanfreq[18], chantrack[18];
static unsigned char trinst[16], trquant[16], trchan[16];
static unsigned char trprio[16], trvol[16];

int ksay(char *filename)
{
	printf("STUB: ksay(%s)\n", filename);
	return 0;
/*
	int infile;
	unsigned char tempchar;
	unsigned int register i, j;
	unsigned int leng;
	unsigned long addr;
	char *sbset;

	if ((infile = open(filename, O_BINARY, S_IREAD)) == -1) return(-1);
	read(infile, snd, 44);
	if (option[5] == 0)
	{
		leng = read(infile, snd, 32000);
		outp(97,inp(97) | 3);
		if (musicstat == 0)
			outp(0x21,1);
		for(j=0;j<leng-2;j++)
		{
			outp(0x43,144);
			outp(0x42,snd[j]>>2);
			for(i=0;i<option[2];i++);
			outp(0x43,144);
			outp(0x42,snd[j]>>2);
			for(i=0;i<option[2];i++);
		}
		if (musicstat == 0)
			outp(0x21,0);
		outp(97,inp(97) & 0xfc);
	}
	if (option[5] == 1)
	{
		leng = read(infile, snd, 32000);
		if (musicstat == 0)
			outp(0x21,1);
		for(j=0;j<leng-2;j++)
		{
			outp(559,snd[j]);
			for(i=0;i<option[2];i++);
		}
		if (musicstat == 0)
			outp(0x21,0);
	}
	if (option[5] == 2)
	{
		if (sbinited == 0)
		{
			sbinited = 1;

			sbset = getenv("BLASTER");
			i = 0;
			while (sbset[i] != 0)
			{
				switch(sbset[i])
				{
					case 'A': case 'a':
						i++;
						sbport = 0;
						while (((sbset[i] >= 48) && (sbset[i] <= 57)) ||
								 ((sbset[i] >= 'A') && (sbset[i] <= 'F')) ||
								 ((sbset[i] >= 'a') && (sbset[i] <= 'f')))
						{
							sbport <<= 4;
							if ((sbset[i] >= 48) && (sbset[i] <= 57)) sbport += (short)(sbset[i]-48);
							else if ((sbset[i] >= 'A') && (sbset[i] <= 'F')) sbport += (short)(sbset[i]-55);
							else if ((sbset[i] >= 'a') && (sbset[i] <= 'f')) sbport += (short)(sbset[i]-55-32);
							i++;
						}
						break;
					case 'D': case 'd':
						i++;
						if ((sbset[i] >= 48) && (sbset[i] <= 57))
							{ sbdma = (short)(sbset[i]-48); i++; }
						break;
					default: i++; break;
				}
			}
		}
		if (reset_dsp() == 0)
		{
			addr = (((long)FP_SEG(snd))<<4) + ((long)FP_OFF(snd));
			if ((addr&65535) > (32000^0xffff))
				{ leng = read(infile,&snd[32000],32000); addr += 32000; }
			else
				leng = read(infile,&snd[0],32000);
			_asm \
			{
				mov dx, sbport
				add dx, 0xc
sb1:        in al, dx
				test al, 128
				jnz short sb1
				mov al, 0xd1
				out dx, al
sb2:        in al, dx
				test al, 128
				jnz short sb2
				mov al, 0x40
				out dx, al
sb3:        in al, dx
				test al, 128
				jnz short sb3
				mov al, 145
				out dx, al
sb4:        in al, dx
				test al, 128
				jnz short sb4
				mov al, 0x14
				out dx, al
sb5:        in al, dx
				test al, 128
				jnz short sb5
				mov ax, word ptr leng[0]
				dec ax
				out dx, al
sb6:        in al, dx
				test al, 128
				jnz short sb6
				mov al, ah
				out dx, al

				mov bx, word ptr sbdma[0]
				mov al, bl
				add al, 4
				out 0xa, al
				xor al, al
				out 0xc, al
				mov al, bl
				add al, 0x48
				out 0xb, al
				mov dx, bx
				add dx, dx
				mov ax, word ptr addr[0]
				out dx, al
				mov al, ah
				out dx, al
				inc dx
				mov ax, word ptr leng[0]
				dec ax
				out dx, al
				mov al, ah
				out dx, al
				mov dl, byte ptr dmapagenum[bx]
				mov al, byte ptr addr[2]
				out dx, al
				mov al, bl
				out 0xa, al
			}
			//leng--;
			//while (inp(sbport+0xc)&128); outp(sbport+0xc,0xd1);
			//while (inp(sbport+0xc)&128); outp(sbport+0xc,0x40);
			//while (inp(sbport+0xc)&128); outp(sbport+0xc,145);
			//while (inp(sbport+0xc)&128); outp(sbport+0xc,0x14);
			//while (inp(sbport+0xc)&128); outp(sbport+0xc,leng&255);
			//while (inp(sbport+0xc)&128); outp(sbport+0xc,(leng>>8)&255);
			//outp(0xa,sbdma+4);
			//outp(0xc,0);
			//outp(0xb,sbdma+0x48);
			//outp(sbdma<<1,(int)(addr&255)); outp(sbdma<<1,(int)((addr>>8)&255));
			//outp((sbdma<<1)+1,leng&255); outp((sbdma<<1)+1,(leng>>8)&255);
			//outp(dmapagenum[sbdma],(int)((addr>>16)&255));
			//outp(0xa,sbdma);
		}
	}
	if (option[5] == 3)
	{
		addr = ((long)FP_SEG(snd)<<4) + (long)FP_OFF(snd);
		if (((addr&65535)^65535) < 32000)
		{
			leng = read(infile, &snd[32000], 32000);
			addr += 32000;
		}
		else
			leng = read(infile, &snd[0], 32000);
		outp(0x138b,0x36); outp(0x1388,132); outp(0x1388,0);
		outp(0xf8a,0xf9); outp(0xb8a,0xf9); outp(0xb8b,8);
		outp(0xa,0x5);
		outp(0x83,(int)((addr>>16)&255));
		outp(0xc,0);
		outp(0x2,(int)(addr&255)); outp(0x2,(int)((addr>>8)&255));
		outp(0x3,(leng-1)&255); outp(0x3,((leng-1)>>8)&255);
		outp(0xb,0x49);
		outp(0xa,0x1);
	}
	close(infile);
	return(0);
*/
}

int reset_dsp()
{
    printf("STUB: reset_dsp()\n");
    return 0;
/*
	_asm \
	{
		mov dx, sbport
		add dx, 6
 lb1: and dx, 0xfff7
		mov al, 1
		out dx, al
		mov cx, 128
 lab1:in al, dx
		dec cx
		jnz lab1
		xor al, al
		out dx, al
		mov cx, 128
 lab2:in al, dx
		dec cx
		jnz lab2
		or dx, 0xe
		mov cx, -1
 lb2: in al, dx
		test al, 128
		jnz lb3
		dec cx
		jnz lb2
		jmp lb1
 lb3:
	}
	if (inp(sbport+0xa) == 0xaa)
		return(0);
	else
		return(-1);
*/
}
