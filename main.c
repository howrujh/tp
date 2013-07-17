#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xft/Xft.h>

#include <sys/time.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

/* 색상 이름에 해당하는 픽셀값 반환하는 함수 */
unsigned long UsrColorPixel( Display*, char* );
/* 요청을 플러쉬하고 지정된 초만큼 지연시키는 함수 */
void    Pause( Display*, int );

static XPoint points[] = {{ 200, 50 },{ 100, 150 },{ 300, 150 },{ 200, 50 }};
static XSegment segs[] = {{ 10, 10, 390, 190 },{ 10, 190, 390, 10 }};
static char dash_list[] = { 20, 5, 10, 5 };

int main()
{
	Display *dp;
	Window mw; /* One TopLevel Window & Two Child Windows */
	GC gc, gc_rect1, gc_rect2, gc_string;
	XftFont *xft;
	XftDraw *xftdraw;
	XRenderColor xrcolor;
	XftColor xftcolor;

	XGCValues gv;
	XSetWindowAttributes xswa;

	struct timeval tv;

	/* 서버와 접속하기 그리고 변수들을 초기화 */
	dp = XOpenDisplay ( NULL );

	/* 자, 시작해볼까요? */

	mw = XCreateSimpleWindow ( dp, DefaultRootWindow( dp ),
			0, 0, 720, 480, 0,
			WhitePixel(dp,0), BlackPixel(dp,0) );

	xswa.override_redirect = True;
	XChangeWindowAttributes ( dp, mw, CWOverrideRedirect, &xswa );
	XMapWindow( dp, mw );

	gv.foreground = UsrColorPixel( dp, "red" );
	gv.background = UsrColorPixel( dp, "yello" );
	gc = XCreateGC( dp, mw, GCForeground | GCBackground, &gv );
	gv.foreground = UsrColorPixel( dp, "red" );
	gv.background = UsrColorPixel( dp, "black" );
	gc_rect1 = XCreateGC( dp, mw, GCForeground | GCBackground, &gv );
	gv.foreground = UsrColorPixel( dp, "blue" );
	gv.background = UsrColorPixel( dp, "black" );
	gc_rect2 = XCreateGC( dp, mw, GCForeground | GCBackground, &gv );
	gv.foreground = UsrColorPixel( dp, "white" );
	gv.background = UsrColorPixel( dp, "black" );
	gc_string = XCreateGC( dp, mw, GCForeground | GCBackground, &gv );
	
	//xft  = XftFontOpenName( dp, 0, "Bitstream Vera Sans Mono-50:bold" ); 
	xft  = XftFontOpenName( dp, 0, "Courier New-48:bold" ); 
	if(!xft){
		printf("font null\n");
		return -1;
	}
	xftdraw = XftDrawCreate(dp, mw, DefaultVisual(dp, 0), DefaultColormap(dp, 0));

	xrcolor.red = 65535;
	xrcolor.green = 65535;
	xrcolor.blue =65535;
	xrcolor.alpha = 65535;

	XftColorAllocValue(dp, DefaultVisual(dp, 0), DefaultColormap(dp, 0), &xrcolor, &xftcolor);




	char line1[40];
	char line2[40];



	while(1){
		int move;
		for( move = 0; move < 30; move ++){
			gettimeofday(&tv, NULL);
			struct tm *ptm = localtime(&(tv.tv_sec));


			XFlush(dp);
			XClearWindow( dp, mw );

			XFillRectangle ( dp, mw, gc_rect1, (move==0)? 0:(720/30)*move, 40, 100, 100);

			XFillRectangle ( dp, mw, gc_rect2, (move==0)? 0:720-(720/30)*move, 340, 100, 100);


			sprintf(line1," %04d/%02d/%02d ", ptm->tm_year+1900, ptm->tm_mon+1, ptm->tm_mday);

			sprintf(line2,"%02d:%02d:%02d %02ld\"",ptm->tm_hour, ptm->tm_min, ptm->tm_sec, tv.tv_usec/33333);
			XftDrawString8(xftdraw, &xftcolor, xft, 100, 220, (XftChar8 *)line1, strlen(line1));
			XftDrawString8(xftdraw, &xftcolor, xft, 100, 280, (XftChar8 *)line2, strlen(line2));
			usleep(33333);
		}
	}


	getchar();

	/* GC, 창 파괴 & 서버와의 접속 해제 */
	XFreeGC( dp, gc );
	XUnmapWindow( dp, mw );

	XftDrawDestroy(xftdraw);
	XftColorFree(dp, DefaultVisual(dp, 0), DefaultColormap(dp, 0), &xftcolor);

	XDestroyWindow( dp, mw );


	XCloseDisplay( dp );

	return 0;
}

/*
 *
 *   UsrColorPixel() : 주어진 이름의 색상에 대한 기본 컬러맵의 픽셀값 반환
 *
 *   */

unsigned long UsrColorPixel( display, name )
	Display *display;
	char *name;
{
	Colormap cmap;
	XColor c0, c1;

	cmap = DefaultColormap( display, 0 );

	XAllocNamedColor( display, cmap, name, &c0, &c1 );

	return ( c0.pixel );
}

void Pause( Display *d, int secs )
{
	XFlush ( d );
	sleep( secs );
}