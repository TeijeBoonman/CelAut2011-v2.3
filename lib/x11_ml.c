#include <stdio.h>
#include <stdlib.h>
#include "x11_ml.h"
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "readpng.h"
#include "write_dxf.h"
#include <png.h>

struct coor {
              int x;
              int y;
};

int scale;
int stline=0;
extern int graphics,xfield,yfield,species,layers,thetime,popdyn,spacetime;
extern char ***state;
extern int  *graph;
extern char **title;

static int *hsize,*vsize;
static int phhsize, phvsize;
static int sthsize, stvsize;
static char phtitle[] = "Popdyn plot CA X11";
static char sttitle[] = "Space time plot CA";
static Display **display;
static Display *phdisplay;
static Display *stdisplay;
static char *server = NULL;
static Window *window;
static Window phwindow;
static Window stwindow;
static XImage **image;
static XImage *phimage;
static XImage *stimage;
static GC *windowGC;
static GC phwindowGC;
static GC stwindowGC;
static XEvent *event;
static XEvent phevent;
static XEvent stevent;
static XSizeHints *hint;
static XSizeHints sthint;
static XSizeHints phhint;
int nextposx=0, nextposy=0;
static XWindowAttributes *attributes;
static XWindowAttributes phattributes;
static XWindowAttributes stattributes;
static XSetWindowAttributes *setattributes;
static XSetWindowAttributes phsetattributes;
static XSetWindowAttributes stsetattributes;
static int *screen,*depth,*class,*color_screen;
static int phscreen,phdepth,phclass,phcolor_screen;
static int stscreen,stdepth,stclass,stcolor_screen;
static Visual **visual;
static Visual *phvisual;
static Visual *stvisual;
static unsigned long white, black, foreground, background;
static unsigned long phwhite, phblack, phforeground, phbackground;
static unsigned long stwhite, stblack, stforeground, stbackground;
static XColor *colors;
static XColor *png_colors;

static Colormap new_colormap;
static char **image_data;
static char *phimage_data;
static char *stimage_data;


/* teken procedures */

int Mouse()
{
  int mouse = 0,k;
  for (k=0;k<graphics;k++) {
    while (XEventsQueued(display[k],QueuedAfterFlush) > 0) {
      XNextEvent(display[k], &event[k]);
      switch (event[k].type) {
        case Expose:
          break;
        case ButtonPress:
	mouse   = 1;
        event[k].type=Expose;
        break;
        default:
          break;
      }
    } 
  }
  return (mouse);
}



struct coor GetXYcoo()
{
	struct coor co;
         int mouse = 0,k;
         for (k=0;k<graphics;k++) {
         while (XEventsQueued(display[k],QueuedAfterFlush) > 0) {
           XNextEvent(display[k], &event[k]);
           switch (event[k].type) {
           case Expose:
             break;
            case ButtonPress:
                           co.x = event[k].xbutton.x;
			   co.x /= scale;
			
			   co.y = event[k].xbutton.y;
			   co.y /= scale;
			
                           event[k].type=Expose;
        break;
        default:
          break;
      }
    } 
  }
	
       
	return (co);
}






int Erase() 
{
    int k=graphics;
    XBell(display[0],0);
    printf("Click button to erase\n");
    do { 
      k=++k%(graphics);
      while (XEventsQueued(display[k],QueuedAfterFlush) > 0) {
         XNextEvent(display[k], &(event[k]));
       }
    }
    while (event[k].type != ButtonPress);
    XClearWindow(display[0],window[0]);
}

int CloseGraphics()
{ 
    int k;
    Erase();
   
    for (k=0;k<graphics;++k) {
        XFreeGC(display[k],windowGC[k]); 
        XDestroyWindow(display[k], window[k]);
        XCloseDisplay(display[k]);
     }
}

int ClosePopDyn()
{ 
    XClearWindow(phdisplay,phwindow);
    XFreeGC(phdisplay, phwindowGC);
    XDestroyWindow(phdisplay, phwindow);
    XCloseDisplay(phdisplay);
}

int CloseSpaceTime()
{ 
    XClearWindow(stdisplay,stwindow);
    XFreeGC(stdisplay, stwindowGC);
    XDestroyWindow(stdisplay, stwindow);
    XCloseDisplay(stdisplay);
}


void ReadColorTable()
{  
   int i;
   int p,q,r;
   char name[50];
   FILE *fopen(),*fpc;

   sprintf(name,"default.ctb");
   while ((fpc = fopen(name,"r")) == NULL) {
         fprintf(stdout,"colormap '%s' not found, try again\n",name);
         fprintf(stdout,"name colormap : "); fflush(stdout);
         scanf("%s",name);
   }
   while (fscanf(fpc,"%d",&i) != EOF) {
         fscanf(fpc,"%d %d %d\n",&p,&q,&r);
         colors[i].red=p*255;
         colors[i].green=q*255;
         colors[i].blue=r*255;
   }
   fclose(fpc);
}

