
#include <stdio.h>

#include "music.h"

static unsigned int pcfreq[62] =
{

	0,
	65,69,73,78,82,87,92,98,104,110,117,123,
	131,139,147,156,165,175,185,196,208,220,233,247,
	262,277,294,311,330,349,370,392,415,440,466,494,
	523,554,587,622,659,698,740,784,831,880,932,988,
	1047,1109,1175,1245,1319,1397,1480,1568,1661,1760,1864,1976,
	2094
};
static unsigned int adlibfreq[62] =
{
	0,
	2390,2411,2434,2456,2480,2506,2533,2562,2592,2625,2659,2695,
	3414,3435,3458,3480,3504,3530,3557,3586,3616,3649,3683,3719,
	4438,4459,4482,4504,4528,4554,4581,4610,4640,4673,4707,4743,
	5462,5483,5506,5528,5552,5578,5605,5634,5664,5697,5731,5767,
	6486,6507,6530,6552,6576,6602,6629,6658,6688,6721,6755,6791,
	7510
};

unsigned long musicstat = 0, musicount, countstop, countskip = 15, nownote;
unsigned int numnotes, drumstat, numchans = 1;

/*
void interrupt far ksmhandler(void);
void (interrupt far *old_handler)();
*/

void loadmusic(char *filename)
{
	printf("STUB: loadmusic(%s)\n", filename);
	return;
/*
	int infile, i, j;

	if (firstime == 1)
	{
		if (option[9] == 2)
		{
			midiscrap = 256;
			while ((midiscrap > 0) && ((inp(statport) & 0x40) != 0))
				midiscrap--;
			outp(statport,0x3f);
			midiscrap = 32000;
			while ((midiscrap > 0) && ((inp(dataport) & 0xfe) != 0))
				midiscrap--;
		}
		if (option[9]== 3)
		{
			if((infile=open("insts.dat",O_BINARY|O_RDONLY,S_IREAD))==-1)
				return(-1);
			for(i=0;i<256;i++)
			{
				read(infile,&buffer,33);
				for(j=0;j<20;j++)
					inst[i][j] = buffer[j];
				inst[i][20] = 0;
				for(j=21;j<32;j++)
					inst[i][j] = buffer[j-1];
			}
			close(infile);
			numchans = 9;
			outdata(0,0x1,0);  //clear test stuff
			outdata(0,0x4,0);  //reset
			outdata(0,0x8,0);  //2-operator synthesis
			firstime = 0;
		}
	}
	if((infile=open(filename,O_BINARY|O_RDONLY,S_IREAD))==-1)
		return(-1);
	read(infile,&trinst[0],16);
	read(infile,&trquant[0],16);
	read(infile,&trchan[0],16);
	read(infile,&trprio[0],16);
	read(infile,&trvol[0],16);
	read(infile,&numnotes,2);
	read(infile,&note[0],numnotes<<2);
	close(infile);
	countskip = 15;
	if ((trquant[0] == 3) || (trquant[1] == 6))
		countskip = 10;
	numchans = 9-trchan[11]*3;
	if (option[9] == 2)
		setmidiinsts();
	if (option[9] == 3)
	{
		if (trchan[11] == 0)
		{
			drumstat = 0;
			outdata(0,0xbd,drumstat);
		}
		if (trchan[11] == 1)
		{
			setinst(0,6,0,63-trvol[11],0xd6,0x68,0,0,10,0xd6,0x68,0,4); //bass
			setinst(0,7,0,63-trvol[12],0xd8,0x4f,0,0,63-trvol[14],0xf8,0xff,0,14); //snare & hihat
			setinst(0,8,0,63-trvol[15],0xf5,0xc8,0,0,63-trvol[13],0xd6,0x88,0,0);  //topsymb & tom
			outdata(0,0xa0+6,600&255);
			outdata(0,0xb0+6,(600>>8)&223);
			outdata(0,0xa0+7,400&255);
			outdata(0,0xb0+7,(400>>8)&223);
			outdata(0,0xa0+8,5510&255);
			outdata(0,0xb0+8,(5510>>8)&223);
			drumstat = 32;
			outdata(0,0xbd,drumstat);
		}
	}
*/
}

