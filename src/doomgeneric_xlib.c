#include "doomkeys.h"

#include "doomgeneric.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>

static Display *s_Display = NULL;
static Window s_Window = 0;
static int s_Screen = 0;
static GC s_Gc = 0;
static XImage *s_Image = NULL;
static int s_KeyRepeat = 0; // ANS
#ifdef CMAP256
static Colormap s_Cmap; // ANS
#endif

#define KEYQUEUE_SIZE 16

static unsigned short s_KeyQueue[KEYQUEUE_SIZE];
static unsigned int s_KeyQueueWriteIndex = 0;
static unsigned int s_KeyQueueReadIndex = 0;

static unsigned char convertToDoomKey(unsigned int key)
{
	switch (key)
	{
    case XK_Return:
		key = KEY_ENTER;
		break;
    case XK_Escape:
		key = KEY_ESCAPE;
		break;
    case XK_Left:
		key = KEY_LEFTARROW;
		break;
    case XK_Right:
		key = KEY_RIGHTARROW;
		break;
    case XK_Up:
		key = KEY_UPARROW;
		break;
    case XK_Down:
		key = KEY_DOWNARROW;
		break;
    case XK_Control_L:
    case XK_Control_R:
		key = KEY_FIRE;
		break;
    case XK_space:
		key = KEY_USE;
		break;
    case XK_Shift_L:
    case XK_Shift_R:
		key = KEY_RSHIFT;
		break;
    // ANS note: Alt keys are problematic for many window managers, including
    // CDE on the console.
    case XK_Alt_L:
    case XK_Alt_R:
		key = KEY_LALT;
		break;
#ifdef CMAP256
    // ANS special case. The Command keys don't map to anything in X11, but if
    // we get a keycode of zero, only the Command keys can generate it. This is
    // only true on the console, so don't do this for the remote capable spin.
    case 0:
		key = KEY_LALT;
		break;
#endif
	default:
		key = tolower(key);
		break;
	}

	return key;
}

static void addKeyToQueue(int pressed, unsigned int keyCode)
{
	unsigned char key = convertToDoomKey(keyCode);

	unsigned short keyData = (pressed << 8) | key;

	s_KeyQueue[s_KeyQueueWriteIndex] = keyData;
	s_KeyQueueWriteIndex++;
	s_KeyQueueWriteIndex %= KEYQUEUE_SIZE;
}

// ANS
// Called by atexit
void DG_Quit()
{
	fprintf(stderr, "DG_Quit: restoring settings\n");
	if (s_KeyRepeat)
		XAutoRepeatOn(s_Display);
	/* else XAutoRepeatOff(s_Display); *//* default */
}

void DG_Init()
{
	int blackColor, whiteColor, depth;
    	XSetWindowAttributes attr;

	memset(s_KeyQueue, 0, KEYQUEUE_SIZE * sizeof(unsigned short));

    // window creation

    s_Display = XOpenDisplay(NULL);

    s_Screen = DefaultScreen(s_Display);

    /* int */ blackColor = BlackPixel(s_Display, s_Screen);
    /* int */ whiteColor = WhitePixel(s_Display, s_Screen);

    /* XSetWindowAttributes attr; */
// XXX: this is never actually used
    memset(&attr, 0, sizeof(XSetWindowAttributes));
    attr.event_mask = ExposureMask | KeyPressMask;
    attr.background_pixel = BlackPixel(s_Display, s_Screen);

    /* int */ depth = DefaultDepth(s_Display, s_Screen);

// ANS
#ifdef CMAP256
    {
	XVisualInfo xvis;

	if (!XMatchVisualInfo(s_Display, s_Screen, 8, PseudoColor, &xvis)) {
		fprintf(stderr, "Can't get a 256 colour PseudoColor visual\n");
		exit(1);
	} 
    	s_Cmap = XCreateColormap(s_Display, DefaultRootWindow(s_Display), 
		xvis.visual, AllocAll);
    }
#else
    if (depth != 24) {
	fprintf(stderr, "Expected 24-bit TrueColor visual\n");
	exit(1);
    }
#endif
    s_Window = XCreateSimpleWindow(s_Display, DefaultRootWindow(s_Display), 0, 0, DOOMGENERIC_RESX, DOOMGENERIC_RESY, 0, blackColor, blackColor);

// ANS
#ifdef CMAP256
    XSetWindowColormap(s_Display, s_Window, s_Cmap);
#endif

    XSelectInput(s_Display, s_Window, StructureNotifyMask | KeyPressMask | KeyReleaseMask);

    XMapWindow(s_Display, s_Window);

    s_Gc = XCreateGC(s_Display, s_Window, 0, NULL);

    XSetForeground(s_Display, s_Gc, whiteColor);

    // ANS
    // XkbSetDetectableAutoRepeat(s_Display, 1, 0);
    {
	XKeyboardState ks;
	XGetKeyboardControl(s_Display, &ks);
	atexit(DG_Quit);
	XAutoRepeatOn(s_Display);
    }

    // Wait for the MapNotify event

    while(1)
    {
        XEvent e;
        XNextEvent(s_Display, &e);
        if (e.type == MapNotify)
        {
            break;
        }
    }

    s_Image = XCreateImage(s_Display, DefaultVisual(s_Display, s_Screen), depth, ZPixmap, 0, (char *)DG_ScreenBuffer, DOOMGENERIC_RESX, DOOMGENERIC_RESX, 32, 0);
}


