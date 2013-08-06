#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xft/Xft.h>

#include <sys/time.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <pthread.h>
#include <stdlib.h>


unsigned long UsrColorPixel( Display*, char* );
void Pause( Display*, int );

static int kill_thread =0;

void* thread_pattern_gen(void *data){

	Display *dp;
	Bool is_ntsc = (Bool)data;
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

	mw = XCreateSimpleWindow ( dp, DefaultRootWindow( dp ),
			0, 0, 720, is_ntsc?480:576, 0,
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
		return 0;
	}
	xftdraw = XftDrawCreate(dp, mw, DefaultVisual(dp, 0), DefaultColormap(dp, 0));

	xrcolor.red = 65535;
	xrcolor.green = 65535;
	xrcolor.blue =65535;
	xrcolor.alpha = 65535;

	XftColorAllocValue(dp, DefaultVisual(dp, 0), DefaultColormap(dp, 0), &xrcolor, &xftcolor);




	char line1[40];
	char line2[40];

	int line1_y = is_ntsc? (480/2)-30 : (576/2)-30;
	int line2_y = is_ntsc? (480/2)+30 : (576/2)+30;

	int rect1_y = is_ntsc? 480/4 : 576/4;
	int rect2_y = is_ntsc? (480/4)*3 : (576/4)*3;


	while(1){
		if(kill_thread) break;

		int move;
		for( move = 0; move < 30; move ++){
			gettimeofday(&tv, NULL);
			struct tm *ptm = localtime(&(tv.tv_sec));


			XFlush(dp);
			XClearWindow( dp, mw );

			XFillRectangle ( dp, mw, gc_rect1, (move==0)? 0:(720/30)*move, rect1_y, 50, 10);

			XFillRectangle ( dp, mw, gc_rect2, (move==0)? 0:720-(720/30)*move, rect2_y, 50, 10);


			sprintf(line1," %04d/%02d/%02d ", ptm->tm_year+1900, ptm->tm_mon+1, ptm->tm_mday);

			sprintf(line2,"%02d:%02d:%02d %02ld\'",ptm->tm_hour, ptm->tm_min, ptm->tm_sec, tv.tv_usec/33333);
			XftDrawString8(xftdraw, &xftcolor, xft, 100, line1_y, (XftChar8 *)line1, 12);
			XftDrawString8(xftdraw, &xftcolor, xft, 100, line2_y, (XftChar8 *)line2, 12);
			usleep(33333);
		}
	}


	printf("thread kill\n");

	/* GC, 창 파괴 & 서버와의 접속 해제 */
	XFreeGC( dp, gc );
	XUnmapWindow( dp, mw );

	XftDrawDestroy(xftdraw);
	XftColorFree(dp, DefaultVisual(dp, 0), DefaultColormap(dp, 0), &xftcolor);

	XDestroyWindow( dp, mw );


	XCloseDisplay( dp );

	return 0;


}


int main(int argc, char* argv[])
{
	Bool vmode = 1;
	
	if(!(argc == 1 || argc ==2)){
		return -1;
	}

	if(argc ==2 && strcmp(argv[1], "pal") ==0 ){
		vmode =0;

	}else{
		vmode =1;
	}
	kill_thread = 0;
	pthread_t thread_t;
	int thread_ret;

	if(pthread_create(&thread_t, NULL, thread_pattern_gen, (void*)vmode) < 0){
		perror("thread create error:");
		exit(0);
	}


	while(1){
		char input[100];
		memset(input,0,sizeof(input));
		scanf("%s", input);

		if(strcmp(input, "exit")==0){
			kill_thread= 1;
			break;

		}else if(strcmp(input, "exit")==0){
		}else{

		}

	}
	pthread_join(thread_t,(void**)&thread_ret);
	printf("test pattern generator normally closed\n");
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