void musicon()
{
    printf("STUB: musicon()\n");
    return;
/*
	char ch;
	unsigned int num;
	int i, j, k, offs;
	unsigned char instbuf[11];

	for(i=0;i<numchans;i++)
	{
		chantrack[i] = 0;
		chanage[i] = 0;
	}
	j = 0;
	for(i=0;i<16;i++)
		if ((trchan[i] > 0) && (j < numchans))
		{
			k = trchan[i];
			while ((j < numchans) && (k > 0))
			{
				chantrack[j] = i;
				k--;
				j++;
			}
		}
	for(i=0;i<numchans;i++)
	{
		if (option[9] == 3)
		{
			for(j=0;j<11;j++)
				instbuf[j] = inst[trinst[chantrack[i]]][j+21];
			instbuf[1] = ((instbuf[1]&192)|(63-trvol[chantrack[i]]));
			setinst(0,i,instbuf[0],instbuf[1],instbuf[2],instbuf[3],instbuf[4],instbuf[5],instbuf[6],instbuf[7],instbuf[8],instbuf[9],instbuf[10]);
		}
		chanfreq[i] = 0;
	}
	k = 0;
	musicstat = 0;
	musicount = (note[k]>>12)-1;
	countstop = (note[k]>>12)-1;
	nownote = note[k];
	musicstat = 1;
	outp(0x43,0x36); outp(0x40,0); outp(0x40,0);
	if (countskip == 10)
	{
		outp(0x43,0x36); outp(0x40,255); outp(0x40,170);
	}
	old_handler = _dos_getvect(0x1c);
	_disable(); _dos_setvect(0x1c, ksmhandler); _enable();
*/
}

void musicoff()
{
    printf("STUB: musicoff()\n");
    return;
/*
	int i;

	outp(0x43,0x36); outp(0x40,0); outp(0x40,0);
	_disable(); _dos_setvect(0x1c, old_handler); _enable();
	if (option[9] == 1)
		outp(97, inp(97) & 0xfc);
	if (option[9] == 2)
		for(i=0;i<numchans;i++)
		{
			midiscrap = 256;
			while ((midiscrap > 0) && ((inp(statport) & 0x40) != 0))
				midiscrap--;
			if (midiscrap > 0)
			{
				if (i < 6)
					outp(dataport,0x90);
				else
					outp(dataport,0x91);
			}
			midiscrap = 256;
			while ((midiscrap > 0) && ((inp(statport) & 0x40) != 0))
				midiscrap--;
			if (midiscrap > 0)
				outp(dataport,chanfreq[i]+35);
			midiscrap = 256;
			while ((midiscrap > 0) && ((inp(statport) & 0x40) != 0))
				midiscrap--;
			if (midiscrap > 0)
				outp(dataport,0);
		}
	if (option[9] == 3)
		for(i=0;i<numchans;i++)
		{
			outdata(0,0xa0+i,0);
			outdata(0,0xb0+i,0);
		}
	musicstat = 0;
*/
}


void outdata(unsigned char synth, unsigned char index, unsigned char data)
{
    printf("STUB: outdata()\n");
    return;
/*
	if (synth == 1)
		_asm mov dx, 0x38a ; get the right 3812 address
	else
		_asm mov dx, 0x388 ; get the left 3812 address
	_asm \
	{
		mov ax, index    ; get the index value
		out dx, al       ; output to both chips
		in al, dx        ; slow down for the index to settle
		in al, dx
		in al, dx
		in al, dx
		in al, dx
		inc dx           ; move to the data register
		mov ax, data
		out dx, al       ; write the data out
		mov cx, 33       ; load the loop count
 lbl1:in al, dx
		loop lbl1
	}
*/
}