void MakeColorMap()
{
   int i,colormap_size,k;
   colormap_size = DisplayCells(display[0], screen[0]); 
   if ((colors = (XColor *)calloc(colormap_size,sizeof(XColor))) == NULL) {
      fprintf(stderr, "No memory for setting up colormap\n");
      exit(1);
   }
   for (i=0; i < colormap_size; i++) {
       colors[i].pixel = i;
       colors[i].flags = DoRed | DoGreen | DoBlue;
   }
   for (k=0;k<graphics;++k)
               XQueryColors(display[k],DefaultColormap(display[k],screen[k]),
                                                 colors,colormap_size);
    
     ReadColorTable(); 
		// Obsolete code for old machines with 256 colors
/*	new_colormap = XCreateColormap(display[0], 
                         RootWindow(display[0],screen[0]),
                         DefaultVisual(display[0],screen[0]),
                         AllocAll);
   for (k=0;k<graphics;++k)
              XStoreColors(display[k],new_colormap,colors,colormap_size);
   
   free(colors);*/
  
	for (k=0;k<graphics;++k) {
		new_colormap = XDefaultColormap(display[k],screen[k]);
		for (i=0; i <= 255 && i < colormap_size; i++)
			XAllocColor(display[k],new_colormap,&colors[i]);
	}
}   
   
void PHMakeColorMap()
{
   int i,colormap_size;

   colormap_size = DisplayCells(phdisplay, phscreen);
   if ((colors = (XColor *)calloc(colormap_size,sizeof(XColor))) == NULL) {
      fprintf(stderr, "No memory for setting up colormap\n");
      exit(1);
   }
   for (i=0; i < colormap_size; i++) {
       colors[i].pixel = i;
       colors[i].flags = DoRed | DoGreen | DoBlue;
   }
     XQueryColors(phdisplay,DefaultColormap(phdisplay,phscreen),
                colors,colormap_size);
     ReadColorTable(); 
     
	// Obsolete code for old machines with 256 colors

	/* new_colormap = XCreateColormap(phdisplay, 
                         RootWindow(phdisplay,phscreen),
                         DefaultVisual(phdisplay,phscreen),
                         AllocAll);
   XStoreColors(phdisplay,new_colormap,colors,colormap_size);
   free(colors);*/
	new_colormap = XDefaultColormap(phdisplay,phscreen);
    for (i=0; i <= 255 && i < colormap_size; i++)
		XAllocColor(phdisplay,new_colormap,&colors[i]);
}

int OpenGraphics(int w, int h)
{
 int i,k;
 char **argv;
  display = (Display **)calloc ((graphics),sizeof(Display *));
  visual = (Visual **)calloc ((graphics),sizeof(Visual *));
  image = (XImage **)calloc ((graphics),sizeof(XImage *));
  event = (XEvent *)calloc ((graphics),sizeof(XEvent));
  hsize = (int *)calloc ((graphics),sizeof(int));
  vsize = (int *)calloc ((graphics),sizeof(int));
  window = (Window *)calloc ((graphics),sizeof(Window));
  windowGC = (GC *)calloc ((graphics),sizeof(GC));
  hint = (XSizeHints *)calloc ((graphics),sizeof(XSizeHints));
  attributes = (XWindowAttributes *)calloc ((graphics),sizeof(XWindowAttributes));
  setattributes = (XSetWindowAttributes *)calloc ((graphics),sizeof(XSetWindowAttributes));
  screen = (int *)calloc ((graphics),sizeof(int));
  depth = (int *)calloc ((graphics),sizeof(int));
  class = (int *)calloc ((graphics),sizeof(int));
  color_screen = (int *)calloc ((graphics),sizeof(int));
  for (k=0;k<graphics;++k) {
  hsize[k] = w*scale;
  vsize[k] = h*scale;
  display[k] = XOpenDisplay(server);
  if (!display[k]) {
    printf("Failed to open display connection to %s\n",
      XDisplayName(server));
    exit(0);
  }
  screen[k] = DefaultScreen(display[k]);
  depth[k] = XDefaultDepth(display[k], screen[k]);
  visual[k] = XDefaultVisual(display[k], screen[k]);
  class[k] = visual[k]->class;
  if (depth[k] == 1) color_screen[k] = 0;
  else if (class[k] == GrayScale) color_screen[k] = 0;
  else if (class[k] == PseudoColor) color_screen[k] = 1;
  else if (class[k] == DirectColor) color_screen[k] = 1;
  else if (class[k] == TrueColor) color_screen[k] = 1;
  else if (class[k] == StaticColor) color_screen[k] = 1;
  else if (class[k] == StaticGray) color_screen[k] = 1;
  black = BlackPixel(display[k], screen[k]);
  white = WhitePixel(display[k], screen[k]);
  if (color_screen[k]) {
    background = black;
    foreground = white;
  } else {
    background = white;
    foreground = black;
  }
  hint[k].x = nextposx; hint[k].y = nextposy;
  hint[k].width = hsize[k]; hint[k].height = vsize[k];
  hint[k].flags = PPosition | PSize;
	  nextposx += hint[k].width;
	  
  }
  MakeColorMap(); 
  for (k=0;k<graphics;++k) {
  setattributes[k].colormap = new_colormap;
  setattributes[k].background_pixel = WhitePixel(display[k],screen[k]);
  setattributes[k].border_pixel = BlackPixel(display[k],screen[k]);
  window[k] = XCreateWindow(display[k],
           DefaultRootWindow(display[k]), hint[k].x, hint[k].y,
           hint[k].width, hint[k].height, 5 , depth[k],
           InputOutput, visual[k],
           CWColormap | CWBackPixel | CWBorderPixel,
           &(setattributes[k]));
  

/* window = XCreateSimpleWindow(display[k],
    DefaultRootWindow(display[k]),
    hint[k].x, hint[k].y, hint[k].width, hint[k].height, 5,
    foreground[k], background[k]);
*/
  XSetStandardProperties(display[k], window[k], title[k], title[k],
    None, argv, 0, &(hint[k]));
  XGetWindowAttributes(display[k],window[k],&(attributes[k]));
  XSelectInput(display[k], window[k], ButtonPressMask | ExposureMask);
  if (DoesBackingStore(DefaultScreenOfDisplay(display[k]))) {
    setattributes[k].backing_store = WhenMapped;
    XChangeWindowAttributes(display[k],window[k],CWBackingStore,&(setattributes[k]));

  }
  if (depth[k] != 24) {
    fprintf(stderr,"Only full color displays supported.\n");
    exit(0);
  }
  windowGC[k] = XCreateGC(display[k], window[k], 0, 0);
  XSetBackground(display[k], windowGC[k], background);
  XSetForeground(display[k], windowGC[k], foreground);

  XSelectInput(display[k], window[k], ButtonPressMask | ExposureMask);
  
  XMapRaised(display[k], window[k]);
  XNextEvent(display[k], &event[k]); 
 }
  image_data = (char **)calloc(graphics+1,sizeof(char *));
  for (k=0;k<graphics;++k) {
  image_data[k] = (char *)calloc(xfield*yfield*scale*scale,sizeof(char)*6);
  if (image_data[k] == NULL) printf("Error in memory allocation\n");
  }
  for (k=0;k<graphics;++k) {
  image[k] = XCreateImage(display[k], visual[k], depth[k], ZPixmap,
    0, image_data[k], xfield*scale, yfield*scale, 32, 0); 
   }
 
  return (0);
}

