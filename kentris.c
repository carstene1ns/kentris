/*
KENTRIS source code

License for this code:
	* No commercial exploitation please
	* Do not remove my name or this message from the code or credits
	* You may distribute modified code/executables, but please make it clear
	  that it is modified

History:
	04/??/1990: Wrote original tetris game in QuickBasic.
	02/??/1991: Ported QB version to C. Added lots of features until 1993.
	02/28/1993: Last touch of KENTRIS.C until 1999.
	03/22/1999: Fixed 3D polygon sorting of the 'K'. Another 6-year gap occurs.
	08/14/2005: Updated my contact info in the code and released KENTRIS.C.

-Ken S. (Ken Silverman's official web site: http://advsys.net/ken)
*/

#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <ctype.h>
#include <io.h>
#include <fcntl.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <graph.h>
#include <time.h>
#include <float.h>
#include <math.h>
#include <string.h>
#include <process.h>
#include <dos.h>
#include <malloc.h>
#define dataport 0x330
#define statport 0x331

static char blockmade[7] = {-1,-1,-1,-1,-1,-1,-1};
static unsigned char huge *blockmap = NULL;
static int shape[7][4][6] =
{
	1,0,0,1,1,1,1,0,0,1,1,1,1,0,0,1,1,1,1,0,0,1,1,1,
	0,-1,0,1,0,2,-1,0,1,0,2,0,0,-1,0,1,0,2,-1,0,1,0,2,0,
	0,-1,1,-1,0,1,-1,0,1,0,1,1,0,-1,0,1,-1,1,-1,-1,-1,0,1,0,
	-1,-1,0,-1,0,1,-1,0,1,0,1,-1,0,-1,0,1,1,1,-1,1,-1,0,1,0,
	1,-1,1,0,0,1,-1,0,0,1,1,1,1,-1,1,0,0,1,-1,0,0,1,1,1,
	-1,-1,-1,0,0,1,-1,1,0,1,1,0,-1,-1,-1,0,0,1,-1,1,0,1,1,0,
	0,-1,1,0,0,1,-1,0,0,1,1,0,-1,0,0,-1,0,1,-1,0,1,0,0,-1
};
static int fit[7][4][5] =
{
	0,0,-1,-1,0,0,0,-1,-1,0,0,0,-1,-1,0,0,0,-1,-1,0,
	0,-1,-1,-1,0,0,0,0,0,1,0,-1,-1,-1,0,0,0,0,0,1,
	0,2,-1,-1,0,1,1,0,-1,1,0,0,-1,-1,1,0,0,0,-1,1,
	2,0,-1,-1,1,0,0,0,-1,1,0,0,-1,-1,0,0,1,1,-1,1,
	0,1,-1,-1,0,1,0,0,-1,1,0,1,-1,-1,0,1,0,0,-1,1,
	1,0,-1,-1,1,0,0,1,-1,1,1,0,-1,-1,1,0,0,1,-1,1,
	0,1,-1,-1,0,1,0,1,-1,1,1,0,-1,-1,1,0,0,0,-1,1
};
static unsigned char huge *snd = NULL;
static unsigned char newpos[20];
static short sbinited = 0, sbport = 0x220, sbdma = 1;
static char dmapagenum[8] = {0x87,0x83,0x81,0x82,0x8f,0x8b,0x89,0x8a};
static int option[11], optionum;
static int coltable[7] = {9,12,11,10,13,14,15};
static int tocolsvga[16] = {0,0,0,0,0,0,0,0,0,33,46,53,42,59,43,100};
static unsigned int stadd, dirstadd;
static int x[2], y[2], kind[2], rotate[2], loadedmusic;
static char board[10][21][2];
static int name[2], stage[2], lastpass[2] = {0,0}, numstops[2], graph;
static int score[2], oldscore[2], down[2], cheat[2], dead[2], piecenum[2], avg[2];
static char piece[512];
char hsname[20][15], hspass[20][15];
unsigned int hswin[20][20];
static char str[80], filenam[15];
static clock_t tim[2], tick;
static time_t tnow;
static unsigned char mask[10][8] =
{
	255,255,255,255,255,255,255,255,
	170,85,170,85,170,85,170,85,
	59,139,115,116,46,206,209,220,
	119,170,221,170,119,170,221,170,
	221,187,136,17,221,187,136,17,
	85,68,85,187,85,68,85,187,
	85,221,17,238,85,221,17,238,
	51,187,153,119,204,238,102,221,
	89,101,149,86,89,101,149,86,
	91,107,109,173,181,182,214,218
};
static int lmost[350], rmost[350];
static double lin[16][14] =
{
	4,4,-70,-25,-20,-70,25,-20,-70,25,20,-70,-25,20,
	4,5,-30,-25,-20,-70,0,-20,-70,0,20,-30,-25,20,
	4,6,-30,25,-20,-70,0,-20,-70,0,20,-30,25,20,
	4,7,-20,-25,-20,-20,25,-20,-20,25,20,-20,-25,20,
	4,9,-20,-25,-20,20,-25,-20,20,-25,20,-20,-25,20,
	4,10,-20,0,-20,20,0,-20,20,0,20,-20,0,20,
	4,11,-20,25,-20,20,25,-20,20,25,20,-20,25,20,
	4,12,30,-25,-20,30,25,-20,30,25,20,30,-25,20,
	4,13,30,-25,-20,70,25,-20,70,25,20,30,-25,20,
	4,14,70,-25,-20,70,25,-20,70,25,20,70,-25,20,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0
};
static double cosa1, cosa2, cosa3, sina1, sina2, sina3;
static double a1, a2, a3, xd, yd, zd;
static char buffer[640];
static double ang1, ang2, ang3, xdir, ydir, zdir;
static int numlines, xdim, ydim, cols;
static int leng, count, enoughmemory;
static unsigned int speed;
int midiscrap, midiinst = 0;
unsigned long musicstat = 0, musicount, countstop, countskip = 15, nownote;
unsigned int numnotes, speed, drumstat, numchans, firstime = 1;
static unsigned long huge *note = NULL, chanage[18];
static unsigned char inst[256][32], databuf[512];
static unsigned char chanfreq[18], chantrack[18];
static unsigned char trinst[16], trquant[16], trchan[16];
static unsigned char trprio[16], trvol[16];
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
void interrupt far ksmhandler(void);
void (interrupt far *old_handler)();
unsigned char backpal0, backpal1, backpal2, palchangecount;
signed char pal0vel, pal1vel, pal2vel;
int maxscanline;

quitgame()
{
	if (blockmap != NULL) hfree(blockmap);
	if (note != NULL) hfree(note);
	if (snd != NULL) hfree(snd);
	exit(0);
}
main(int argc,char **argv)
{
	int i;
	char *founddot;

	filenam[0] = 0;
	for (i=1;i<argc;i++)
	{
		if ((argv[i][0] == '?') || (argv[i][1] == '?'))
		{
			printf("\nKentris V4.3 Feb 1991-Aug 2005                Programmed by Kenneth Silverman\n\n");
			printf("KENTRIS [filename]\n");
			printf("Example: KENTRIS FOOTBALL loads Kentris with football.set & football.dat\n");
			quitgame();
		}
		else if (filenam[0] == 0)
			strcpy(filenam,argv[i]);
	}
	filenam[8] = 0;
	if ((founddot = strchr(filenam,'.')) != 0)
		filenam[founddot-filenam+1] = 0;
	if (strlen(filenam) == 0)
		strcpy(filenam,"kentris");
	time(&tnow);
	srand((unsigned)tnow);
	optionum = 11;
	if ((loadset() != 0) || (loaddat() != 0))
	{
		strcpy(filenam,"kentris");
		if (loadset() != 0)
		{
			printf("Cannot load .SET file.\n");
			if (loaddat() != 0)
				printf("Cannot load .DAT file.\n");
			quitgame();
		}
		if (loaddat() != 0)
		{
			printf("Cannot load .DAT file.\n");
			quitgame();
		}
	}
	speed = 65535;
	outp(0x40,0); outp(0x40,0);
	enoughmemory = 7;
	if ((blockmap = (unsigned char huge *)halloc(7168L,1)) == NULL)
		enoughmemory--;
	if ((note = (unsigned long huge *)halloc(8192L,4)) == NULL)
		{ option[9] = 0; enoughmemory-=2; }
	if ((snd = (unsigned char huge *)halloc(64000L,1)) == NULL)
		{ option[3] = 0; enoughmemory-=4; }
	introduction();
	while(1)
	{
		while (option[4] == 0)
		{
			if (getnames() == 0)
				introduction();
			else
				tetris();
		}
		while (option[4] == 1)
		{
			tetris();
			introduction();
		}
	}
}