/*
void interrupt far ksmhandler()
{
	int i, j, quanter, bufnum, chan, drumnum, freq;
	long temp;

	musicount += countskip;
	if (musicount >= countstop)
	{
		bufnum = 0;
		while (musicount >= countstop)
		{
			if (option[9] == 1)
			{
				if ((((note[nownote]>>8)&15) == 0) && ((note[nownote]&64) > 0))
				{
					databuf[bufnum] = (unsigned char)(note[nownote]&63);
					bufnum++;
				}
			}
			if (option[9] > 1)
			{
				if (((note[nownote]&255) >= 1) && ((note[nownote]&255) <= 61))
				{
					i = 0;
					while (((chanfreq[i] != (note[nownote]&63)) || (chantrack[i] != ((note[nownote]>>8)&15))) && (i < numchans))
						i++;
					if (i < numchans)
					{
						if (option[9] == 2)
						{
							if (i < 6)
								databuf[bufnum] = (unsigned char)(0x90);
							else
								databuf[bufnum] = (unsigned char)(0x91);
							bufnum++;
							databuf[bufnum] = (unsigned char)(note[nownote]&63)+35; bufnum++;
							databuf[bufnum] = (unsigned char)0; bufnum++;
						}
						if (option[9] == 3)
						{
							databuf[bufnum] = (unsigned char)(0xa0+i); bufnum++;
							databuf[bufnum] = (unsigned char)(adlibfreq[note[nownote]&63]&255); bufnum++;
							databuf[bufnum] = (unsigned char)(0xb0+i); bufnum++;
							databuf[bufnum] = (unsigned char)((adlibfreq[note[nownote]&63]>>8)&223); bufnum++;
						}
						chanfreq[i] = 0;
						chanage[i] = 0;
					}
				}
				else if (((note[nownote]&255) >= 65) && ((note[nownote]&255) <= 125))
				{
					if (((note[nownote]>>8)&15) < 11)
					{
						temp = 0;
						i = numchans;
						for(j=0;j<numchans;j++)
							if ((countstop - chanage[j] >= temp) && (chantrack[j] == ((note[nownote]>>8)&15)))
							{
								temp = countstop - chanage[j];
								i = j;
							}
						if (i < numchans)
						{
							if (option[9] == 2)
							{
								if (i < 6)
									databuf[bufnum] = (unsigned char)(0x90);
								else
									databuf[bufnum] = (unsigned char)(0x91);
								bufnum++;
								databuf[bufnum] = (unsigned char)(note[nownote]&63)+35; bufnum++;
								databuf[bufnum] = (unsigned char)64; bufnum++;
							}
							if (option[9] == 3)
							{
								databuf[bufnum] = (unsigned char)(0xa0+i); bufnum++;
								databuf[bufnum] = (unsigned char)0; bufnum++;
								databuf[bufnum] = (unsigned char)(0xb0+i); bufnum++;
								databuf[bufnum] = (unsigned char)0; bufnum++;
								databuf[bufnum] = (unsigned char)(0xa0+i); bufnum++;
								databuf[bufnum] = (unsigned char)(adlibfreq[note[nownote]&63]&255); bufnum++;
								databuf[bufnum] = (unsigned char)(0xb0+i); bufnum++;
								databuf[bufnum] = (unsigned char)((adlibfreq[note[nownote]&63]>>8)|32); bufnum++;
							}
							chanfreq[i] = note[nownote]&63;
							chanage[i] = countstop;
						}
					}
					else
					{
						if (option[9] == 2)
						{
							databuf[bufnum] = (unsigned char)(0x92), bufnum++;
							switch((note[nownote]>>8)&15)
							{
								case 11: drumnum = 38; break;
								case 12: drumnum = 43; break;
								case 13: drumnum = 64; break;
								case 14: drumnum = 50; break;
								case 15: drumnum = 48; break;
							}
							databuf[bufnum] = (unsigned char)drumnum; bufnum++;
							databuf[bufnum] = (unsigned char)64; bufnum++;
						}
						if (option[9] == 3)
						{
							freq = adlibfreq[note[nownote]&63];
							switch((note[nownote]>>8)&15)
							{
								case 11: drumnum = 16; chan = 6; freq -= 2048; break;
								case 12: drumnum = 8; chan = 7; freq -= 2048; break;
								case 13: drumnum = 4; chan = 8; break;
								case 14: drumnum = 2; chan = 8; break;
								case 15: drumnum = 1; chan = 7; freq -= 2048; break;
							}
							databuf[bufnum] = (unsigned char)(0xa0+chan); bufnum++;
							databuf[bufnum] = (unsigned char)(freq&255); bufnum++;
							databuf[bufnum] = (unsigned char)(0xb0+chan); bufnum++;
							databuf[bufnum] = (unsigned char)((freq>>8)&223); bufnum++;
							databuf[bufnum] = (unsigned char)(0xbd); bufnum++;
							databuf[bufnum] = (unsigned char)(drumstat&(255-drumnum)); bufnum++;
							drumstat |= drumnum;
							databuf[bufnum] = (unsigned char)(0xbd); bufnum++;
							databuf[bufnum] = (unsigned char)(drumstat); bufnum++;
						}
					}
				}
			}
			nownote++;
			if (nownote >= numnotes)
			{
				nownote = 0;
				musicount = (note[nownote]>>12)-1;
			}
			quanter = (240/trquant[(note[nownote]>>8)&15]);
			countstop = (((note[nownote]>>12)+(quanter>>1)) / quanter) * quanter;
		}
		if (option[9] == 1)
		{
			j = 0;
			for(i=0;i<bufnum;i++)
				if (databuf[i] > j)
					j = databuf[i];
			if (j == 0)
				outp(97,inp(97)&0xfc);
			if (j > 0)
			{
				outp(0x43,0xb6);
				outp(0x42,(unsigned)(1193280L / pcfreq[j])&255);
				outp(0x42,(unsigned)(1193280L / pcfreq[j])>>8);
				outp(0x61,inp(0x61)|0x3);
			}
		}
		if (option[9] == 2)
		{
			while ((inp(0x331)&0x80) == 0)
				inp(0x330);
			for(i=0;i<bufnum;i++)
			{
				midiscrap = 256;
				while ((midiscrap > 0) && ((inp(statport) & 0x40) != 0))
					midiscrap--;
				if (midiscrap > 0)
					outp(dataport,databuf[i]);
			}
		}
		if (option[9] == 3)
			for(i=0;i<bufnum;i+=2)
				_asm \
				{
					mov dx, 0x388             ; get the left 3812 address
					mov bx, i
					mov ax, databuf[bx]       ; get the index value
					out dx, al                ; output to both chips
					in al, dx                 ; slow down for the index to settle
					in al, dx
					in al, dx
					in al, dx
					in al, dx
					inc dx                    ; move to the data register
					inc bx
					mov ax, databuf[bx]
					out dx, al                ; write the data out
					mov cx, 33                ; load the loop count
			 lbl2:in al, dx
					loop lbl2
				}
	}
	outp(0x20,0x20);
}
*/

