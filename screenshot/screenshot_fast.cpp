// g++ xshm2.c -o xshm2 -lX11 -lXext `$cv`-Ofast -mfpmath=both -march=native -m64 -funroll-loops -mavx2 && ./xshm2

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/XShm.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <iostream>
#include <cstdio>
#include <cstring>

// Using one monitor DOESN'T improve performance! Querying a smaller subset of the screen DOES
const uint WIDTH  = 1920>>0;
const uint HEIGHT = 1080>>0;
void saveXImageToBitmap(XImage *pImage);
// -------------------------------------------------------
int main(){
    Display* display = XOpenDisplay(NULL);
    Window root = DefaultRootWindow(display);  // Macro to return the root window! It's a simple uint32
    XWindowAttributes window_attributes;
    XGetWindowAttributes(display, root, &window_attributes);
    Screen* screen = window_attributes.screen;
    XShmSegmentInfo shminfo;
    XImage* ximg = XShmCreateImage(display, DefaultVisualOfScreen(screen), DefaultDepthOfScreen(screen), ZPixmap, NULL, &shminfo, WIDTH, HEIGHT);

    shminfo.shmid = shmget(IPC_PRIVATE, ximg->bytes_per_line * ximg->height, IPC_CREAT|0777);
    shminfo.shmaddr = ximg->data = (char*)shmat(shminfo.shmid, 0, 0);
    shminfo.readOnly = False;
    if(shminfo.shmid < 0)
        puts("Fatal shminfo error!");;
    Status s1 = XShmAttach(display, &shminfo);
    printf("XShmAttach() %s\n", s1 ? "success!" : "failure!");

    //cv::Mat img;

    for(int i; ; i++){
            XShmGetImage(display, root, ximg, 0, 0, 0x00ffffff);
            if (ximg ->data != NULL)
              saveXImageToBitmap(ximg);
            //img = cv::Mat(HEIGHT, WIDTH, CV_8UC4, ximg->data);
            break;
        }

    XShmDetach(display, &shminfo);
    XDestroyImage(ximg);
    shmdt(shminfo.shmaddr);
    XCloseDisplay(display);
    puts("Exit success!");
}
#pragma pack (1)
typedef struct BITMAPFILEHEADER 
{
short    bfType;
int    bfSize;
short    bfReserved1;
short    bfReserved2;
int   bfOffBits;
};

typedef struct BITMAPINFOHEADER
{
int  biSize;
int   biWidth;
int   biHeight;
short   biPlanes;
short   biBitCount;
int  biCompression;
int  biSizeImage;
int   biXPelsPerMeter;
int   biYPelsPerMeter;
int  biClrUsed;
int  biClrImportant;
};

void saveXImageToBitmap(XImage *pImage)
{
BITMAPFILEHEADER bmpFileHeader;
BITMAPINFOHEADER bmpInfoHeader;
FILE *fp;
static int cnt = 0;
char filePath[255];
memset(&bmpFileHeader, 0, sizeof(BITMAPFILEHEADER));
memset(&bmpInfoHeader, 0, sizeof(BITMAPINFOHEADER));
bmpFileHeader.bfType = 0x4D42;
bmpFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
bmpFileHeader.bfReserved1 = 0;
bmpFileHeader.bfReserved2 = 0;
int biBitCount =32;
int dwBmpSize = ((pImage->width * biBitCount + 31) / 32) * 4 * pImage->height;
printf("size of short:%d\r\n",(int)sizeof(short));
printf("size of int:%d\r\n",(int)sizeof(int));
printf("size of long:%d\r\n",(int)sizeof(long));
printf("dwBmpSize:%d\r\n",(int)dwBmpSize);
printf("BITMAPFILEHEADER:%d\r\n",(int)sizeof(BITMAPFILEHEADER));
printf("BITMAPINFOHEADER:%d\r\n",(int)sizeof(BITMAPINFOHEADER));
bmpFileHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) +  dwBmpSize;

bmpInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
bmpInfoHeader.biWidth = pImage->width;
bmpInfoHeader.biHeight = -1*pImage->height;
bmpInfoHeader.biPlanes = 1;
bmpInfoHeader.biBitCount = biBitCount;
bmpInfoHeader.biSizeImage = 0;
bmpInfoHeader.biCompression = 0;
bmpInfoHeader.biXPelsPerMeter = 0;
bmpInfoHeader.biYPelsPerMeter = 0;
bmpInfoHeader.biClrUsed = 0;
bmpInfoHeader.biClrImportant = 0;

sprintf(filePath, "screenshot.bmp");
fp = fopen(filePath,"wb");

if(fp == NULL)
    return;

fwrite(&bmpFileHeader, sizeof(bmpFileHeader), 1, fp);
fwrite(&bmpInfoHeader, sizeof(bmpInfoHeader), 1, fp);
fwrite(pImage->data, dwBmpSize, 1, fp);
fclose(fp);
}