tetris()
{
	static char pal[768];
	int i, j, k, l, pl, color, row[20];
	char ch, filename[80];

	graph = 0;
	if (option[10] == 0)
		i = setvideomodetseng();
	if ((i == 0) || (option[10] > 0))
	{
		graph = 1;
		if ((_setvideomode(_VRES16COLOR) == 0) || (option[10] > 1))
		{
			graph = 2;
			if ((_setvideomode(_ERESCOLOR) == 0) || (option[10] > 2))
			{
				graph = 3;
				if (_setvideomode(_HRESBW) == 0)
				{
					printf("\nPlay me when you get graphics!\n");
					quitgame();
				}
			}
		}
	}
	for(i=0;i<10;i++)
		for(k=0;k<2;k++)
		{
			for(j=0;j<20;j++)
				board[i][j][k]=0;
			board[i][20][k] = 1;
		}
	stadd = 640;
	dirstadd = 640;
	if (graph == 0) { stadd = 1024; dirstadd = 1024; }
	if (graph < 3)
	{
		outp(0x3d4,0x9);
		maxscanline = inp(0x3d5);
		outp(0x3d5,maxscanline+31);
	}
	if (option[7] > 0)
	{
		if (graph == 0)
		{
			while ((inp(0x3da) & 8) != 0);
			outp(0x3d4,0xd); outp(0x3d5,(stadd>>2)&255);
			outp(0x3d4,0xc); outp(0x3d5,(stadd>>2)>>8);
			while ((inp(0x3da) & 8) == 0);
			inp(0x3da); outp(0x3c0,0x33); outp(0x3c0,(stadd&3)>>1);
		}
		if (graph > 0)
		{
			while ((inp(0x3da) & 8) != 0);
			outp(0x3d4,0xd); outp(0x3d5,(stadd>>3)&255);
			outp(0x3d4,0xc); outp(0x3d5,(stadd>>3)>>8);
			while ((inp(0x3da) & 8) == 0);
			inp(0x3da); outp(0x3c0,0x33); outp(0x3c0,stadd&7);
		}
	}
	if (graph < 2)
	{
		if (graph == 0)
			rectfill(0,0,1023,769,8);
		if (graph == 1)
		{
			_setfillmask(mask[0]);
			_setcolor(8);
			_rectangle(_GFILLINTERIOR,0,0,639,479);
		}
		backpal0 = 0;
		backpal1 = 0;
		backpal2 = 0;
		while (((backpal0+backpal1+backpal2)>>6) != 1)
		{
			backpal0 = (rand()%44)+10;
			backpal1 = (rand()%44)+10;
			backpal2 = (rand()%44)+10;
		}
		pal0vel = ((backpal0>>4)&2)-1;
		pal1vel = ((backpal1>>4)&2)-1;
		pal2vel = ((backpal2>>4)&2)-1;
		palchangecount = 0;
		outp(0x3c8,8);
		outp(0x3c9,backpal0);
		outp(0x3c9,backpal1);
		outp(0x3c9,backpal2);
	}
	if (graph > 1)
	{
		_setfillmask(mask[0]);
		i = (rand() % 6) + 1;
		_setcolor(i);
		if (graph == 2)
			_rectangle(_GFILLINTERIOR,0,0,639,350);
		j = (rand() % 6) + 1;
		while (j == i)
			j = (rand() % 6) + 1;
		_setcolor(j);
		k = (rand() % 8) + 2;
		_setfillmask(mask[k]);
		if (graph == 2)
		{
			_rectangle(_GFILLINTERIOR,0,0,319,350);
			for(i=0;i<7;i++) buffer[i] = mask[k][i+1];
			buffer[7] = mask[k][0];
			buffer[8] = 0;
			_setfillmask(buffer);
			_rectangle(_GFILLINTERIOR,320,0,639,350);
		}
		if (graph == 3)
			_rectangle(_GFILLINTERIOR,0,0,639,200);
	}
	for(i=0;i<2;i++)
	{
		_setcolor(0);
		_setfillmask(mask[0]);
		if (graph == 0)
			rectfill(i*384+160,64,i*384+480,704,0);
		if (graph == 1)
			_rectangle(_GFILLINTERIOR,i*240+100,40,i*240+300,440);
		if (graph == 2)
			_rectangle(_GFILLINTERIOR,i*240+100,29,i*240+300,321);
		if (graph == 3)
			_rectangle(_GFILLINTERIOR,i*240+100,17,i*240+300,183);
		_setcolor(3);
		if (graph == 0)
		{
			rectopen(i*384+159,63,i*384+481,705,3);
			rectopen(i*384+157,61,i*384+483,707,3);
		}
		if (graph == 1)
		{
			_rectangle(_GBORDER,i*240+99,39,i*240+301,441);
			_rectangle(_GBORDER,i*240+97,37,i*240+303,443);
		}
		if (graph == 2)
		{
			_rectangle(_GBORDER,i*240+99,28,i*240+301,322);
			_rectangle(_GBORDER,i*240+97,26,i*240+303,324);
		}
		if (graph == 3)
		{
			_rectangle(_GBORDER,i*240+99,16,i*240+301,184);
			_rectangle(_GBORDER,i*240+97,14,i*240+303,186);
		}
		_setcolor(7);
		if (graph == 0)
			rectopen(i*384+158,62,i*384+482,706,7);
		if (graph == 1)
			_rectangle(_GBORDER,i*240+98,38,i*240+302,442);
		if (graph == 2)
			_rectangle(_GBORDER,i*240+98,27,i*240+302,323);
		if (graph == 3)
			_rectangle(_GBORDER,i*240+98,15,i*240+302,185);
		dead[i] = 0;
		cheat[i] = 0;
		tim[i] = clock();
		down[i] = 0;
		score[i] = 0;
		oldscore[i] = 0;
		piecenum[i] = 0;
		stage[i] = 0;
		numstops[i] = 0;
		if (graph == 0)
			outstring(45,i*48+36,12,"Score: 0");
		if (graph > 0)
		{
			_settextcolor(4);
			if (graph == 1)
				_settextposition(29,i*30+22);
			if (graph > 1)
				_settextposition(25,i*30+22);
			_outtext("Score: 0");
		}
		if (option[4] == 0)
		{
			if (graph == 0)
			{
				sprintf(buffer,"%s",hsname[name[1-i]]);
				outstring(1,i*48+36,14,&buffer);
			}
			if (graph > 0)
			{
				_settextcolor(14);
				_settextposition(1,i*30+22);
				sprintf(buffer,"%s",hsname[name[1-i]]);
				_outtext(buffer);
			}
		}
	}
	if (option[0] == 1)
		for(i=0;i<500;i++)
			piece[i] = rand() % 7;
	if (option[3] > 0)
		ksay("begin.wav");
	if ((option[9] > 0) && ((enoughmemory&4) > 0))
		loadmusic("game.ksm");
	if (graph < 3)
		for(i=30;i>=0;i--)
		{
			while((inp(0x3da) & 8) != 0);
			while((inp(0x3da) & 8) == 0);
			if (graph < 3)
				outp(0x3d4,0x9); outp(0x3d5,maxscanline+i);
		}
	for(pl=0;pl<2;pl++)
		newblock(pl);
	ch = 0;
	if ((option[9] > 0) && ((enoughmemory&4) > 0))
		musicon();
	speed = 65535;
	outp(0x40,0); outp(0x40,0);
	while((ch != 27) && (((option[4] == 0) && (dead[0] == 0) && (dead[1] == 0)) || ((option[4] == 1) && ((dead[0] == 0) || (dead[1] == 0)))))
	{
		while (kbhit())
		{
			ch = getch();
			if (ch == 0)
			{
				ch = getch();
				if ((ch != 75) && (ch != 77) && (ch != 72) && (ch != 80))
					ch = 0;
				if (ch == 75)
					ch = 'J';
				if (ch == 77)
					ch = 'L';
				if (ch == 72)
					ch = 'I';
				if (ch == 80)
					ch = 'K';
			}
			ch = toupper(ch);
			if (((ch == 'A') || (ch == 'S') || (ch == 'D') || (ch == 'W') || (ch == 'J') || (ch == 'L') || (ch == 'K') || (ch == 'I')) && (dead[0] == 0) && (stage[0] == 0))
			{
				down[0] = 0;
				drawblock(0,0);
				if (((ch == 'J') || (ch == 'A')) && (check(0,x[0]-1,y[0]) == 0))
					x[0]--;
				if (((ch == 'L') || (ch == 'D')) && (check(0,x[0]+1,y[0]) == 0))
					x[0]++;
				if ((ch == 'K') || (ch == 'S'))
				{
					rotate[0] = (rotate[0] + 3) % 4;
					if (check(0,x[0],y[0]) == 1)
					rotate[0] = (rotate[0] + 1) % 4;
				}
				if ((ch == 'I') || (ch == 'W'))
				{
					rotate[0] = (rotate[0] + 1) % 4;
					if (check(0,x[0],y[0]) == 1)
					rotate[0] = (rotate[0] + 3) % 4;
				}
				drawblock(0,1);
				if ((check(0,x[0],y[0]+1) == 1) && (numstops[0] < 8))
				{
					tim[0] = clock();
					numstops[0]++;
				}
			}
			if (((ch == '4') || (ch == '6') || (ch == '5') || (ch == '8')) && (dead[1] == 0) && (stage[1] == 0))
			{
				down[1] = 0;
				drawblock(1,0);
				if ((ch == '4') && (check(1,x[1]-1,y[1]) == 0))
					x[1]--;
				if ((ch == '6') && (check(1,x[1]+1,y[1]) == 0))
					x[1]++;
				if (ch == '5')
				{
					rotate[1] = (rotate[1] + 3) % 4;
					if (check(1,x[1],y[1]) == 1)
					rotate[1] = (rotate[1] + 1) % 4;
				}
				if (ch == '8')
				{
					rotate[1] = (rotate[1] + 1) % 4;
					if (check(1,x[1],y[1]) == 1)
					rotate[1] = (rotate[1] + 3) % 4;
				}
				drawblock(1,1);
				if ((check(1,x[1],y[1]+1) == 1) && (numstops[1] < 8))
				{
					tim[1] = clock();
					numstops[1]++;
				}
			}
			if (ch == 32)
				down[0] = 1 - down[0];
			if (ch == '0')
				down[1] = 1 - down[1];
			if (ch == 59)
				cheat[0] = 1;
			if (ch == 43)
				cheat[1] = 1;
		}
		if ((stadd != dirstadd) && (tick != clock()) && (option[7] > 0))
		{
			if (dirstadd < stadd)
				stadd--;
			if (dirstadd > stadd)
				stadd++;
			if (graph == 0)
			{
				while ((inp(0x3da) & 8) != 0);
				outp(0x3d4,0xd); outp(0x3d5,(stadd>>2)&255);
				outp(0x3d4,0xc); outp(0x3d5,(stadd>>2)>>8);
				while ((inp(0x3da) & 8) == 0);
				inp(0x3da); outp(0x3c0,0x33); outp(0x3c0,(stadd&3)>>1);
			}
			if (graph > 0)
			{
				while ((inp(0x3da) & 8) != 0);
				outp(0x3d4,0xd); outp(0x3d5,(stadd>>3)&255);
				outp(0x3d4,0xc); outp(0x3d5,(stadd>>3)>>8);
				while ((inp(0x3da) & 8) == 0);
				inp(0x3da); outp(0x3c0,0x33); outp(0x3c0,stadd&7);
			}
		}
		if ((graph < 2) && (tick != clock()))
		{
			palchangecount++;
			if (palchangecount > 2)
				palchangecount = 0;
			if (palchangecount == 0)
			{
				backpal0 += pal0vel;
				if (backpal0 < 5)
					pal0vel = 1;
				if (backpal0 > 60)
					pal0vel = -1;
			}
			if (palchangecount == 1)
			{
				backpal1 += pal1vel;
				if (backpal1 < 5)
					pal1vel = 1;
				if (backpal1 > 60)
					pal1vel = -1;
			}
			if (palchangecount == 2)
			{
				backpal2 += pal2vel;
				if (backpal2 < 5)
					pal2vel = 1;
				if (backpal2 > 60)
					pal2vel = -1;
			}
			outp(0x3c8,8);
			outp(0x3c9,backpal0);
			outp(0x3c9,backpal1);
			outp(0x3c9,backpal2);
		}
		tick = clock();
		for(pl=0;pl<2;pl++)
			if ((dead[pl] == 0) && (clock() - tim[pl] > 500 - 490 * down[pl]))
			{
				speed-=12;
				outp(0x40,(int)(speed&255));
				outp(0x40,(int)((speed>>8)&255));
				l = 0;
				if ((l == 0) && (check(pl,x[pl],y[pl]+1) == 0) && (stage[pl] == 0))
				{
					l = 1;
					tim[pl] = clock();
					drawblock(pl,0);
					if ((option[4] == 0) && (stricmp(hsname[name[1-pl]],"computer") == 0) && (down[pl] == 0))
						computermove(pl);
					if ((option[4] == 1) && ((option[6] == 3) || (option[6] == 2-pl)) && (down[pl] == 0))
						computermove(pl);
					if (check(pl,x[pl],y[pl]+1) == 0)
						y[pl]++;
					drawblock(pl,1);
			  }
		if ((l == 0) && (check(pl,x[pl],y[pl]+1) == 1) && (stage[pl] == 0))
		{
			numstops[pl] = 0;
			l = 1;
			stage[pl] = 1;
			down[pl] = 0;
			board[x[pl]][y[pl]][pl] = coltable[kind[pl]];
			for(i=0;i<3;i++)
				board[x[pl]+shape[kind[pl]][rotate[pl]][i*2]][y[pl]+shape[kind[pl]][rotate[pl]][i*2+1]][pl] = coltable[kind[pl]];
			drawblock(pl,1);
			oldscore[pl] = score[pl];
			for(j=0;j<20;j++)
			{
				row[j]=0;
				k = 0;
				for(i=0;i<10;i++)
					if (board[i][j][pl] != 0)
						k++;
				if (k == 10)
				{
					row[j] = 1;
					score[pl]++;
				}
			}
			if (score[pl] > oldscore[pl])
				{
					for(k=0;k<20;k++)
						newpos[k] = k;
					for(k=0;k<20;k++)
						if (row[k] == 1)
						{
							for(j=k;j>0;j--)
							{
								newpos[j-1]++;
								for(i=0;i<10;i++)
									board[i][j][pl] = board[i][j-1][pl];
							}
							for(i=0;i<10;i++)
								board[i][0][pl] = 0;
						}
					for(j=0;j<20;j++)
						for(i=0;i<10;i++)
							if ((board[i][j][pl] != board[i][newpos[j]][pl]) || (row[j] == 1))
								block(pl*240+i*20+100,j*20+40,board[i][j][pl]);
					if ((score[pl] >= oldscore[pl] + 2) && (option[1] == 1) && dead[1-pl] == 0)
					{
						for(j=0;j<20-(score[pl]-oldscore[pl]);j++)
							for(i=0;i<10;i++)
								board[i][j][1-pl] = board[i][j+(score[pl]-oldscore[pl])][1-pl];
						for(j=20-(score[pl]-oldscore[pl]);j<20;j++)
						{
							for(i=0;i<10;i++)
								board[i][j][1-pl] = coltable[rand() % 7];
							for(i=0;i<(rand()%9)+1;i++)
								board[rand()%10][j][1-pl] = 0;
						}
						if (stage[1-pl] == 0)
							drawblock(1-pl,0);
						for(j=0;j<20-(score[pl]-oldscore[pl]);j++)
							for(i=0;i<10;i++)
								if (board[i][j][1-pl] != board[i][j-(score[pl]-oldscore[pl])][1-pl])
									block((1-pl)*240+i*20+100,j*20+40,board[i][j][1-pl]);
						for(j=20-(score[pl]-oldscore[pl]);j<20;j++)
							for(i=0;i<10;i++)
								block((1-pl)*240+i*20+100,j*20+40,board[i][j][1-pl]);
						if (stage[1-pl] == 0)
						{
							y[1-pl]-=(score[pl]-oldscore[pl]);
							if (y[1-pl] < 0)
							{
								if (graph < 2)
									drawblock(1-pl,0);
								y[1-pl] = 0;
							}
							drawblock(1-pl,1);
						}
					}
					if (graph == 0)
					{
						sprintf(buffer,"%d",score[pl]);
						outstring(45,pl*48+43,12,buffer);
					}
					if (graph > 0)
					{
						_settextcolor(4);
						if (graph == 1)
							_settextposition(29,pl*30+29);
						if (graph > 1)
							_settextposition(25,pl*30+29);
						sprintf(buffer,"%d",score[pl]);
						_outtext(buffer);
					}
					if (option[3] == 2)
					{
						if (score[pl]-oldscore[pl] == 4)
							ksay("4row.wav");
						if (score[pl]-oldscore[pl] == 3)
							ksay("3row.wav");
						if (score[pl]-oldscore[pl] == 2)
							ksay("2row.wav");
						if (score[pl]-oldscore[pl] == 1)
							ksay("1row.wav");
					}
					if ((stage[1-pl] == 0) && (check(1-pl,x[1-pl],y[1-pl]) == 1) && (dead[1-pl] == 0))
						youlose(1-pl);
				}
				avg[pl] = 0;
				for(i=0;i<10;i++)
				{
					j = 0;
					while (board[i][j][pl] == 0)
						j++;
					avg[pl]+=j;
				}
				if ((option[8] == 0) || (score[pl] > oldscore[pl]))
				{
					if (graph > 0)
					{
						if (option[7] == 1)
							dirstadd=640+(avg[1]-avg[0]);
						if (option[7] == 2)
							dirstadd=640+(avg[0]-avg[1]);
						if (dirstadd < 544)
							dirstadd = 544;
						if (dirstadd > 737)
							dirstadd = 737;
					}
					if (graph == 0)
					{
						if (option[7] == 1)
							dirstadd=1024+(avg[1]-avg[0]);
						if (option[7] == 2)
							dirstadd=1024+(avg[0]-avg[1]);
						if (dirstadd < 868)
							dirstadd = 868;
						if (dirstadd > 1181)
							dirstadd = 1181;
					}
				}
			}
			if ((l == 0) && (stage[pl] == 1))
			{
				l = 1;
				newblock(pl);
				stage[pl] = 0;
			}
		}
	}
	speed = 65535;
	outp(0x40,0); outp(0x40,0);
	if ((option[9] > 0) && ((enoughmemory&4) > 0))
	{
		musicoff();
		loadmusic("gameover.ksm");
		musicon();
	}
	while ((ch != 13) && (ch != 27))
		ch = getch();
	if ((option[9] > 0) && ((enoughmemory&4) > 0))
		musicoff();
	if (graph < 2)
	{
		if (graph == 0)
		{
			outp(0x3c7,0);
			for(i=0;i<768;i++)
				pal[i] = inp(0x3c9);
			for(j=63;j>=0;j--)
			{
				outp(0x3c8,0);
				while ((inp(0x3da) & 8) != 0);
				while ((inp(0x3da) & 8) == 0);
				for(i=0;i<768;i++)
					outp(0x3c9,(pal[i]*j)>>6);
			}
		}
		if (graph == 1)
		{
			outp(0x3c7,0);
			for(i=0;i<48;i++)
				pal[i] = inp(0x3c9);
			for(j=63;j>=0;j--)
			{
				outp(0x3c8,0);
				while ((inp(0x3da) & 8) != 0);
				while ((inp(0x3da) & 8) == 0);
				for(i=0;i<48;i++)
					outp(0x3c9,(pal[i]*j)>>6);
			}
		}
	}
	if (graph == 2)
		for(i=0;i<32;i++)
		{
			while ((inp(0x3da) & 8) != 0);
			while ((inp(0x3da) & 8) == 0);
			outp(0x3d4,0x9); outp(0x3d5,maxscanline+i);
		}
	for(i=0;i<2;i++)
		if ((dead[i] != 0) && (option[4] == 0))
			 hswin[name[1-i]][name[i]]++;
}