int OpenPopDyn() 
{
 int i;
 char **argv;

  phhsize = 200;
  phvsize = 200;
  phdisplay = XOpenDisplay(server);
  if (!phdisplay) {
    printf("Failed to open display connection to %s\n",
      XDisplayName(server));
    exit(0);
  }
  phscreen = DefaultScreen(phdisplay);
  phdepth = XDefaultDepth(phdisplay, phscreen);
  phvisual = XDefaultVisual(phdisplay, phscreen);
  phclass = phvisual->class;
  if (phdepth == 1) phcolor_screen = 0;
  else if (phclass == GrayScale) phcolor_screen = 0;
  else if (phclass == PseudoColor) phcolor_screen = 1;
  else if (phclass == DirectColor) phcolor_screen = 1;
  else if (phclass == TrueColor) phcolor_screen = 1;
  else if (phclass == StaticColor) phcolor_screen = 1;
  else if (phclass == StaticGray) phcolor_screen = 1;
  phblack = BlackPixel(phdisplay, phscreen);
  phwhite = WhitePixel(phdisplay, phscreen);
  if (phcolor_screen) {
    phbackground = phblack;
    phforeground = phwhite;
  } else {
    phbackground = phwhite;
    phforeground = phblack;
  }
  phhint.x = nextposx; phhint.y = nextposy;
  phhint.width = phhsize; phhint.height = phvsize;
  phhint.flags = PPosition | PSize;
	nextposx += phhint.width; nextposy = 0;
  PHMakeColorMap(); 
  phsetattributes.colormap = new_colormap;
  phsetattributes.background_pixel = WhitePixel(phdisplay,phscreen);
  phsetattributes.border_pixel = BlackPixel(phdisplay,phscreen);
  phwindow = XCreateWindow(phdisplay,
           DefaultRootWindow(phdisplay), phhint.x, phhint.y,
           phhint.width, phhint.height, 5 , phdepth,
           InputOutput, phvisual,
           CWColormap | CWBackPixel | CWBorderPixel,
           &phsetattributes);
  


  XSetStandardProperties(phdisplay, phwindow, phtitle, phtitle,
    None, argv, 0, &phhint);
  XGetWindowAttributes(phdisplay,phwindow,&phattributes);
  XSelectInput(phdisplay, phwindow, ButtonPressMask | ExposureMask);
  if (DoesBackingStore(DefaultScreenOfDisplay(phdisplay))) {
    phsetattributes.backing_store = WhenMapped;
    XChangeWindowAttributes(phdisplay,phwindow,CWBackingStore,&phsetattributes);
  }

  if (phdepth != 24) {
    fprintf(stderr,"Screen depth ph %d not supported\n", phdepth);
    exit(0);
  }
  phwindowGC = XCreateGC(phdisplay, phwindow, 0, 0);
  XSetBackground(phdisplay, phwindowGC, phbackground);
  XSetForeground(phdisplay, phwindowGC, phforeground);

  XSelectInput(phdisplay, phwindow, ButtonPressMask | ExposureMask);
  
  XMapRaised(phdisplay, phwindow);
   
	  phimage_data = (char *)calloc(phhsize*phvsize,sizeof(char)*6);
  if (phimage_data == NULL) printf("Error in memory allocation\n");
  phimage = XCreateImage(phdisplay, phvisual, phdepth, ZPixmap,
    0, phimage_data, phhsize, phvsize, 32, 0); 


  return (0);
}

