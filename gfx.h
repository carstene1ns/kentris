
#include <stdlib.h>

void drawtri(int*);
void checkline(int, int, int, int);
void drawlines();
void hlin(int, int, int, char);
void vlin(int, int, int, char);
void rectfill(int, int, int, int, char);
void rectopen(int, int, int, int, char);
void outstring(unsigned char, unsigned char, unsigned char, char[80]);

void getblock(unsigned int x1, unsigned int y1, unsigned int x2,
              unsigned int y2, unsigned int blocknum);
void putblock(unsigned int x1, unsigned int y1, unsigned int x2,
              unsigned int y2, unsigned int blocknum);

void _setfillmask();
void _setcolor();
void _rectangle();
void _settextcolor();
void _settextposition();
void _putimage();
void _lineto();
void _getimage();
void _displaycursor();
void _setvisualpage();
void _setactivepage();
void _clearscreen();
void _outtext();
void _moveto();

unsigned char *blockmap;
double cosa1, cosa2, cosa3, sina1, sina2, sina3;
double a1, a2, a3, xd, yd, zd;
int numlines, xdim, ydim, cols;
double lin[16][14];
