
#include <string.h>

#include "gfx.h"

static int lmost[350], rmost[350];

void drawtri(int *obj)
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

void checkline(int x1, int y1, int x2, int y2)
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

void drawlines()
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


void hlin(int x1, int x2, int y, char color)
{
	unsigned int lopos, lineleng;

	lineleng = (x2-x1)+1;
	outp(0x3cd,y>>6);
	lopos = ((unsigned int)(y&63)<<10)+x1;
/*
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
*/
}

void vlin(int x, int y1, int y2, char color)
{
	unsigned int lopos, lineleng, ycount;

	outp(0x3cd,y1>>6);
	lopos = ((unsigned int)(y1&63)<<10)+x;

/*
	_asm \
	{
		mov ax, 0xa000
		mov es, ax
	}
*/
	for(ycount=y1;ycount<=y2;ycount++)
	{
/*
		_asm \
		{
			mov di, lopos
			mov al, color
			stosb
		}
*/
		lopos += 1024;
		if (lopos < 1024)
			outp(0x3cd,(ycount+1)>>6);
	}
}

void rectfill(int x1, int y1, int x2, int y2, char color)
{
	unsigned int lopos, lineleng, ycount;

	lineleng = (x2-x1)+1;
	outp(0x3cd,y1>>6);
	lopos = ((unsigned int)(y1&63)<<10)+x1;
/*
	_asm \
	{
		mov ax, 0xa000
		mov es, ax
		cld
	}
*/
	for(ycount=y1;ycount<=y2;ycount++)
	{
/*
		_asm \
		{
			mov di, lopos
			mov cx, lineleng
			mov al, color
			rep stosb
		}
*/
		lopos += 1024;
		if (lopos < 1024)
			outp(0x3cd,(ycount+1)>>6);
	}
}

void rectopen(int x1, int y1, int x2, int y2, char color)
{
	unsigned int lopos, lineleng, ycount;

	lineleng = (x2-x1)+1;
	outp(0x3cd,y1>>6);
	lopos = ((unsigned int)(y1&63)<<10)+x1;
/*
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
*/
	for(ycount=y1+1;ycount<y2;ycount++)
	{
		lopos += 1024;
		if (lopos < 1024)
			outp(0x3cd,(ycount+1)>>6);
/*
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
*/
	}
	lopos += 1024;
	if (lopos < 1024)
		outp(0x3cd,(ycount+1)>>6);
/*
	_asm \
	{
		mov di, lopos
		mov cx, lineleng
		mov al, color
		rep stosb
	}
*/
}

void outstring(unsigned char y, unsigned char x, unsigned char col,
               char buf[80])
{
	int i;
	char j;

/*
	_asm \
	{
		mov ah, 2
		mov bh, 0
		mov dh, y
		mov dl, x
		int 0x10
	}
*/
	for(i=0;i<strlen(buf);i++)
	{
		j = buf[i];
/*
		_asm \
		{
			mov ah, 0xe
			mov al, j
			mov bl, col
			int 0x10
		}
*/
	}
}

void getblock(unsigned int x1, unsigned int y1, unsigned int x2,
              unsigned int y2, unsigned int blocknum)
{
	unsigned int lopos, lineleng, ycount, arrpos;
	void *addr = (void *)blockmap;

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

void putblock(unsigned int x1, unsigned int y1, unsigned int x2,
              unsigned int y2, unsigned int blocknum)
{
	unsigned int lopos, lineleng, ycount, arrpos;
	void *addr = (void *)blockmap;

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