int OpenSpaceTime(int axt)
{
 int i;
 char **argv;

  sthsize = xfield*scale;
  stvsize = axt*scale;
  stdisplay = XOpenDisplay(server);
  if (!stdisplay) {
    printf("Failed to open display connection to %s\n",
      XDisplayName(server));
    exit(0);
  }
  stscreen = DefaultScreen(stdisplay);
  stdepth = XDefaultDepth(stdisplay, stscreen);
  stvisual = XDefaultVisual(stdisplay, stscreen);
  stclass = stvisual->class;
  if (stdepth == 1) stcolor_screen = 0;
  else if (stclass == GrayScale) stcolor_screen = 0;
  else if (stclass == PseudoColor) stcolor_screen = 1;
  else if (stclass == DirectColor) stcolor_screen = 1;
  else if (stclass == TrueColor) stcolor_screen = 1;
  else if (stclass == StaticColor) stcolor_screen = 1;
  else if (stclass == StaticGray) stcolor_screen = 1;
  stblack = BlackPixel(stdisplay, stscreen);
  stwhite = WhitePixel(stdisplay, stscreen);
  if (color_screen) {
    stbackground = stblack;
    stforeground = stwhite;
  } else {
    stbackground = stwhite;
    stforeground = stblack;
  }
  sthint.x = nextposx; sthint.y = nextposy;
  sthint.width = sthsize; sthint.height = stvsize;
  sthint.flags = PPosition | PSize;
	nextposx += sthint.width;
	nextposy = 0;
  /* STMakeColorMap();  */
  stsetattributes.colormap = new_colormap;
  stsetattributes.background_pixel = WhitePixel(stdisplay,stscreen);
  stsetattributes.border_pixel = BlackPixel(stdisplay,stscreen);
  stwindow = XCreateWindow(stdisplay,
           DefaultRootWindow(stdisplay), sthint.x, sthint.y,
           sthint.width, sthint.height, 5 , stdepth,
           InputOutput, stvisual,
           CWColormap | CWBackPixel | CWBorderPixel,
           &stsetattributes);
  

/* window = XCreateSimpleWindow(display,
    DefaultRootWindow(display),
    hint.x, hint.y, hint.width, hint.height, 5,
    foreground, background);
*/
  XSetStandardProperties(stdisplay, stwindow, sttitle, sttitle,
    None, argv, 0, &sthint);
  XGetWindowAttributes(stdisplay,stwindow,&stattributes);
  XSelectInput(stdisplay, stwindow, ButtonPressMask | ExposureMask);
  if (DoesBackingStore(DefaultScreenOfDisplay(stdisplay))) {
    stsetattributes.backing_store = WhenMapped;
    XChangeWindowAttributes(stdisplay,stwindow,CWBackingStore,&stsetattributes);
  }

  if (stdepth != 24) {
    fprintf(stderr,"Screen depth st %d not supported\n", stdepth);
    exit(0);
  }
  stwindowGC = XCreateGC(stdisplay, stwindow, 0, 0);
  XSetBackground(stdisplay, stwindowGC, stbackground);
  XSetForeground(stdisplay, stwindowGC, stforeground);

  XSelectInput(stdisplay, stwindow, ButtonPressMask | ExposureMask);
  
  XMapRaised(stdisplay, stwindow);
   
  stimage_data = (char *)calloc(sthsize*stvsize,sizeof(char)*6);
  if (stimage_data == NULL) printf("Error in memory allocation\n");
  stimage = XCreateImage(stdisplay, stvisual, stdepth, ZPixmap,
    0, stimage_data, sthsize, stvsize, 32, 0); 
  return (0);
}




int DrawField2()
{
    int x,y,mx,my,k,i;
    long offset=0;
    int p;
    int kk;
    kk=0;
    for (p=0;p<graphics;++p) {
        if (graph[p]==-1) kk=p;  /* in what window the field should be drawn? */
    }
    
    if (kk>=graphics) kk=graphics-1;
    if (kk<0) kk=0;
    for (y=1;y <= yfield; y++)  {
        for (x=1; x <= xfield;x++) {
            i=0;
            for (k=1;k<layers;++k) {
                i *= species;
                i += state[k][x][y];
            }
            i %= 256;
            for (my=0; my < scale; my++)
                for (mx=0; mx < scale; mx++)
                    //  (image[kk]->data)[(offset+ (x-1)*scale+mx + my*xfield*scale)] = i;
                    XPutPixel(image[kk],(x-1)*scale+mx, (y-1)*scale+my,colors[i].pixel);
        }
        //offset +=xfield*scale*scale;
    }
    XPutImage(display[kk],window[kk],windowGC[kk],image[kk],0,0,0,0,image[kk]->width,image[kk]->height);
}