check(pl,xx,yy)
int pl,xx,yy;
{
	int i, j;

	i = 0;
	if (board[xx][yy][pl] != 0)
		i = 1;
	for(j=0;j<3;j++)
		if ((board[xx+shape[kind[pl]][rotate[pl]][j*2]][yy+shape[kind[pl]][rotate[pl]][j*2+1]][pl] != 0) && (yy+shape[kind[pl]][rotate[pl]][j*2+1] >= 0))
			i = 1;
	if ((xx < 0) || (xx > 9))
		i = 1;
	for(j=0;j<3;j++)
		if ((xx+shape[kind[pl]][rotate[pl]][j*2] < 0) || (xx+shape[kind[pl]][rotate[pl]][j*2] > 9))
			i = 1;
	return(i);
}

newblock(pl)
int pl;
{
	x[pl] = (rand() % 2) + 4;
	y[pl] = 0;
	kind[pl] = rand() % 7;
	if (option[0] == 1)
	{
		kind[pl] = piece[piecenum[pl]];
		piecenum[pl] = (piecenum[pl] + 1) % 512;
	}
	if (cheat[pl] == 1)
	{
		kind[pl] = 1;
		cheat[pl] = 0;
	}
	rotate[pl] = rand() % 4;
	drawblock(pl,1);
	if (check(pl,x[pl],y[pl]) == 1)
		youlose(pl);
	if ((check(pl,x[pl],y[pl]) == 0) && (kind[pl] == 1) && (option[3] == 2))
		ksay("bone.wav");
	tim[pl] = clock();
}

youlose(pl)
int pl;
{
	dead[pl] = 1;
	avg[pl] = 0;
	_setfillmask(mask[(rand()&7) + 2]);
	_setcolor((rand()%6) + 1);
	if (graph == 0)
		rectfill(pl*384+160,64,pl*384+480,704,(rand()%6)+1);
	if (graph == 1)
		_rectangle(_GFILLINTERIOR,pl*240+100,40,pl*240+300,440);
	if (graph == 2)
		_rectangle(_GFILLINTERIOR,pl*240+100,29,pl*240+300,321);
	if (graph == 3)
		_rectangle(_GFILLINTERIOR,pl*240+100,17,pl*240+300,183);
	if ((option[3] > 0) && (option[4] == 0))
	{
		if ((option[9] > 0) && ((enoughmemory&4) > 0))
			musicoff();
		strcpy(buffer,hsname[name[pl]]);
		buffer[8] = 0;
		strcat(buffer,".wav");
		if (ksay(buffer) != 0)
			ksay("congrats.wav");
	}
}

drawblock(pl,color)
int pl,color;
{
	int i, xcoord, ycoord, clr;

	clr = coltable[kind[pl]];
	if (color == 0)
		clr = 0;
	xcoord = pl*240+x[pl]*20+100;
	ycoord = y[pl]*20+40;
	if (y[pl] >= 0)
		block(xcoord,ycoord,clr);
	for(i=0;i<3;i++)
	{
		xcoord = pl*240+x[pl]*20+shape[kind[pl]][rotate[pl]][i*2]*20+100;
		ycoord = y[pl]*20+shape[kind[pl]][rotate[pl]][i*2+1]*20+40;
		if (y[pl]+shape[kind[pl]][rotate[pl]][i*2+1] >= 0)
			block(xcoord,ycoord,clr);
	}
}