void setinst(unsigned char synth, int chan, unsigned char v0, unsigned char v1,
             unsigned char v2, unsigned char v3, unsigned char v4,
             unsigned char v5, unsigned char v6, unsigned char v7,
             unsigned char v8, unsigned char v9, unsigned char v10)
{
    printf("STUB: setinst()\n");
    return;
/*
	int offs;

	outdata(synth,0xa0+chan,0);
	outdata(synth,0xb0+chan,0);
	outdata(synth,0xc0+chan,v10);
	if (chan == 0)
		offs = 0;
	if (chan == 1)
		offs = 1;
	if (chan == 2)
		offs = 2;
	if (chan == 3)
		offs = 8;
	if (chan == 4)
		offs = 9;
	if (chan == 5)
		offs = 10;
	if (chan == 6)
		offs = 16;
	if (chan == 7)
		offs = 17;
	if (chan == 8)
		offs = 18;
	outdata(synth,0x20+offs,v5);
	outdata(synth,0x40+offs,v6);
	outdata(synth,0x60+offs,v7);
	outdata(synth,0x80+offs,v8);
	outdata(synth,0xe0+offs,v9);
	offs+=3;
	outdata(synth,0x20+offs,v0);
	outdata(synth,0x40+offs,v1);
	outdata(synth,0x60+offs,v2);
	outdata(synth,0x80+offs,v3);
	outdata(synth,0xe0+offs,v4);
*/
}

void setmidiinsts()
{
    printf("STUB: setmidiinsts()\n");
    return;
/*
	midiscrap = 256;
	while ((midiscrap > 0) && ((inp(statport) & 0x40) != 0))
		midiscrap--;
	if (midiscrap > 0)
		outp(dataport,0xc0);
	midiscrap = 256;
	while ((midiscrap > 0) && ((inp(statport) & 0x40) != 0))
		midiscrap--;
	if (midiscrap > 0)
		outp(dataport,midiinst);
	midiscrap = 256;
	while ((midiscrap > 0) && ((inp(statport) & 0x40) != 0))
		midiscrap--;
	if (midiscrap > 0)
		outp(dataport,0xc1);
	midiscrap = 256;
	while ((midiscrap > 0) && ((inp(statport) & 0x40) != 0))
		midiscrap--;
	if (midiscrap > 0)
		outp(dataport,midiinst);
	midiscrap = 256;
	while ((midiscrap > 0) && ((inp(statport) & 0x40) != 0))
		midiscrap--;
	if (midiscrap > 0)
		outp(dataport,0xc2);
	midiscrap = 256;
	while ((midiscrap > 0) && ((inp(statport) & 0x40) != 0))
		midiscrap--;
	if (midiscrap > 0)
		outp(dataport,14);
*/
}