int DrawField()
{
    int x,y,mx,my,k,i;
    long offset=0;
    int p;
    int kk;
    kk=0;
    for (p=0;p<graphics;++p) {
           if (graph[p]==-1) kk=p;  /* in what window the field should be drawn? */
    }
    
    if (kk>=graphics) kk=graphics-1;
    if (kk<0) kk=0;
     for (y=1;y <= yfield; y++)  {
         for (x=1; x <= xfield;x++) { 
             i=0; 
             for (k=0;k<layers;++k) {
	         i *= species;
	         i += state[k][x][y];
             }
             i %= 256;
             for (my=0; my < scale; my++) 
             for (mx=0; mx < scale; mx++) 
               //  (image[kk]->data)[(offset+ (x-1)*scale+mx + my*xfield*scale)] = i;
				 XPutPixel(image[kk],(x-1)*scale+mx, (y-1)*scale+my,colors[i].pixel);
          }
          //offset +=xfield*scale*scale;
     }
     XPutImage(display[kk],window[kk],windowGC[kk],image[kk],0,0,0,0,image[kk]->width,image[kk]->height);
}

int DrawPopDyn(int layx, int sx, int layy, int sy, int axx, int axy, int popdyncol)
{
    int x,y;
    int px=0,py=0;
    
    int p;
    int plotx,ploty;
    
    if (axx==0) axx=xfield*yfield;
    if (axy==0) axy=xfield*yfield;
    if ((sx==0)&&(thetime%axx==0)) ClearPopDyn(); 
     for (y=1;y <= yfield; y++) 
         for (x=1; x <= xfield;x++) {
           if (state[layx][x][y]==sx) px++; 
	   if (state[layy][x][y]==sy) py++;
         }
     if (sx>0) plotx=(int)(((float)px/axx)*phhsize);
       else plotx=(int)(((float)(thetime%axx)/axx)*phvsize);
     ploty=phvsize-(int)(((float)py/axy)*phvsize);
     if (plotx>=phhsize) plotx=phhsize-1;
      else if (plotx<0) plotx=0;
     
     if (ploty>=phvsize) ploty=phvsize-1;
      else if (ploty<0) ploty=0;

     //(phimage->data)[ploty*phhsize+plotx] = popdyncol;
	XPutPixel(phimage,plotx, ploty,colors[popdyncol].pixel); 
	XPutImage(phdisplay,phwindow,phwindowGC,phimage,0,0,0,0,phimage->width,phimage->height);
}

int DrawSpaceTime(int lay,int ypos)
{
    int x,mx,my;
    long offset=0;
    int p,i,k;
    if ((stline)>(stvsize/scale)) stline=(stvsize/scale)-1;
    if (stline<0) stline=0;
    if (lay>(layers-1)) lay=-1;
    offset=(stline)*xfield*scale*scale;
     
       
         for (x=1; x <= xfield;x++) {
             if (lay<0) {
                 i=0; 
                 for (k=0;k<layers;++k) {
	            i *= species;
	            i += state[k][x][ypos];
                 }
                 i %= 256;
             }
             else
                 i=state[lay][x][ypos];
             for (my=0; my < scale; my++) 
				 for (mx=0; mx < scale; mx++) 
          //        (stimage->data)[(offset+ (x-1)*scale+mx + my*xfield*scale)] = i;
					 XPutPixel(stimage,(x-1)*scale+mx, stline*scale+my,colors[i].pixel);

				 }
        
     stline=++stline%spacetime; 
     
     XPutImage(stdisplay,stwindow,stwindowGC,stimage,0,0,0,0,stimage->width,stimage->height);
}


int ClearPopDyn()
{
    int x,y;
    long offset=0;
    int p;

	for (x=0;x<phhsize;x++)
		for (y=0;y<phvsize;y++) 
		//<= phhsize*phvsize-1; i++)
       // (phimage->data)[i] = 0;
		 XPutPixel(phimage,x, y,colors[0].pixel);
    XPutImage(phdisplay,phwindow,phwindowGC,phimage,0,0,0,0,phimage->width,phimage->height);
}

int ClearSpaceTime()
{
    int i;
    long offset=0;
    int p;

     for (i=0;i <= sthsize*stvsize-1; i++)
        (stimage->data)[i] = 0;
    
    XPutImage(stdisplay,stwindow,stwindowGC,stimage,0,0,0,0,stimage->width,stimage->height);
}

int DrawLayer(int kk)
{
    int k,x,y,mx,my;
    long offset=0;
    int p;
    k=0; 
    for (p=0;p<graphics;++p) {
           if (graph[p]==kk) k=p;  /* in what window the layer should be drawn? */
    }

    if (k>=graphics) k=graphics-1;
    if (k<0) k=0;
     for (y=1;y <= yfield; y++)  {
         for (x=1; x <= xfield;x++) {  
             for (my=0; my < scale; my++) 
             for (mx=0; mx < scale; mx++) 
                 //(image[k]->data)[(offset+ (x-1)*scale+mx + my*xfield*scale)] = state[kk][x][y];
				 XPutPixel(image[k],(x-1)*scale+mx, (y-1)*scale+my,colors[state[kk][x][y]].pixel);
          }
		 //offset +=xfield*scale*scale;
     }
     XPutImage(display[k],window[k],windowGC[k],image[k],0,0,0,0,image[k]->width,image[k]->height);
}



 void DrawLayerField()
 {
      int k;

      printf("\nlayer : ");
      scanf("%d",&k);
      if (k>=0 && k < layers)
         DrawLayer(k);
      else 
        printf("choose between 0 and %d\n",layers-1);
 }