block(x1,y1,clr)
int x1,y1,clr;
{
	if (graph == 0)
	{
		x1*=1.6;
		y1*=1.6;
	}
	if (graph == 2)
		y1*=.73;
	if (graph == 3)
		y1*=.42;
	if (clr == 0)
	{
		_setcolor(0);
		_setfillmask(mask[0]);
		if (graph == 0)
			rectfill(x1+1,y1+1,x1+31,y1+31,0);
		if (graph == 1)
			_rectangle(_GFILLINTERIOR,x1+1,y1+1,x1+19,y1+19);
		if (graph == 2)
			_rectangle(_GFILLINTERIOR,x1+1,y1+1,x1+19,y1+14);
		if (graph == 3)
			_rectangle(_GFILLINTERIOR,x1+1,y1+1,x1+19,y1+7);
	}
	if ((clr > 0) && (blockmade[clr-9] == 0))
	{
		if (graph > 0)
			_putimage(x1+1,y1+1,&blockmap[(clr-9)<<10],_GPSET);
		if (graph == 0)
			putblock(x1+1,y1+1,x1+31,y1+31,clr-9);
	}
	if ((clr > 0) && (blockmade[clr-9] != 0))
	{
		if (graph == 0)
		{
			hlin(x1+5,x1+31,y1+31,tocolsvga[clr]+144);
			hlin(x1+4,x1+30,y1+30,tocolsvga[clr]+144);
			vlin(x1+31,y1+5,y1+30,tocolsvga[clr]+144);
			vlin(x1+30,y1+4,y1+29,tocolsvga[clr]+144);
			rectfill(x1+3,y1+3,x1+29,y1+29,tocolsvga[clr]+72);
			hlin(x1+1,x1+27,y1+1,tocolsvga[clr]);
			hlin(x1+2,x1+28,y1+2,tocolsvga[clr]);
			vlin(x1+1,y1+2,y1+27,tocolsvga[clr]);
			vlin(x1+2,y1+3,y1+28,tocolsvga[clr]);
			if ((enoughmemory&1) == 1)
				getblock(x1+1,y1+1,x1+31,y1+31,clr-9);
		}
		if (graph == 1)
		{
			_setcolor(clr - 8);
			_moveto(x1+19,y1+3);
			_lineto(x1+19,y1+19);
			_lineto(x1+3,y1+19);
			_setfillmask(mask[0]);
			_rectangle(_GFILLINTERIOR,x1+2,y1+2,x1+18,y1+18);
			_setcolor(clr);
			_setfillmask(mask[1]);
			_rectangle(_GFILLINTERIOR,x1+2,y1+2,x1+18,y1+18);
			_moveto(x1+17,y1+1);
			_lineto(x1+1,y1+1);
			_lineto(x1+1,y1+17);
			if ((enoughmemory&1) == 1)
				_getimage(x1+1,y1+1,x1+19,y1+19,&blockmap[(clr-9)<<10]);
		}
		if (graph == 2)
		{
			_setcolor(clr - 8);
			_moveto(x1+19,y1+3);
			_lineto(x1+19,y1+14);
			_lineto(x1+3,y1+14);
			_setfillmask(mask[0]);
			_rectangle(_GFILLINTERIOR,x1+2,y1+2,x1+18,y1+13);
			_setcolor(clr);
			_setfillmask(mask[1]);
			_rectangle(_GFILLINTERIOR,x1+2,y1+2,x1+18,y1+13);
			_moveto(x1+17,y1+1);
			_lineto(x1+1,y1+1);
			_lineto(x1+1,y1+12);
			if ((enoughmemory&1) == 1)
				_getimage(x1+1,y1+1,x1+19,y1+14,&blockmap[(clr-9)<<10]);
		}
		if (graph == 3)
		{
			_setcolor(1);
			_setfillmask(mask[0]);
			_rectangle(_GFILLINTERIOR,x1+2,y1+2,x1+18,y1+7);
			_setcolor(0);
			_setfillmask(mask[1]);
			_rectangle(_GFILLINTERIOR,x1+2,y1+2,x1+18,y1+7);
			if ((enoughmemory&1) == 1)
				_getimage(x1+1,y1+1,x1+19,y1+7,&blockmap[(clr-9)<<10]);
		}
		if ((enoughmemory&1) == 1)
			blockmade[clr-9] = 0;
	}
}

int ksay(char *filename)
{
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
}

options()
{
	char ch, highlighted;
	int i;

	firstime = 1;
	ch = 0;
	highlighted = 0;
	_setvideomode(_TEXTC80);
	_displaycursor(_GCURSOROFF);
	_settextcolor(6);
	_settextposition(5,10);
	_outtext("ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿");
	_settextposition(6,10);
	_outtext("³                     Kentris Options:                     ³");
	_settextposition(7,10);
	_outtext("ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄÄÄ´");
	_settextposition(8,10);
	_outtext("³ 1.  Same pieces to both sides (yes/no)        ³          ³");
	_settextposition(9,10);
	_outtext("³ 2.  Penalize other players (yes/no)           ³          ³");
	_settextposition(10,10);
	_outtext("³ 3.  Speech speed (0-9999)                     ³          ³");
	_settextposition(11,10);
	_outtext("³ 4.  Speech (none/some/all)                    ³          ³");
	_settextposition(12,10);
	_outtext("³ 5.  Mode of play (head-head/practice)         ³          ³");
	_settextposition(13,10);
	_outtext("³ 6.  Speech source (PC/Covox VMK2/SndBlst/PAS) ³          ³");
	_settextposition(14,10);
	_outtext("³ 7.  Computer controlled sides (L/R/Both/None) ³          ³");
	_settextposition(15,10);
	_outtext("³ 8.  Panning (off/mid/side)                    ³          ³");
	_settextposition(16,10);
	_outtext("³ 9.  When to pan (dropping piece/getting row)  ³          ³");
	_settextposition(17,10);
	_outtext("³ 10. Music (off/PC/MPU401 MIDI/Adlib)          ³          ³");
	_settextposition(18,10);
	_outtext("³ 11. Graphics (SVGA-Tseng ET4000/VGA/EGA/CGA)  ³          ³");
	_settextposition(19,10);
	_outtext("ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÄÄÙ");
	_settextposition(20,10);
	_outtext("   Up/Down - select, Left/Right - change, Enter/Esc - exit  ");
	showoptions(highlighted);
	while((ch != 27) && (ch != 13))
	{
		ch = getch();
		if ((ch >= 97) && (ch <= 122))
			ch -= 32;
		if ((ch >= 65) && (ch <= 90))
			ch -= 7;
		if ((ch >= 48) && (ch <= 57) && (highlighted == 2))
		{
			option[2] = option[2] % 1000;
			option[2] *= 10;
			option[2] += (ch - 48);
			showoptions(highlighted);
		}
		if (ch == 0)
		{
			ch = getch();
			if (ch == 72)
			{
				highlighted=(highlighted+optionum-1)%optionum;
				showoptions(highlighted);
			}
			if (ch == 80)
			{
				highlighted=(highlighted+optionum+1)%optionum;
				showoptions(highlighted);
			}
			if (ch == 75)
			{
				if (highlighted == 0)
					option[0] = 1 - option[0];
				if (highlighted == 1)
					option[1] = 1 - option[1];
				if ((highlighted == 2) && (option[2] > 0))
					option[2]--;
				if ((highlighted == 3) && (((enoughmemory&2) > 0) || (option[3] != 0)))
					option[3] = (option[3] + 2) % 3;
				if (highlighted == 4)
					option[4] = 1 - option[4];
				if (highlighted == 5)
					option[5] = (option[5] + 3) % 4;
				if (highlighted == 6)
					option[6] = (option[6] + 3) % 4;
				if (highlighted == 7)
					option[7] = (option[7] + 2) % 3;
				if (highlighted == 8)
					option[8] = 1 - option[8];
				if (highlighted == 9)
					option[9] = (option[9] + 3) % 4;
				if (highlighted == 10)
				{
					option[10] = (option[10] + 3) % 4;
					for(i=0;i<7;i++)
						blockmade[i] = -1;
				}
				showoptions(highlighted);
			}
			if (ch == 77)
			{
				if (highlighted == 0)
					option[0] = 1 - option[0];
				if (highlighted == 1)
					option[1] = 1 - option[1];
				if ((highlighted == 2) && (option[2] < 9999))
					option[2]++;
				if ((highlighted == 3) && (((enoughmemory&2) > 0) || (option[3] != 0)))
					option[3] = (option[3] + 1) % 3;
				if (highlighted == 4)
					option[4] = 1 - option[4];
				if (highlighted == 5)
					option[5] = (option[5] + 1) % 4;
				if (highlighted == 6)
					option[6] = (option[6] + 1) % 4;
				if (highlighted == 7)
					option[7] = (option[7] + 1) % 3;
				if (highlighted == 8)
					option[8] = 1 - option[8];
				if (highlighted == 9)
					option[9] = (option[9] + 1) % 4;
				if (highlighted == 10)
				{
					option[10] = (option[10] + 1) % 4;
					for(i=0;i<7;i++)
						blockmade[i] = -1;
				}
				showoptions(highlighted);
			}
		}
	}
}

showoptions(highlighted)
int highlighted;
{
	_settextposition(8,60); _settextcolor((highlighted==0)*8+6);
	if (option[0] == 0)
		_outtext("NO      ");
	if (option[0] == 1)
		_outtext("YES     ");
	_settextposition(9,60); _settextcolor((highlighted==1)*8+6);
	if (option[1] == 0)
		_outtext("NO      ");
	if (option[1] == 1)
		_outtext("YES     ");
	_settextposition(10,60); _settextcolor((highlighted==2)*8+6);
	_outtext("         "); _settextposition(10,60);
	sprintf(buffer,"%d",option[2]); _outtext(buffer);
	_settextposition(11,60); _settextcolor((highlighted==3)*8+6);
	if (option[3] == 0)
		_outtext("NONE     ");
	if (option[3] == 1)
		_outtext("SOME     ");
	if (option[3] == 2)
		_outtext("ALL      ");
	_settextposition(12,60); _settextcolor((highlighted==4)*8+6);
	if (option[4] == 0)
		_outtext("HEAD-HEAD");
	if (option[4] == 1)
		_outtext("PRACTICE ");
	_settextposition(13,60); _settextcolor((highlighted==5)*8+6);
	if (option[5] == 0)
		_outtext("PC       ");
	if (option[5] == 1)
		_outtext("COVOX VMK");
	if (option[5] == 2)
		_outtext("SNDBLAST ");
	if (option[5] == 3)
		_outtext("PROAUDIO ");
	_settextposition(14,60); _settextcolor((highlighted==6)*8+6);
	if (option[6] == 0)
		_outtext("NONE     ");
	if (option[6] == 1)
		_outtext("RIGHT    ");
	if (option[6] == 2)
		_outtext("LEFT     ");
	if (option[6] == 3)
		_outtext("BOTH     ");
	_settextposition(15,60); _settextcolor((highlighted==7)*8+6);
	if (option[7] == 0)
		_outtext("OFF      ");
	if (option[7] == 1)
		_outtext("MID      ");
	if (option[7] == 2)
		_outtext("SIDE     ");
	_settextposition(16,60); _settextcolor((highlighted==8)*8+6);
	if (option[8] == 0)
		_outtext("DROPPING ");
	if (option[8] == 1)
		_outtext("GET ROW  ");
	_settextposition(17,60); _settextcolor((highlighted==9)*8+6);
	if (option[9] == 0)
		_outtext("OFF      ");
	if (option[9] == 1)
		_outtext("PC       ");
	if (option[9] == 2)
		_outtext("MIDI     ");
	if (option[9] == 3)
		_outtext("ADLIB    ");
	_settextposition(18,60); _settextcolor((highlighted==10)*8+6);
	if (option[10] == 0)
		_outtext("SuperVGA ");
	if (option[10] == 1)
		_outtext("VGA      ");
	if (option[10] == 2)
		_outtext("EGA      ");
	if (option[10] == 3)
		_outtext("CGA      ");
}