void DG_DrawFrame()
{
    if (s_Display)
    {
        while (XPending(s_Display) > 0)
        {
            XEvent e;
            XNextEvent(s_Display, &e);
            if (e.type == KeyPress)
            {
		// ANS
/*
                KeySym sym = XkbKeycodeToKeysym(s_Display, e.xkey.keycode, 0, 0);
*/
		KeySym sym = XLookupKeysym(&e.xkey, 0);
                //printf("KeyPress:%d sym:%d\n", e.xkey.keycode, sym);

                addKeyToQueue(1, sym);
            }
            else if (e.type == KeyRelease)
            {
		// ANS
/*
                KeySym sym = XkbKeycodeToKeysym(s_Display, e.xkey.keycode, 0, 0);
*/
		KeySym sym = XLookupKeysym(&e.xkey, 0);
                //printf("KeyRelease:%d sym:%d\n", e.xkey.keycode, sym);
                addKeyToQueue(0, sym);
            }
        }

        XPutImage(s_Display, s_Window, s_Gc, s_Image, 0, 0, 0, 0, DOOMGENERIC_RESX, DOOMGENERIC_RESY);

        //XFlush(s_Display);
    }

    //printf("frame\n");
}

void DG_SleepMs(uint32_t ms)
{
    usleep (ms * 1000);
}

uint32_t DG_GetTicksMs()
{
    struct timeval  tp;
    struct timezone tzp;

    gettimeofday(&tp, &tzp);

    return (tp.tv_sec * 1000) + (tp.tv_usec / 1000); /* return milliseconds */
}

int DG_GetKey(int* pressed, unsigned char* doomKey)
{
	if (s_KeyQueueReadIndex == s_KeyQueueWriteIndex)
	{
		//key queue is empty

		return 0;
	}
	else
	{
		unsigned short keyData = s_KeyQueue[s_KeyQueueReadIndex];
		s_KeyQueueReadIndex++;
		s_KeyQueueReadIndex %= KEYQUEUE_SIZE;

		*pressed = keyData >> 8;
		*doomKey = keyData & 0xFF;

		return 1;
	}
}

void DG_SetWindowTitle(const char * title)
{
    if (s_Window)
    {
      XChangeProperty(s_Display, s_Window, XA_WM_NAME, XA_STRING, 8, PropModeReplace, (const unsigned char *)title, strlen(title));
    }
}

// ANS
#ifdef CMAP256

static XColor colors[256];
static int called = 0;
struct color { /* from i_video.h */
    uint32_t b:8;
    uint32_t g:8;
    uint32_t r:8;
    uint32_t a:8;
};

void DG_Upload_Palette(struct color *dgcolors) /* Cribbed from OG Linux DOOM */
{
	int c, i;

	if (!called)
	{
		called++;
		for(i=0; i<256; i++)
		{
			colors[i].pixel = i;
			colors[i].flags = DoRed|DoGreen|DoBlue;
		}
	}

	/* Set X colormap entries */
	for (i=0; i<256; i++) {
		colors[i].red = (dgcolors[i].r << 8) | dgcolors[i].r;
		colors[i].blue = (dgcolors[i].b << 8) | dgcolors[i].b;
		colors[i].green = (dgcolors[i].g << 8) | dgcolors[i].g;
	}

	XStoreColors(s_Display, s_Cmap, colors, 256);
}
#endif

int main(int argc, char **argv)
{
    doomgeneric_Create(argc, argv);

    while(1)
    {
      doomgeneric_Tick(); 
    }

    return 0;
}