void WriteLayer(char *fname, int layer) {
	
	
	// NB quality is dummy parameter for compatibility with
	// QtGraphics. No need to supply it. This method can only write PNG.
	
	// based on code borrowed from Cash2003, png.c
	
	
	if (strcasestr(fname,"png")==NULL) {
		fprintf(stderr,"WriteField: Only PNG writing is implemented. Please use extension .png\n");
		fprintf(stderr,"Current filename '%s' will not do.\n", fname);
		exit(1);
	}
	
	fprintf(stderr,"Writing PNG picture '%s'\n",fname);
	
	
	int i,j;
	
	FILE *fp;
	fp = fopen(fname,"wb");
	if (fp==0) {
		perror(fname);
		fprintf(stderr,"WriteField: File error\n");
	}
	png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
												  (png_voidp)NULL,
												  (png_error_ptr)NULL,
												  (png_error_ptr)NULL);
	png_infop info_ptr = png_create_info_struct (png_ptr);
	png_init_io(png_ptr, fp);
	png_set_IHDR(png_ptr, info_ptr, scale*xfield, scale*yfield,
				 8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
				 PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
	png_write_info(png_ptr,info_ptr);
	
	// data to hold true colour image
	unsigned char *png_image =
    (unsigned char *)malloc(3*scale*scale*xfield*yfield*sizeof(unsigned char));
	
	MakePngColorMap();
    
    int x,y,mx,my;
	for (y=1;y <= yfield; y++)  {
		for (my=0; my < scale; my++) {
			for (x=1; x <= xfield;x++) {
                
				for (mx=0; mx < scale; mx++) {
					
					XColor col;
					col=png_colors[state[layer][x][y]];
					//fprintf(stderr,"[ %d: %d, %d, %d ]",state[x][y], col.red,col.green,col.blue );
					png_image[ ((x-1)*scale+mx)*3 + ((y-1)*scale+my)*xfield*3 ] = col.red/256;
					png_image[ ((x-1)*scale+mx)*3 + ((y-1)*scale+my)*xfield*3 + 1] = col.green/256;
					png_image[ ((x-1)*scale+mx)*3 + ((y-1)*scale+my)*xfield*3 + 2] = col.blue/256;
				}
			}
			png_bytep ptr = png_image + ((y-1)*scale+my)*3*xfield;
            
			png_write_rows(png_ptr, &ptr, 1);
		}
	}
    
    
	png_write_end(png_ptr, info_ptr);
    png_destroy_write_struct(&png_ptr,(png_infopp)NULL);
    free(png_image);
	free(png_colors);
    fclose(fp);
}

void ReadPngColorTable()
{  
   int i,k=0;
   int p,q,r;
   char name[50];
   FILE *fopen(),*fpc;

   sprintf(name,"default.ctb");
   while ((fpc = fopen(name,"r")) == NULL) {
         fprintf(stdout,"colormap '%s' not found, try again\n",name);
         fprintf(stdout,"name colormap : "); fflush(stdout);
         scanf("%s",name);
   }
   while (fscanf(fpc,"%d",&i) != EOF) {
         fscanf(fpc,"%d %d %d\n",&p,&q,&r);
         png_colors[i].red=p*255;
         png_colors[i].green=q*255;
         png_colors[i].blue=r*255;
   }
   fclose(fpc);
}
void MakePngColorMap()
{
   int i,colormap_size;

   colormap_size = 256;
   if ((png_colors = (XColor *)calloc(colormap_size,sizeof(XColor))) == NULL) {
      fprintf(stderr, "No memory for setting up png colormap\n");
      exit(1);
   }
   for (i=0; i < colormap_size; i++) {
       png_colors[i].pixel = i;
       png_colors[i].flags = DoRed | DoGreen | DoBlue;
   }
     ReadPngColorTable();

}    