loadset()
{
	FILE *infile;
	int i, j;
	char st[80];

	strcpy(st,filenam);
	strcat(st,".set");
	if ((infile = fopen(st, "r+")) == 0)
		return(-1);
	for (i=0;i<optionum;i++)
	{
		fgets(st,80,infile);
		sscanf(st,"%d",&option[i]);
	}
	for (i=0;i<20;i++)
	{
		fgets(hsname[i],80,infile);
		hsname[i][strlen(hsname[i])-1] = 0;
	}
	for (i=0;i<20;i++)
	{
		fgets(hspass[i],80,infile);
		for(j=0;j<14;j++)
			hspass[i][j] = hspass[i][j]+i+j-128;
	}
	fclose(infile);
	return(0);
}

saveset()
{
	FILE *infile;
	int i, j;
	char st[80];

	strcpy(st,filenam);
	strcat(st,".set");
	if ((infile = fopen(st, "w+")) == 0)
		return(-1);
	for(i=0;i<optionum;i++)
		fprintf(infile,"%d\n",option[i]);
	for (i=0;i<20;i++)
		fprintf(infile,"%s\n",hsname[i]);
	for (i=0;i<20;i++)
	{
		for (j=0;j<14;j++)
	 fprintf(infile,"%c",hspass[i][j]+128-i-j);
		fprintf(infile,"\n");
	}
	fclose(infile);
	return(0);
}

loaddat()
{
	int i, j, infile;
	char st[80];

	strcpy(st,filenam);
	strcat(st,".dat");
	if((infile=open(st,O_BINARY|O_RDWR,S_IREAD))==-1)
		return(-1);
	for (i=0;i<20;i++)
		for (j=0;j<20;j++)
	 read(infile,&hswin[i][j],2);
	close(infile);
	return(0);
}

savedat()
{
	int i, j, infile;
	char st[80];

	strcpy(st,filenam);
	strcat(st,".dat");
	if((infile=open(st,O_BINARY|O_RDWR,S_IWRITE))==-1)
		return(-1);
	for (i=0;i<20;i++)
		for (j=0;j<20;j++)
	 write(infile,&hswin[i][j],2);
	close(infile);
	return(0);
}

getnames()
{
	int i, j, pag, pass[2];
	char ch, st[80];

	_setvideomode(_TEXTC80);
	_displaycursor(_GCURSOROFF);
	pag = 0;
	_setvisualpage(pag);
	_setactivepage(pag);
	drawchart();
	drawinfo(1);
	pass[0] = 0;
	pass[1] = 0;
	ch = 0;
	if ((option[9] > 0) && ((enoughmemory&4) > 0))
	{
		loadmusic("chart.ksm");
		musicon();
	}
	while (((pass[0] == 0) || (pass[1] == 0)) && (ch != 27))
	{
		if (kbhit())
		{
			ch = getch();
			drawinfo(0);
			if (ch != 13)
				for(i=0;i<2;i++)
					lastpass[i] = 0;
			if (((ch == 'j') || (ch == 'J')) && (name[1] > 0))
				name[1]--;
			if (((ch == 'l') || (ch == 'L')) && (name[1] < 19))
				name[1]++;
			if ((ch == '4') && (name[0] > 0))
				name[0]--;
			if ((ch == '6') && (name[0] < 19))
				name[0]++;
			if (ch == 0)
			{
				ch = getch();
				if ((ch == 75) && (name[0] > 0))
					name[0]--;
				if ((ch == 77) && (name[0] < 19))
					name[0]++;
				if ((ch == 72) && (name[1] > 0))
					name[1]--;
				if ((ch == 80) && (name[1] < 19))
					name[1]++;
				ch = 0;
			}
			if ((ch == 'C') || (ch == 'c'))
			{
				if (stricmp(hsname[name[1]],"computer") == 0)
					strcpy(str,hspass[name[1]]);
				if (stricmp(hsname[name[1]],"computer") != 0)
				{
					_settextcolor(15);
					_settextposition(name[1]+4,2);
					sprintf(buffer,"%s",hsname[name[1]]);
					_outtext(buffer);
					_settextcolor(7);
					_settextposition(25,1);
					_outtext("Please type in the password of the highlighted name.                           ");
					password();
					if ((strcmp(str,hspass[name[1]]) != 0) && (option[3] > 0))
						ksay("wrong.wav");
				}
				if (strcmp(str,hspass[name[1]]) == 0)
				{
					_settextposition(25,1);
					_outtext("Please type in the new name to be set in this position.                        ");
					_settextposition(2,2);
					_outtext("              ");
					_settextposition(2,2);
					hsname[name[1]][0] = 14;
					cgets(hsname[name[1]]);
					strcpy(hsname[name[1]],&hsname[name[1]][2]);
					for (i=0;i<20;i++)
						hswin[i][name[1]] = 0;
					for (j=0;j<20;j++)
						hswin[name[1]][j] = 0;
					if (stricmp(hsname[name[1]],"computer") != 0)
					{
						_settextposition(25,1);
						_outtext("Please type in the new password to be set in this position.                    ");
						password();
						strcpy(hspass[name[1]],str);
					}
				}
				_setactivepage(pag = 1 - pag);
				drawchart();
				_setvisualpage(pag);
			}
			if (((ch == 'P') || (ch == 'p')) && (stricmp(hsname[name[1]],"computer") != 0))
			{
				_settextcolor(15);
				_settextposition(name[1]+4,2);
				sprintf(buffer,"%s",hsname[name[1]]);
				_outtext(buffer);
				_settextcolor(7);
				_settextposition(25,1);
				_outtext("Please type in old password of the highlighted name.                           ");
				password();
				if ((strcmp(str,hspass[name[1]]) != 0) && (option[3] > 0))
					ksay("wrong.wav");
				if (strcmp(str,hspass[name[1]]) == 0)
				{
					_settextposition(25,1);
					_outtext("Please type in new password of the highlighted name.                           ");
					password();
					strcpy(hspass[name[1]],str);
				}
				_setactivepage(pag = 1 - pag);
				drawchart();
				_setvisualpage(pag);
			}
			if ((ch == 13) && (name[0] != name[1]))
			{
				_settextcolor(7);
				_settextposition(25,1);
				_outtext("Please type in the password of the highlighted name.                           ");
				if ((lastpass[0] == name[0]) && (lastpass[1] == name[1]))
					for(i=0;i<2;i++)
						pass[i] = 1;
				if ((lastpass[0] != name[0]) || (lastpass[1] != name[1]))
					for(i=1;i>=0;i--)
					{
						if (stricmp(hsname[name[i]],"computer") == 0)
							pass[i] = 1;
						if (stricmp(hsname[name[i]],"computer") != 0)
						{
							pass[i] = 0;
							_settextcolor(15);
							_settextposition(name[i]+4,2);
							sprintf(buffer,"%s",hsname[name[i]]);
							_outtext(buffer);
							password();
							_settextposition(name[i]+4,2);
							sprintf(buffer,"%s",hsname[name[i]]);
							_outtext(buffer);
							if (strcmp(str,hspass[name[i]]) == 0)
								pass[i] = 1;
							if ((strcmp(str,hspass[name[i]]) != 0) && (i == 1))
								i = 0;
						}
					}
				if ((pass[0] != 1) || (pass[1] != 1))
				{
					if (option[3] > 0)
						ksay("wrong.wav");
					_setactivepage(pag = 1 - pag);
					drawchart();
					_setvisualpage(pag);
				}
				if ((pass[0] == 1) && (pass[1] == 1))
					for (i=0;i<2;i++)
						lastpass[i] = name[i];
			}
			if (option[9] > 1)
			{
				if ((ch == ',') || (ch == '<'))
				{
					 midiinst = (midiinst + 29) % 30;
					 setmidiinsts();
				}
				if ((ch == '.') || (ch == '>'))
				{
					midiinst = (midiinst + 1) % 30;
					setmidiinsts();
				}
			}
			drawinfo(1);
		}
	}
	if ((option[9] > 0) && ((enoughmemory&4) > 0))
		musicoff();
	if (ch == 27)
		return(0);
	else
		return(1);
}

drawinfo(color)
int color;
{
	int i;

	if (name[0] != name[1])
	{
		_settextcolor(15);
		if (color == 0)
			_settextcolor(((name[0] % 2) * 2) + 4);
		_settextposition(name[1]+4,name[0]*3+20);
		sprintf(buffer,"%d",hswin[name[0]][name[1]]);
		_outtext(buffer);
		if (color == 1)
			_settextcolor(13);
		_settextposition(name[0]+4,name[1]*3+20);
		sprintf(buffer,"%d",hswin[name[1]][name[0]]);
		_outtext(buffer);
	}
	for(i=0;i<2;i++)
	{
		_settextcolor(i * 2 + 13);
		if (color == 0)
			_settextcolor(7);
		_settextposition(name[i]+4,2);
		sprintf(buffer,"%s",hsname[name[i]]);
		_outtext(buffer);
	}
}

