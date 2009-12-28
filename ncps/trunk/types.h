#ifndef _TYPESDECL
#define _TYPESDECL
#include <pspkernel.h>
#include <pspctrl.h>

#define SCR_WIDTH		(480)
#define SCR_HEIGHT		(272)
#define PIXEL_SIZE		(4)
#define BUF_WIDTH		(512)
#define BUF_SIZE		(BUF_WIDTH*SCR_HEIGHT*PIXEL_SIZE)


#define ARGB(a, r, g, b) ((a << 24) | (r << 16) | (g << 8) | b)
#define RGBA(r, g, b, a) ((a << 24) | (b << 16) | (g << 8) | r)

#define tostr(n) tostr_(n)
#define tostr_(n) #n
#define TRACE pspDebugScreenPrintf("TRACE: (%s:%s)\n", __FILE__, tostr(__LINE__));
#define TRACEF {sprintf(trace_buf, "TRACE: (%s:%s)\r\n", __FILE__, tostr(__LINE__));fp=fopen("ms0:/debug.txt", "a");fputs(trace_buf, fp);fclose(fp);}

#define FREE(_Memory) if(_Memory!=NULL){free(_Memory);_Memory=NULL;}

struct intraFont;
extern intraFont *ltn[16];
extern intraFont *jpn0;
extern intraFont *kr0;
extern intraFont *arib;
extern intraFont *chn;

class GUMenu;
class GUListView;
class GUTextBox;
extern GUMenu *menubar;
extern GUListView *lview;
extern GUTextBox *GUT_stat;

extern unsigned int __attribute__((aligned(16))) list[262144];
extern bool g_controllable;

extern SceCtrlData currpad;

#endif