void WriteField(char *fname) {
	
	
	// NB quality is dummy parameter for compatibility with
	// QtGraphics. No need to supply it. This method can only write PNG.
	
	// based on code borrowed from Cash2003, png.c
	
	
	if (strcasestr(fname,"png")==NULL) {
		fprintf(stderr,"WriteField: Only PNG writing is implemented. Please use extension .png\n");
		fprintf(stderr,"Current filename '%s' will not do.\n", fname);
		exit(1);
	}
	
	fprintf(stderr,"Writing PNG picture '%s'\n",fname);
	
	
	int i,j;
	
	FILE *fp;
	fp = fopen(fname,"wb");
	if (fp==0) {
		perror(fname);
		fprintf(stderr,"WriteField: File error\n");
	}
	png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
												  (png_voidp)NULL,
												  (png_error_ptr)NULL,
												  (png_error_ptr)NULL);
	png_infop info_ptr = png_create_info_struct (png_ptr);
	png_init_io(png_ptr, fp);
	png_set_IHDR(png_ptr, info_ptr, scale*xfield, scale*yfield,
				 8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
				 PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
	png_write_info(png_ptr,info_ptr);
	
	// data to hold true colour image
	unsigned char *png_image =
    (unsigned char *)malloc(3*scale*scale*xfield*yfield*sizeof(unsigned char));
	
	MakePngColorMap();
    
    int x,y,mx,my,c,k;
	for (y=1;y <= yfield; y++)  {
		for (my=0; my < scale; my++) {
			for (x=1; x <= xfield;x++) {
                c=0;
                for (k=0;k<layers;++k) {
                    c *= species;
                    c += state[k][x][y];
                }
                c %= 256;

				for (mx=0; mx < scale; mx++) {
					
					XColor col;
					col=png_colors[c];
					//fprintf(stderr,"[ %d: %d, %d, %d ]",state[x][y], col.red,col.green,col.blue );
					png_image[ ((x-1)*scale+mx)*3 + ((y-1)*scale+my)*xfield*3 ] = col.red/256;
					png_image[ ((x-1)*scale+mx)*3 + ((y-1)*scale+my)*xfield*3 + 1] = col.green/256;
					png_image[ ((x-1)*scale+mx)*3 + ((y-1)*scale+my)*xfield*3 + 2] = col.blue/256;
				}
			}
			png_bytep ptr = png_image + ((y-1)*scale+my)*3*xfield;
            
			png_write_rows(png_ptr, &ptr, 1);
		}
	}
    
    
	png_write_end(png_ptr, info_ptr);
    png_destroy_write_struct(&png_ptr,(png_infopp)NULL);
    free(png_image);
	free(png_colors);
    fclose(fp);
}

void WriteFieldDXFLayerOffset(char *fname, int layer_offset) {
    
    printf("Memory init\n");
    int **field=(int **)malloc((xfield)*sizeof(int *));
    field[0]=(int *)malloc((xfield)*(yfield)*sizeof(int));
    for (int i=1;i<(xfield);i++) {
        field[i]=field[i-1]+(yfield);
    }
    
    
    
    int x,y,mx,my,c,k;
    printf("Coping\n");
    for (y=1;y <= yfield; y++)  {
        printf("[%d, %d--]\n", x-1,y-1);
            for (x=1; x <= xfield;x++) {
                c=0;
                for (k=layer_offset;k<layers;++k) {
                    c *= species;
                    c += state[k][x][y];
                }
                c %= 10;
                if (c>1) {
                    c+=2;
                }
                printf("[%d, %d]\n", x-1,y-1);
                field[x-1][y-1]=c;
            }
        
    }
    
    write_dxf(fname, field, xfield-2, yfield-2);
}


void WriteFieldLayerOffset(char *fname, int layer_offset) {
    
    
    // NB quality is dummy parameter for compatibility with
    // QtGraphics. No need to supply it. This method can only write PNG.
    
    // based on code borrowed from Cash2003, png.c
    
    
    if (strcasestr(fname,"png")==NULL) {
        fprintf(stderr,"WriteField: Only PNG writing is implemented. Please use extension .png\n");
        fprintf(stderr,"Current filename '%s' will not do.\n", fname);
        exit(1);
    }
    
    fprintf(stderr,"Writing PNG picture '%s'\n",fname);
    
    
    int i,j;
    
    FILE *fp;
    fp = fopen(fname,"wb");
    if (fp==0) {
        perror(fname);
        fprintf(stderr,"WriteField: File error\n");
    }
    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
                                                  (png_voidp)NULL,
                                                  (png_error_ptr)NULL,
                                                  (png_error_ptr)NULL);
    png_infop info_ptr = png_create_info_struct (png_ptr);
    png_init_io(png_ptr, fp);
    png_set_IHDR(png_ptr, info_ptr, scale*xfield, scale*yfield,
                 8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
    png_write_info(png_ptr,info_ptr);
    
    // data to hold true colour image
    unsigned char *png_image =
    (unsigned char *)malloc(3*scale*scale*xfield*yfield*sizeof(unsigned char));
    
    int x,y,mx,my,c,k;
    for (y=1;y <= yfield; y++)  {
        for (my=0; my < scale; my++) {
            for (x=1; x <= xfield;x++) {
                c=0;
                for (k=layer_offset;k<layers;++k) {
                    c *= species;
                    c += state[k][x][y];
                }
                c %= 256;
                
                for (mx=0; mx < scale; mx++) {
                    
                    XColor col;
                    col=colors[c];
                    //fprintf(stderr,"[ %d: %d, %d, %d ]",state[x][y], col.red,col.green,col.blue );
                    png_image[ ((x-1)*scale+mx)*3 + ((y-1)*scale+my)*xfield*3 ] = col.red/256;
                    png_image[ ((x-1)*scale+mx)*3 + ((y-1)*scale+my)*xfield*3 + 1] = col.green/256;
                    png_image[ ((x-1)*scale+mx)*3 + ((y-1)*scale+my)*xfield*3 + 2] = col.blue/256;
                }
            }
            png_bytep ptr = png_image + ((y-1)*scale+my)*3*xfield;
            
            png_write_rows(png_ptr, &ptr, 1);
        }
    }
    
    
    png_write_end(png_ptr, info_ptr);
    png_destroy_write_struct(&png_ptr,(png_infopp)NULL);
    free(png_image);
	free(png_colors);
    fclose(fp);
}