drawchart()
{
	int i, j, k, l, win, los;

	_clearscreen(_GCLEARSCREEN);
	_settextcolor(7);
	_settextposition(1,1);
	_outtext("ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÂÄÄÂÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿");
	_settextposition(2,1);
	_outtext("³              ³  ³1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17 18 19 20 ³");
	_settextposition(3,1);
	_outtext("ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÅÄÄÅÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´");
	_settextposition(24,1);
	_outtext("ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÁÄÄÁÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ");
	for(j=0;j<20;j++)
	{
		_settextcolor(7);
		_settextposition(j+4,1);
		_outtext("³              ³  ³                                                            ³");
		_settextposition(j+4,2);
		sprintf(buffer,"%s",hsname[j]);
		_outtext(buffer);
		_settextposition(j+4,17);
		sprintf(buffer,"%d",j+1);
		_outtext(buffer);
		for(i=0;i<20;i++)
		{
			if (i != j)
			{
				_settextcolor(((i % 2) * 2) + 4);
				_settextposition(j+4,i*3+20);
				sprintf(buffer,"%d",hswin[i][j]);
				_outtext(buffer);
			}
			if (i == j)
			{
				win = 0;
				los = 0;
				for(l=0;l<20;l++)
					if (l != i)
						win+=hswin[l][i];
				for(l=0;l<20;l++)
					if (l != j)
						los+=hswin[i][l];
				if ((win > 0) || (los > 0))
					k = (100. * win) / (win + los) + .5;
				else
					k = 0;
				_settextcolor(11);
				_settextposition(j+4,i*3+20);
				sprintf(buffer,"%d",k);
				_outtext(buffer);
			}
		}
	}
	_settextcolor(7);
	_settextposition(25,1);
	_outtext("4 arrows - move, C - change player, P - change password, Enter - play Kentris!");
}

password()
{
	int i;
	char ch;

	_settextcolor(7);
	_settextposition(2,2);
	_outtext("              ");
	_settextposition(2,2);
	i = 0;
	ch = 0;
	while (ch != 13)
	{
		ch = getch();
		if (ch == 0)
		{
			ch = getch();
			ch = 0;
		}
		if ((ch == 8) && (i > 0))
		{
			i--;
			_settextposition(2,2+i);
			_outtext(" ");
		}
		if ((ch != 13) && (ch != 8) && (i < 14))
		{
			str[i] = ch;
			_settextposition(2,2+i);
			_outtext("*");
			i++;
		}
	}
	str[i] = 0;
}

computermove(pl)
int pl;
{
	int i, j, rotation, place, rotsave, plasave, size, high, work, top[10];
	int hole, holsave, average, dependable, direction;

	average = 0;
	for(i=0;i<10;i++)
	{
		j = 0;
		while (board[i][j][pl] == 0)
			j++;
		top[i] = j;
		average += j;
	}
	average /= 10;
	if (top[0]+top[1]+top[2]+top[3]+top[4] > top[5]+top[6]+top[7]+top[8]+top[9])
		direction = 1;
	else
		direction = -1;
	dependable = 0;
	for(i=0;i<9;i++)
		if (abs(top[i]-top[i+1]) > 2)
			dependable++;
	high = 0;
	rotsave = 0;
	plasave = 0;
	holsave = 20;
	for(rotation=0;rotation<4;rotation++)
	{
		size = 1;
		if (fit[kind[pl]][rotation][1] != -1)
			size = 2;
		if (fit[kind[pl]][rotation][2] != -1)
			size = 3;
		if (fit[kind[pl]][rotation][3] != -1)
			size = 4;
		if (direction == 1)
			for(place=0;place<11-size;place++)
			{
				work = 20;
				for(i=place;i<place+size;i++)
					if (top[i] + fit[kind[pl]][rotation][i-place] < work)
						work = top[i] + fit[kind[pl]][rotation][i-place];
				hole = 0;
				for(i=place;i<place+size;i++)
					if (top[i] + fit[kind[pl]][rotation][i-place] > work)
						hole++;
				if ((work >= average - 2) && ((hole < holsave) || ((hole == holsave) && (work > high))))
				{
					high = work;
					rotsave = rotation;
					plasave = place;
					holsave = hole;
				}
			}
		if (direction == -1)
			for(place=10-size;place>=0;place--)
			{
				work = 20;
				for(i=place;i<place+size;i++)
					if (top[i] + fit[kind[pl]][rotation][i-place] < work)
						work = top[i] + fit[kind[pl]][rotation][i-place];
				hole = 0;
				for(i=place;i<place+size;i++)
					if (top[i] + fit[kind[pl]][rotation][i-place] > work)
						hole++;
				if ((work >= average - 2) && ((hole < holsave) || ((hole == holsave) && (work > high))))
				{
					high = work;
					rotsave = rotation;
					plasave = place;
					holsave = hole;
				}
			}
	}
	if ((rotate[pl] != rotsave) || (x[pl] != plasave+fit[kind[pl]][rotate[pl]][4]))
	{
		i = rotate[pl];
		j = x[pl];
		rotate[pl] = rotsave;
		x[pl] = plasave+fit[kind[pl]][rotate[pl]][4];
		if (check(pl,x[pl],y[pl]) == 1)
		{
			rotate[pl] = i;
			x[pl] = j;
		}
	}
	down[pl] = 1;
}

introduction()
{
	int i, page, saidkentris;
	char ch;
	void far *addr = (void far *)buffer;

	xdim = 640;
	ydim = 350;
	cols = 16;
	movedata(0x0000,0x0417,FP_SEG(addr),FP_OFF(addr),1);
	buffer[0] |= 0x20;
	movedata(FP_SEG(addr),FP_OFF(addr),0x0000,0x0417,1);
	if ((_setvideomode(_ERESCOLOR) == 0) || (option[10] == 3))
	{
		xdim = 640;
		ydim = 200;
		cols = 4;
		if (_setvideomode(_HRESBW) == 0)
		{
			printf("\nPlay me when you get graphics!\n");
			quitgame();
		}
	}
	loadfile();
	if ((option[9] > 0) && ((enoughmemory&4) > 0))
		loadmusic("intro.ksm");
	page = 0;
	_setactivepage(1-page);
	_setvisualpage(page);
	ch = 0;
	saidkentris = 0;
	while (ch != 13)
	{
		cosa1 = cos(a1);
		cosa2 = cos(a2);
		cosa3 = cos(a3);
		sina1 = sin(a1);
		sina2 = sin(a2);
		sina3 = sin(a3);

		//_setfillmask(0xffff);
		_setcolor(0);
		_rectangle(_GFILLINTERIOR,0,0,639,479);

		//_clearscreen(_GCLEARSCREEN);

		_settextcolor(3);
		_settextposition(1,20);
		_outtext("Enter - play, Space - options, F1 - help");
		_settextposition(25,24);
		_outtext("V4.3 Written by Kenneth Silverman");
		drawlines();
		_setactivepage(page);
		page = 1 - page;
		_setvisualpage(page);
		while((inp(0x3da) & 8) != 0);
		while((inp(0x3da) & 8) == 0);
		if (saidkentris == 0)
		{
			if (option[3] > 0)
				ksay("kentris.wav");
			saidkentris = 1;
			if ((option[9] > 0) && ((enoughmemory&4) > 0))
				musicon();
		}
		if (kbhit())
		{
			ch = getch();
			if ((ch == 27) || (ch == 'q') || (ch == 'Q'))
			{
				if ((option[9] > 0) && ((enoughmemory&4) > 0))
					musicoff();
				_setvideomode(_DEFAULTMODE);
				if (saveset() != 0)
					printf("Cannot save .SET file.\n");
				if (savedat() != 0)
					printf("Cannot save .DAT file.\n");
				if (option[9] == 2)
				{
					midiscrap = 256;
					while ((midiscrap > 0) && ((inp(statport) & 0x40) != 0))
						midiscrap--;
					outp(statport,0xff);
					midiscrap = 256;
					while ((midiscrap > 0) && ((inp(dataport) & 0xfe) != 0))
						midiscrap--;
				}
				quitgame();
			}
			if (ch == 32)
			{
				if ((option[9] > 0) && ((enoughmemory&4) > 0))
					musicoff();
				options();
				if ((option[9] > 0) && ((enoughmemory&4) > 0))
					loadmusic("intro.ksm");
				ydim = 350;
				cols = 16;
				if ((_setvideomode(_ERESCOLOR) == 0) || (option[10] == 3))
				{
					ydim = 200;
					cols = 4;
					if (_setvideomode(_HRESBW) == 0)
					{
						printf("\nPlay me when you get graphics!\n");
						quitgame();
					}
				}
				_setactivepage(1);
				_setvisualpage(0);
				page = 0;
				saidkentris = 0;
			}
			if (ch == 0)
			{
				ch = getch();
				if (ch == 59)
				{
					helpscreen();
					ydim = 350;
					cols = 16;
					if ((_setvideomode(_ERESCOLOR) == 0) || (option[10] == 3))
					{
						ydim = 200;
						cols = 4;
						if (_setvideomode(_HRESBW) == 0)
						{
							printf("\nPlay me when you get graphics!\n");
							quitgame();
						}
					}
					_setactivepage(1);
					_setvisualpage(0);
					page = 0;
				}
			}
			if (option[9] > 1)
			{
				if ((ch == ',') || (ch == '<'))
				{
					midiinst = (midiinst + 29) % 30;
					setmidiinsts();
				}
				if ((ch == '.') || (ch == '>'))
				{
					midiinst = (midiinst + 1) % 30;
					setmidiinsts();
				}
			}
		}
		xd+=xdir;
		yd+=ydir;
		zd+=zdir;
		a1+=ang1;
		a2+=ang2;
		a3+=ang3;
	}
	if ((option[9] > 0) && ((enoughmemory&4) > 0))
		musicoff();
	_setvideomode(_DEFAULTMODE);
}

drawtri(int *obj)
{
	int i, umost, dmost;

	umost = ydim;
	dmost = -1;
	for(i=3;i<=9;i+=2)
	{
		if (obj[i] < umost) umost = obj[i];
		if (obj[i] > dmost) dmost = obj[i];
	}
	if (umost < 0) umost = 0;
	if (dmost >= ydim) dmost = ydim-1;
	for (i=umost;i<=dmost;i++)
	{
		lmost[i] = xdim;
		rmost[i] = -1;
	}
	checkline(obj[2],obj[3],obj[4],obj[5]);
	checkline(obj[4],obj[5],obj[6],obj[7]);
	if (obj[0] == 3)
		checkline(obj[6],obj[7],obj[2],obj[3]);
	if (obj[0] == 4)
	{
		checkline(obj[6],obj[7],obj[8],obj[9]);
		checkline(obj[8],obj[9],obj[2],obj[3]);
	}
	_setcolor(obj[1]);
	for(i=umost;i<=dmost;i++)
		if (lmost[i] < rmost[i])
		{
			_moveto(lmost[i],i);
			_lineto(rmost[i],i);
		}
}