#include <stdarg.h>
void abort_(const char * s, ...)
{
    va_list args;
    va_start(args, s);
    vfprintf(stderr, s, args);
    fprintf(stderr, "\n");
    va_end(args);
    abort();
}


void ReadLayerPNG(char *file_name, int layer, int color) {

    int x, y;
    
    int width, height, rowbytes;
    png_byte color_type;
    png_byte bit_depth;
    
    png_structp png_ptr;
    png_infop info_ptr;
    int number_of_passes;
    png_bytep * row_pointers;
    
    #define PROGNAME "ReadLayerPNG"
        unsigned char header[8];    // 8 is the maximum size that can be checked
        
        /* open file and test for it being a png */
        FILE *fp = fopen(file_name, "rb");
        if (!fp)
            abort_(PROGNAME ": File %s could not be opened for reading", file_name);
        fread(header, 1, 8, fp);
        if (png_sig_cmp(header, 0, 8))
            abort_(PROGNAME ": File %s is not recognized as a PNG file", file_name);
        
        
        /* initialize stuff */
        png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
        
        if (!png_ptr)
            abort_(PROGNAME ": png_create_read_struct failed");
        
        info_ptr = png_create_info_struct(png_ptr);
        if (!info_ptr)
            abort_(PROGNAME ": png_create_info_struct failed");
        
        if (setjmp(png_jmpbuf(png_ptr)))
            abort_(PROGNAME ": Error during init_io");
        
        png_init_io(png_ptr, fp);
        png_set_sig_bytes(png_ptr, 8);
        
        png_read_info(png_ptr, info_ptr);
        
        width = png_get_image_width(png_ptr, info_ptr);
        height = png_get_image_height(png_ptr, info_ptr);
        color_type = png_get_color_type(png_ptr, info_ptr);
        bit_depth = png_get_bit_depth(png_ptr, info_ptr);
        
        number_of_passes = png_set_interlace_handling(png_ptr);
        png_read_update_info(png_ptr, info_ptr);
        
        
        /* read file */
        if (setjmp(png_jmpbuf(png_ptr)))
            abort_(PROGNAME ": Error during read_image");
    
    printf("Done reading file\n");
        row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * height);
        
        if (bit_depth == 16)
            rowbytes = width*8;
        else
            rowbytes = width*4;
        
        for (y=0; y<height; y++)
            row_pointers[y] = (png_byte*) malloc(rowbytes);
    
    
    /* Expand any grayscale, RGB, or palette images to RGBA */
  //  printf("png_set_expand\n");
   // png_set_expand(png_ptr);
    
    /* Reduce any 16-bits-per-sample images to 8-bits-per-sample */
    //printf("png_set_strip\n");
   // png_set_strip_16(png_ptr);
    
        png_read_image(png_ptr, row_pointers);
        
        fclose(fp);
    
    
    // Process file
 
    
    int channels = png_get_channels(png_ptr, info_ptr);
    for (y=0; y<height; y++) {
        png_byte* row = row_pointers[y];
        for (x=0; x<width; x++) {
            png_byte* ptr = &(row[x*channels]);
            //printf("Pixel at position [ %d - %d ] has RGBA values: %d - %d - %d - %d\n",
             //      x, y, ptr[0], ptr[1], ptr[2], ptr[3]);
            //
            //  ((x-1)*scale+mx)*3 + ((y-1)*scale+my)*xfield*3
            
            
            if (!color) {
                /* dark pixels indicate CA state of 1; white pixels are CA state of 0 */
                int gray = (ptr[0]+ptr[1]+ptr[2])/3;
                int xx=(x/scale)+1, yy=(y/scale)+1;
                //fprintf(stderr,"[%d, %d]",xx,yy);
                if (xx<xfield && yy<yfield) {
                    state[layer][xx][yy]=(gray<250)?1:0;
                }
            } else {
               /* color pixels: ad hoc, currently red=1, green=2, blue=3 */
                int red = ptr[0];
                int green = ptr[1];
                int blue = ptr[2];
                
                /* Filter: identify brightest color of the three */
                int c_state = 0;
                if (red+green+blue < 600) { // color shall not be nearly white
                if (red > green && red > blue) { c_state = 1; }
                if (green > red && green > blue) { c_state = 2; }
                if (blue > red && blue > green) { c_state =3; }
                }
                int xx=(x/scale)+1, yy=(y/scale)+1;
                if (xx<xfield && yy<yfield) {
                    state[layer][xx][yy]=c_state;
                }
          
            }
            /* perform whatever modifications needed, for example to set red value to 0 and green value to the blue one:
             ptr[0] = 0;
             ptr[1] = ptr[2]; */
        }
    }


}


void ReadLayerPNGBW(char *file_name, int layer) {
    
    ReadLayerPNG(file_name, layer, 0);
    
}


void ReadLayerPNGColor(char *file_name, int layer) {
    
    ReadLayerPNG(file_name, layer, 1);
    
}
