checkline(x1,y1,x2,y2)
int x1,y1,x2,y2;
{
	int dx, dy, incr1, incr2, d, x, y, xend, yend, yinc, xinc;

	dx = abs(x2 - x1);
	dy = abs(y2 - y1);
	if (dx >= dy)
	{
		if (x1 > x2)
		{
			x = x2; y = y2; xend = x1;
			if (dy == 0)
				yinc = 0;
			else
			{
				if (y2 > y1)
					yinc = -1;
				else
					yinc = 1;
			}
		}
		else
		{
			x = x1; y = y1; xend = x2;
			if (dy == 0)
				yinc = 0;
			else
			{
				if (y2 > y1)
					yinc = 1;
				else
					yinc = -1;
			}
		}
		incr1 = 2*dy; d = incr1-dx; incr2 = 2 * (dy-dx);
		if (x < lmost[y])
			lmost[y] = x;
		if (x > rmost[y])
			rmost[y] = x;
		while (x < xend)
		{
			x++;
			if (d < 0)
				d += incr1;
			else
			{
				y += yinc;
				d = d + incr2;
			}
			if (x < lmost[y])
				lmost[y] = x;
			if (x > rmost[y])
				rmost[y] = x;
		}
	}
	else
	{
		if (y1 > y2)
		{
			x = x2; y = y2; yend = y1;
			if (dx == 0)
				xinc = 0;
			else
			{
				if (x2 > x1)
					xinc = -1;
				else
					xinc = 1;
			}
		}
		else
		{
			x = x1; y = y1; yend = y2;
			if (dx == 0)
				xinc = 0;
			else
			{
				if (x2 > x1)
					xinc = 1;
				else
					xinc = -1;
			}
		}
		incr1 = 2*dx; d = incr1-dy; incr2 = 2 * (dx-dy);
		if (x < lmost[y])
			lmost[y] = x;
		if (x > rmost[y])
			rmost[y] = x;
		while (y < yend)
		{
			y++;
			if (d < 0)
				d += incr1;
			else
			{
				x += xinc;
				d += incr2;
			}
			if (x < lmost[y])
				lmost[y] = x;
			if (x > rmost[y])
				rmost[y] = x;
		}
	}
}

drawlines()
{
	static int object[16][10];
	static double distance[16];
	int i, j, k, it;
	double x[4], y[4], z[4], xt, yt, zt, ft, zdoffs, zrecip;
	double mat[9];

	zdoffs = zd*((ydim==200)+1);

	mat[0] = cosa1*cosa2;
	mat[1] = -sina1*cosa2;
	mat[2] = sina2;
	mat[3] = sina1*cosa3+cosa1*sina2*sina3;
	mat[4] = cosa1*cosa3-sina1*sina2*sina3;
	mat[5] = -cosa2*sina3;
	mat[6] = sina1*sina3-cosa1*sina2*cosa3;
	mat[7] = cosa1*sina3+sina1*sina2*cosa3;
	mat[8] = cosa2*cosa3;

	for(i=0;i<numlines;i++)
	{
		k = 0;
		for(j=0;j<4;j++)
		{
			xt = lin[i][k+2]; yt = lin[i][k+3]; zt = lin[i][k+4];
			x[j] = xt*mat[0] + yt*mat[1] + zt*mat[2];
			y[j] = xt*mat[3] + yt*mat[4] + zt*mat[5];
			z[j] = xt*mat[6] + yt*mat[7] + zt*mat[8] + zdoffs;
			k += 3;
		}
		//if ((z[0] > 10) && (z[1] > 10) && (z[2] > 10) && (z[3] > 10))
		//{
			object[i][0] = (int)lin[i][0];
			object[i][1] = (int)lin[i][1];
			for(j=0;j<4;j++)
			{
				zrecip = 1024.0 / z[j];
				object[i][j*2+2] = x[j]*zrecip+xd+320;
				object[i][j*2+3] = y[j]*zrecip+yd+(ydim>>1);
			}
			//if (lin[i][0] == 3) distance[i] = (z[0]+z[1]+z[2])*4;
			//if (lin[i][0] == 4)
				distance[i] = (z[0]+z[1]+z[2]+z[3]); //*3;
		//}
	}
	if (ydim == 200)
	{
		for(i=1;i<numlines;i++)
			for(j=0;j<i;j++)
				if(distance[i] < distance[j])
				{
					ft = distance[i];
					distance[i] = distance[j];
					distance[j] = ft;
					for(k=0;k<10;k++)
					{
						it = object[i][k];
						object[i][k] = object[j][k];
						object[j][k] = it;
					}
				}
		for(i=0;i<numlines;i++) drawtri(&object[i][0]);
	}
	if (ydim == 350)
	{
		/*for(i=1;i<numlines;i++)
			for(j=0;j<i;j++)
				if(distance[i] > distance[j])
				{
					ft = distance[i];
					distance[i] = distance[j];
					distance[j] = ft;
					for(k=0;k<10;k++)
					{
						it = object[i][k];
						object[i][k] = object[j][k];
						object[j][k] = it;
					}
				}*/

//    -70 -30 -20 20 30  70
//-25  |0  /  |----4 |\   |
//     | /1   |      | \ 9|
// 0   |<    3|----5 | 8\ |
//     | \2   |      |   \|
//25   |   \  |----6 |7   |
//
//K:  9876543210
//KE: 9876543021
//EN: 0213987456
//NN: 0213456798
//N:  0213456789
		ft = (mat[2]*mat[4] - mat[1]*mat[5])*zdoffs;

		if (ft < -70)
			{ for(i=10-1;i>=0;i--) drawtri(&object[i][0]); }
		else if (ft > 70)
		{
			drawtri(&object[0][0]); drawtri(&object[2][0]); drawtri(&object[1][0]);
			for(i=3;i<10;i++) drawtri(&object[i][0]);
		}
		else if (ft < -20)
		{
			for(i=9;i>=3;i--) drawtri(&object[i][0]);
			drawtri(&object[0][0]); drawtri(&object[2][0]); drawtri(&object[1][0]);
		}
		else if (ft > 30)
		{
			drawtri(&object[0][0]); drawtri(&object[2][0]); drawtri(&object[1][0]);
			for(i=3;i<=7;i++) drawtri(&object[i][0]);
			drawtri(&object[9][0]); drawtri(&object[8][0]);
		}
		else
		{
			drawtri(&object[0][0]); drawtri(&object[2][0]); drawtri(&object[1][0]);
			drawtri(&object[3][0]); drawtri(&object[9][0]); drawtri(&object[8][0]);
			drawtri(&object[7][0]); drawtri(&object[4][0]); drawtri(&object[5][0]);
			drawtri(&object[6][0]);
		}
	}
}

loadfile()
{
	FILE *infile;
	int i;

	if ((infile = fopen("kensign.3d","r")) == 0)
	{
		xd=5; yd=-20; zd=300; xdir=0; ydir=0; zdir=0;
		a1=0; a2=-7.02; a3=12.82; ang1=0; ang2=.05; ang3=0;
		numlines = 10;
		return(0);
	}
	fscanf(infile,"%lf,%lf,%lf,%lf,%lf,%lf\n",&xd,&yd,&zd,&xdir,&ydir,&zdir);
	fscanf(infile,"%lf,%lf,%lf,%lf,%lf,%lf\n",&a1,&a2,&a3,&ang1,&ang2,&ang3);
	numlines = 0;
	do
	{
		for(i=0;i<13;i++)
			fscanf(infile,"%lf,",&lin[numlines][i]);
		fscanf(infile,"%lf\n",&lin[numlines][13]);
		numlines++;
	}
	while(feof(infile) == 0);
	fclose(infile);
}

helpscreen()
{
	char ch;

	_setvideomode(_TEXTC80);
	_displaycursor(_GCURSOROFF);
	_settextcolor(7);
	_settextposition(1,1);
	_outtext("ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿");
	_outtext("³                        ³ Kentris Help Screen (1 of 3) ³                      ³");
	_outtext("³                        ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ                      ³");
	_outtext("³                                                                              ³");
	_outtext("³    Kentris was made to be the most exciting competition tetris game available³");
	_outtext("³ for PC's.  I started to write Kentris in February '91 for a group of freshmen³");
	_outtext("³ at Brown University and many of the ideas came from them.  This product may  ³");
	_outtext("³ be copied freely as long as it is not modified in any way.                   ³");
	_outtext("³                          ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿                          ³");
	_outtext("³             Left Player: ³ Kentris Game Controls: ³ Right Player:            ³");
	_outtext("³            ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÄÄÄÄ¿             ³");
	_outtext("³            ³    ÚÄÄÄ¿ ÚÄÄÄ¿ ÚÄÄÄ¿    ³    ÚÄÄÄ¿ ÚÄÄÄ¿ ÚÄÄÄ¿    ³             ³");
	_outtext("³            ³    ³J,A³ ³K,S³ ³L,D³    ³    ³ 4 ³ ³ 5 ³ ³ 6 ³    ³             ³");
	_outtext("³            ³    ÀÄÂÄÙ ÀÄÂÄÙ ÀÄÂÄÙ    ³    ÀÄÂÄÙ ÀÄÂÄÙ ÀÄÂÄÙ    ³             ³");
	_outtext("³            ³    left rotate right    ³     left rotate right   ³             ³");
	_outtext("³            ³        ÚÄÄÄÄÄÄÄ¿        ³          ÚÄÄÄ¿          ³             ³");
	_outtext("³            ³        ³ Space ³        ³          ³ 0 ³          ³             ³");
	_outtext("³            ³        ÀÄÄÄÂÄÄÄÙ        ³          ÀÄÂÄÙ          ³             ³");
	_outtext("³            ³        drop fast        ³        drop fast        ³             ³");
	_outtext("³            ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ             ³");
	_outtext("³ If you have any problems, questions, or ideas about Kentris, please visit    ³");
	_outtext("³ my official website (http://advsys.net/ken) where you will find my current   ³");
	_outtext("³ E-mail address and forum link.                                               ³");
	_outtext("ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ");
	ch = 0;
	while (ch == 0)
	{
		if (kbhit() != 0)
			ch = getch();
	}
	_settextcolor(7);
	_settextposition(1,1);
	_outtext("ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿");
	_outtext("³                        ³ Kentris Help Screen (2 of 3) ³                      ³");
	_outtext("³                        ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ                      ³");
	_outtext("³                                                                              ³");
	_outtext("³    When playing in competition mode (set in the options menu), a chart will  ³");
	_outtext("³ appear on the screen before each game starts.  This chart allows up to 20    ³");
	_outtext("³ different Kentris players to enter their names and passwords in this chart to³");
	_outtext("³ keep track of their wins, losses, and overall winning percentages. Here is a ³");
	_outtext("³ small section of the game tracking chart:                                    ³");
	_outtext("³    To use the chart, first find the left player's   ÚÄÄÄÄÄÄÄÄÂÄÂÄÄÄÄÄÄÄÄÄÄÄÄ¿³");
	_outtext("³ name.  In this case, Awesoken (me) will be the left ³        ³ ³1  2  3  4  ³³");
	_outtext("³ player.  Looking to the right of this name, in the  ÃÄÄÄÄÄÄÄÄÅÄÅÄÄÄÄÄÄÄÄÄÄÄÄ´³");
	_outtext("³ second column, is the number 13.  This means that I ³Awesoken³1³56 13 0  7  ³³");
	_outtext("³ beat player #2 (Smeal) 13 times.  To see how many   ³Smeal   ³2³11 48 0  4  ³³");
	_outtext("³ times I lost to Smeal, look under his row of wins   ³Computer³3³0  0  0  0  ³³");
	_outtext("³ against Awesoken.  The bright blue number is the    ³Computer³4³5  3  0  42 ³³");
	_outtext("³ player's total winning percentage.  When you press  ÀÄÄÄÄÄÄÄÄÁÄÁÄÄÄÄÄÄÄÄÄÄÄÄÙ³");
	_outtext("³ enter to play Kentris, the white highlighted name is always the left player  ³");
	_outtext("³ and the pink is always the right player.                                     ³");
	_outtext("³                                                                              ³");
	_outtext("³ If you have any problems, questions, or ideas about Kentris, please visit    ³");
	_outtext("³ my official website (http://advsys.net/ken) where you will find my current   ³");
	_outtext("³ E-mail address and forum link.                                               ³");
	_outtext("ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ");
	_settextcolor(11);
	_settextposition(13,67), _outtext("56");
	_settextposition(14,70), _outtext("48");
	_settextposition(15,73), _outtext("0");
	_settextposition(16,76), _outtext("42");
	_settextcolor(15);
	_settextposition(13,56), _outtext("Awesoken");
	_settextposition(13,70), _outtext("13");
	_settextcolor(13);
	_settextposition(14,56), _outtext("Smeal");
	_settextposition(14,67), _outtext("11");
	ch = 0;
	while (ch == 0)
	{
		if (kbhit() != 0)
			ch = getch();
	}
	_settextcolor(7);
	_settextposition(1,1);
	_outtext("ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿");
	_outtext("³ (Note: OUT OF DATE!)   ³ Kentris Help Screen (3 of 3) ³                      ³");
	_outtext("³                        ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ                      ³");
	_outtext("³                                                                              ³");
	_outtext("³    A great new game produced by the same programmer as Kentris will be coming³");
	_outtext("³ out soon, called Ken's Labyrinth.  I have made it to become the fastest three³");
	_outtext("³ dimentional scrolling game that I have seen available for PC's.  On a 486-50 ³");
	_outtext("³ the screen updates on average 40 times a second!  It also has over 32 great  ³");
	_outtext("³ new Adlib songs and Sound Blaster digitized sound, of course.  Unlike other  ³");
	_outtext("³ 3-d scrolling games, Ken's Labyrinth features a high-resolution 360*240      ³");
	_outtext("³ graphics mode for those who have multi-sync monitors, and it has see-through ³");
	_outtext("³ doors and windows so one can look into a room before actually entering it.   ³");
	_outtext("³ The art is outstanding, with over 300 pictures giving Ken's Labyrinth more   ³");
	_outtext("³ variety than any other game of its type.  For controls, you can use either   ³");
	_outtext("³ the mouse, joystick, or custom keys on the keyboard.                         ³");
	_outtext("³    REQUIREMENTS:                                                             ³");
	_outtext("³    - Any IBM PC compatible with a VGA display card and VGA monitor.          ³");
	_outtext("³    - At least 350K of memory (XMS or EMS driver suggested for optimal speed) ³");
	_outtext("³    - Slightly under 1MB of hard drive space - Well worth the space!          ³");
	_outtext("³                                                                              ³");
	_outtext("³ If you would like more information about Ken's Labyrinth, please visit       ³");
	_outtext("³ my official website (http://advsys.net/ken).                                 ³");
	_outtext("³                                                                              ³");
	_outtext("ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ");
	ch = 0;
	while (ch == 0)
	{
		if (kbhit() != 0)
			ch = getch();
	}
	ch = 0;
}

reset_dsp()
{
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
}

setvideomodetseng()
{
	unsigned char new_value, old_value, value;
	int base, returnvalue;

	returnvalue = 0;
	if (option[10] == 0)
	{
		outp(0x3bf, 3);
		if ((inp(0x3cc) & 0x01) == 1)
			outp(0x3d8, 0xa0);
		else
			outp(0x3b8, 0xa0);
		inp(0x3da);
		outp(0x3c0, 0x16);
		old_value = inp(0x3c1);
		inp(0x3da);
		outp(0x3c0, 0x16); new_value = old_value ^ 0x10;
		outp(0x3c0, new_value);
		inp(0x3da); outp(0x3c0, 0x16);
		value = inp(0x3c1);
		inp(0x3da); outp(0x3c0, 0x16);
		outp(0x3c0, old_value);
		if (value == new_value)
		{
			if ((inp(0x3cc) & 0x01) == 1)
				base = 0x3d0;
			else
				base = 0x3b0;
			outp(base+4,0x33); old_value = inp(base+5);
			outp(base+5,old_value ^ 0x0f); new_value = inp(base+5);
			outp(base+5,old_value);
			if (new_value == (old_value ^ 0x0f))
			{
				returnvalue = 1;
				_asm \
				{
					mov ax, 0x38
					int 0x10
				}
			}
		}
	}
	return(returnvalue);
}

hlin(x1,x2,y,color)
int x1,x2,y;
char color;
{
	unsigned int lopos, lineleng;

	lineleng = (x2-x1)+1;
	outp(0x3cd,y>>6);
	lopos = ((unsigned int)(y&63)<<10)+x1;
	_asm \
	{
		mov ax, 0xa000
		mov es, ax
		mov di, lopos
		mov cx, lineleng
		mov al, color
		cld
		rep stosb
	}
}

vlin(x,y1,y2,color)
int x,y1,y2;
char color;
{
	unsigned int lopos, lineleng, ycount;

	outp(0x3cd,y1>>6);
	lopos = ((unsigned int)(y1&63)<<10)+x;
	_asm \
	{
		mov ax, 0xa000
		mov es, ax
	}
	for(ycount=y1;ycount<=y2;ycount++)
	{
		_asm \
		{
			mov di, lopos
			mov al, color
			stosb
		}
		lopos += 1024;
		if (lopos < 1024)
			outp(0x3cd,(ycount+1)>>6);
	}
}

rectfill(x1,y1,x2,y2,color)
int x1,y1,x2,y2;
char color;
{
	unsigned int lopos, lineleng, ycount;

	lineleng = (x2-x1)+1;
	outp(0x3cd,y1>>6);
	lopos = ((unsigned int)(y1&63)<<10)+x1;
	_asm \
	{
		mov ax, 0xa000
		mov es, ax
		cld
	}
	for(ycount=y1;ycount<=y2;ycount++)
	{
		_asm \
		{
			mov di, lopos
			mov cx, lineleng
			mov al, color
			rep stosb
		}
		lopos += 1024;
		if (lopos < 1024)
			outp(0x3cd,(ycount+1)>>6);
	}
}

rectopen(x1,y1,x2,y2,color)
int x1,y1,x2,y2;
char color;
{
	unsigned int lopos, lineleng, ycount;

	lineleng = (x2-x1)+1;
	outp(0x3cd,y1>>6);
	lopos = ((unsigned int)(y1&63)<<10)+x1;
	_asm \
	{
		mov ax, 0xa000
		mov es, ax
		mov di, lopos
		mov cx, lineleng
		mov al, color
		cld
		rep stosb
	}
	for(ycount=y1+1;ycount<y2;ycount++)
	{
		lopos += 1024;
		if (lopos < 1024)
			outp(0x3cd,(ycount+1)>>6);
		_asm \
		{
			mov di, lopos
			mov al, color
			stosb
			add di, lineleng
			dec di
			dec di
			stosb
		}
	}
	lopos += 1024;
	if (lopos < 1024)
		outp(0x3cd,(ycount+1)>>6);
	_asm \
	{
		mov di, lopos
		mov cx, lineleng
		mov al, color
		rep stosb
	}
}

outstring(y,x,col,buf)
unsigned char y, x, col, buf[80];
{
	int i;
	char j;

	_asm \
	{
		mov ah, 2
		mov bh, 0
		mov dh, y
		mov dl, x
		int 0x10
	}
	for(i=0;i<strlen(buf);i++)
	{
		j = buf[i];
		_asm \
		{
			mov ah, 0xe
			mov al, j
			mov bl, col
			int 0x10
		}
	}
}

getblock(x1,y1,x2,y2,blocknum)
unsigned int x1,y1,x2,y2,blocknum;
{
	unsigned int lopos, lineleng, ycount, arrpos;
	void far *addr = (void far *)blockmap;

	lineleng = (x2-x1)+1;
	outp(0x3cd,(y1>>6)<<4);
	lopos = ((unsigned int)(y1&63)<<10)+x1;
	arrpos = (blocknum<<10);
	for(ycount=y1;ycount<=y2;ycount++)
	{
		movedata(0xa000,lopos,FP_SEG(addr),FP_OFF(addr)+arrpos,lineleng);
		arrpos += lineleng;
		lopos += 1024;
		if (lopos < 1024)
			outp(0x3cd,((ycount+1)>>6)<<4);
	}
}

putblock(x1,y1,x2,y2,blocknum)
unsigned int x1,y1,x2,y2,blocknum;
{
	unsigned int lopos, lineleng, ycount, arrpos;
	void far *addr = (void far *)blockmap;

	lineleng = (x2-x1)+1;
	outp(0x3cd,y1>>6);
	lopos = ((unsigned int)(y1&63)<<10)+x1;
	arrpos = (blocknum<<10);
	for(ycount=y1;ycount<=y2;ycount++)
	{
		movedata(FP_SEG(addr),FP_OFF(addr)+arrpos,0xa000,lopos,lineleng);
		arrpos += lineleng;
		lopos += 1024;
		if (lopos < 1024)
			outp(0x3cd,(ycount+1)>>6);
	}
}

loadmusic(filename)
char filename[80];
{
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
}

outdata(synth,index,data)
unsigned char synth,index,data;
{
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
}

musicon()
{
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
}

musicoff()
{
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
}

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

setinst(synth,chan,v0,v1,v2,v3,v4,v5,v6,v7,v8,v9,v10)
int chan;
unsigned char synth,v0,v1,v2,v3,v4,v5,v6,v7,v8,v9,v10;
{
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
}

setmidiinsts()
{
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
}
