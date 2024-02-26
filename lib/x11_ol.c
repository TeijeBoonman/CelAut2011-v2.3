#include <stdio.h>
#include <stdlib.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <png.h> 
int scale,stline=0;
extern int xfield,yfield,species,thetime,popdyn,spacetime;
extern char **state;

static int hsize, vsize;
static int phhsize, phvsize;
static int sthsize, stvsize;
static char title[] = "CA X11";
static char phtitle[] = "Popdyn plot CA X11";
static char sttitle[] = "Space time plot CA";
static Display *display;
static Display *phdisplay;
static Display *stdisplay;
static char *server = NULL;
static Window window;
static Window phwindow;
static Window stwindow;
static XImage *image;
static XImage *phimage;
static XImage *stimage;
static GC windowGC;
static GC phwindowGC;
static GC stwindowGC;
static XEvent event;
static XEvent phevent;
static XEvent stevent;
static XSizeHints hint;
static int nextposx = 0, nextposy=0;
static XSizeHints sthint;
static XSizeHints phhint;
static XWindowAttributes attributes;
static XWindowAttributes phattributes;
static XWindowAttributes stattributes;
static XSetWindowAttributes setattributes;
static XSetWindowAttributes phsetattributes;
static XSetWindowAttributes stsetattributes;
static int screen,depth,class,color_screen;
static int phscreen,phdepth,phclass,phcolor_screen;
static int stscreen,stdepth,stclass,stcolor_screen;
static Visual *visual;
static Visual *phvisual;
static Visual *stvisual;
static unsigned long white, black, foreground, background;
static unsigned long phwhite, phblack, phforeground, phbackground;
static unsigned long stwhite, stblack, stforeground, stbackground;
static XColor *colors;

static Colormap new_colormap;
static char *image_data;
static char *phimage_data;
static char *stimage_data;


/* teken procedures */

int Mouse()
{
  int mouse = 0;
  while (XEventsQueued(display,QueuedAfterFlush) > 0) {
      XNextEvent(display, &event);
      switch (event.type) {
        case Expose:
          break;
        case ButtonPress:
	mouse   = 1;
          break;
        default:
          break;
      }
  }
  return (mouse);
}

int Erase() 
{
    XBell(display,0);
    printf("Click button to erase\n");
    do
      XNextEvent(display, &event);
    while (event.type != ButtonPress);
    XClearWindow(display,window);
     
}



int CloseGraphics()
{ 
    Erase();
    XFreeGC(display, windowGC);
    XDestroyWindow(display, window);
    XCloseDisplay(display);
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
         colors[i].red=p*255;
         colors[i].green=q*255;
         colors[i].blue=r*255;
   }
   fclose(fpc);
}

void MakeColorMap()
{
   int i,colormap_size;

   colormap_size = DisplayCells(display, screen);
   if ((colors = (XColor *)calloc(colormap_size,sizeof(XColor))) == NULL) {
      fprintf(stderr, "No memory for setting up colormap\n");
      exit(1);
   }
   for (i=0; i < colormap_size; i++) {
       colors[i].pixel = i;
       colors[i].flags = DoRed | DoGreen | DoBlue;
   }
     XQueryColors(display,DefaultColormap(display,screen),
                colors,colormap_size);
     ReadColorTable(); 
	// Obsolete code for old machines with 256 colors
	/*
		new_colormap = XCreateColormap(display, 
                         RootWindow(display,screen),
                         DefaultVisual(display,screen),
                         AllocAll);
	
	XStoreColors(display,new_colormap,colors,colormap_size);
	
   free(colors);
	 */
	new_colormap = XDefaultColormap(display,screen);
    for (i=0; i <= 255 && i < colormap_size; i++)
		XAllocColor(display,new_colormap,&colors[i]);

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

/*     new_colormap = XCreateColormap(phdisplay, 
                         RootWindow(phdisplay,phscreen),
                         DefaultVisual(phdisplay,phscreen),
                         AllocAll);
   XStoreColors(phdisplay,new_colormap,colors,colormap_size);
   free(colors);
 */
	new_colormap = XDefaultColormap(phdisplay,phscreen);
    for (i=0; i <= 255 && i < colormap_size; i++)
		XAllocColor(phdisplay,new_colormap,&colors[i]);
}

int OpenGraphics(w,h) 
int w,h;
{
 int i;
 char **argv;

  hsize = w*scale;
  vsize = h*scale;
  display = XOpenDisplay(server);
  if (!display) {
    printf("Failed to open display connection to %s\n",
      XDisplayName(server));
    exit(0);
  }
  screen = DefaultScreen(display);
  depth = XDefaultDepth(display, screen);
  visual = XDefaultVisual(display, screen);
  class = visual->class;
  if (depth == 1) color_screen = 0;
  else if (class == GrayScale) color_screen = 0;
  else if (class == PseudoColor) color_screen = 1;
  else if (class == DirectColor) color_screen = 1;
  else if (class == TrueColor) color_screen = 1;
  else if (class == StaticColor) color_screen = 1;
  else if (class == StaticGray) color_screen = 1;
  black = BlackPixel(display, screen);
  white = WhitePixel(display, screen);
  if (color_screen) {
    background = black;
    foreground = white;
  } else {
    background = white;
    foreground = black;
  }
  hint.x = nextposx; hint.y = nextposy;
  hint.width = hsize; hint.height = vsize;
  hint.flags = PPosition | PSize;
	
	// keep track of window position so we can position next one relative to it
	nextposx = hint.x+hint.width; nextposy=hint.y+hint.height;
	MakeColorMap();  
	fprintf(stderr,"Created  colormap\n");
	
  setattributes.colormap = new_colormap;
  setattributes.background_pixel = WhitePixel(display,screen);
  setattributes.border_pixel = BlackPixel(display,screen);
  window = XCreateWindow(display,
           DefaultRootWindow(display), hint.x, hint.y,
           hint.width, hint.height, 5 , depth,
           InputOutput, visual,
           CWColormap | CWBackPixel | CWBorderPixel,
           &setattributes);
  	fprintf(stderr,"Set attributes\n");

/* window = XCreateSimpleWindow(display,
    DefaultRootWindow(display),
    hint.x, hint.y, hint.width, hint.height, 5,
    foreground, background);
*/
  XSetStandardProperties(display, window, title, title,
    None, argv, 0, &hint);
	/*if (depth != 8) {
		fprintf(stderr,"Screen depth %d not supported; color_screen = %d\n", depth, color_screen);
		exit(0);
	}*/
	XGetWindowAttributes(display,window,&attributes);
	
		XSelectInput(display, window, ButtonPressMask | ExposureMask);
		
  if (DoesBackingStore(DefaultScreenOfDisplay(display))) {
    setattributes.backing_store = WhenMapped;
    XChangeWindowAttributes(display,window,CWBackingStore,&setattributes);
  }


	
  windowGC = XCreateGC(display, window, 0, 0);
  XSetBackground(display, windowGC, background);
  XSetForeground(display, windowGC, foreground);

  XSelectInput(display, window, ButtonPressMask | ExposureMask);
  
  XMapRaised(display, window);
  XNextEvent(display, &event); 
	// true color visuals
	image_data = (char *)calloc(xfield*yfield*scale*scale,sizeof(char)*6);
  if (image_data == NULL) printf("Error in memory allocation\n");
  image = XCreateImage(display, visual, depth, ZPixmap,
    0, image_data, xfield*scale, yfield*scale, 32, 0); 
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
  if (phdepth == 1) color_screen = 0;
  else if (phclass == GrayScale) color_screen = 0;
  else if (phclass == PseudoColor) color_screen = 1;
  else if (phclass == DirectColor) color_screen = 1;
  else if (phclass == TrueColor) color_screen = 1;
  else if (phclass == StaticColor) color_screen = 1;
  else if (phclass == StaticGray) color_screen = 1;
  phblack = BlackPixel(phdisplay, phscreen);
  phwhite = WhitePixel(phdisplay, phscreen);
  if (color_screen) {
    phbackground = phblack;
    phforeground = phwhite;
  } else {
    phbackground = phwhite;
    phforeground = phblack;
  }
	phhint.x = 0; phhint.y = nextposy;
  phhint.width = phhsize; phhint.height = phvsize;
  phhint.flags = PPosition | PSize;
	// keep track of window position so we can position next one relative to it
	nextposx = phhint.x+phhint.width; nextposy=phhint.y+phhint.height;
	
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
  

/* window = XCreateSimpleWindow(display,
    DefaultRootWindow(display),
    hint.x, hint.y, hint.width, hint.height, 5,
    foreground, background);
*/
  XSetStandardProperties(phdisplay, phwindow, phtitle, phtitle,
    None, argv, 0, &phhint);
  XGetWindowAttributes(phdisplay,phwindow,&phattributes);
  XSelectInput(phdisplay, phwindow, ButtonPressMask | ExposureMask);
  if (DoesBackingStore(DefaultScreenOfDisplay(display))) {
    phsetattributes.backing_store = WhenMapped;
    XChangeWindowAttributes(phdisplay,phwindow,CWBackingStore,&phsetattributes);
  }

  if (phdepth != 24) {
    fprintf(stderr,"Screen depth %d not supported\n",phdepth);
    exit(0);
  }
  phwindowGC = XCreateGC(phdisplay, phwindow, 0, 0);
  XSetBackground(phdisplay, phwindowGC, phbackground);
  XSetForeground(phdisplay, phwindowGC, phforeground);

  XSelectInput(phdisplay, phwindow, ButtonPressMask | ExposureMask);
  
  XMapRaised(phdisplay, phwindow);
   
  phimage_data = (char *)calloc(200*200,sizeof(char)*6);
  if (phimage_data == NULL) printf("Error in memory allocation\n");
  phimage = XCreateImage(phdisplay, phvisual, phdepth, ZPixmap,
    0, phimage_data, 200, 200, 32, 0); 
  return (0);
}

int OpenSpaceTime(axt) 
int axt;
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
  if (stdepth == 1) color_screen = 0;
  else if (stclass == GrayScale) color_screen = 0;
  else if (stclass == PseudoColor) color_screen = 1;
  else if (stclass == DirectColor) color_screen = 1;
  else if (stclass == TrueColor) color_screen = 1;
  else if (stclass == StaticColor) color_screen = 1;
  else if (stclass == StaticGray) color_screen = 1;
  stblack = BlackPixel(stdisplay, stscreen);
  stwhite = WhitePixel(stdisplay, stscreen);
  if (color_screen) {
    stbackground = stblack;
    stforeground = stwhite;
  } else {
    stbackground = stwhite;
    stforeground = stblack;
  }
	sthint.x = nextposx, sthint.y = 0;
  sthint.width = sthsize; sthint.height = stvsize;
  sthint.flags = PPosition | PSize;
	// keep track of window position so we can position next one relative to it
	nextposx = sthint.x+sthint.width; nextposy=sthint.y+sthint.height;
	
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
  if (DoesBackingStore(DefaultScreenOfDisplay(display))) {
    stsetattributes.backing_store = WhenMapped;
    XChangeWindowAttributes(stdisplay,stwindow,CWBackingStore,&stsetattributes);
  }

  if (stdepth != 24) {
    fprintf(stderr,"Screen depth %d not supported\n", stdepth);
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


int DrawField()
{
    int x,y,mx,my;
    long offset=0;
    int p;

     for (y=1;y <= yfield; y++)  {
         for (x=1; x <= xfield;x++) {
             for (my=0; my < scale; my++) 
             for (mx=0; mx < scale; mx++) 
                // (image->data)[(offset+ (x-1)*scale+mx + my*xfield*scale)] = state[x][y];
				 XPutPixel(image,(x-1)*scale+mx,(y-1)*scale+my,colors[state[x][y]].pixel);
          }
          //offset +=xfield*scale*scale;
     }
     XPutImage(display,window,windowGC,image,0,0,0,0,image->width,image->height);
}

int DrawSpaceTime(ypos)
int ypos;
{
    int x,mx,my;
    long offset=0;
    int p;
    offset=(stline)*xfield*scale*scale;
     
       
     
         for (x=1; x <= xfield;x++) {
             for (my=0; my < scale; my++) 
             for (mx=0; mx < scale; mx++) 
                 //(stimage->data)[(offset+ (x-1)*scale+mx + my*xfield*scale)] = state[x][ypos];
				  XPutPixel(stimage,(x-1)*scale+mx,stline*scale+my,colors[state[x][ypos]].pixel);
          }
        
    stline=++stline%spacetime;
     XPutImage(stdisplay,stwindow,stwindowGC,stimage,0,0,0,0,stimage->width,stimage->height);
}

int DrawPopDyn(sx,sy,axx,axy,popdyncol)
int sx,sy,axx,axy,popdyncol;
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
           if (state[x][y]==sx) px++; 
	   else
             if (state[x][y]==sy) py++;
         }
     if (sx>0) plotx=(int)(((float)px/axx)*200);
       else plotx=(int)(((float)(thetime%axx)/axx)*200);
     ploty=200-(int)(((float)py/axy)*200);
     if (plotx>199) plotx=199;
      else if (plotx<0) plotx=0;
     
     if (ploty>199) ploty=199;
      else if (ploty<0) ploty=0;

     //(phimage->data)[ploty*200+plotx] = popdyncol;
	 XPutPixel(phimage,plotx,ploty,colors[popdyncol].pixel);
     XPutImage(phdisplay,phwindow,phwindowGC,phimage,0,0,0,0,phimage->width,phimage->height);
}

int ClearPopDyn()
{
    int x,y;

    // for (i=0;i <= 200*200-1; i++)
      //  (phimage->data)[i] = 0;
    for (x=0; x< 200; x++) 
		for (y=0;y<200;y++) 
			XPutPixel(phimage,x,y,colors[0].pixel);
    XPutImage(phdisplay,phwindow,phwindowGC,phimage,0,0,0,0,phimage->width,phimage->height);
}

int ClearSpaceTime()
{
	int x,y;
    for (x=0; x< 200; x++) 
		for (y=0;y<200;y++) 
			XPutPixel(stimage,x,y,colors[0].pixel);
	
   /*  for (i=0;i <= sthsize*stvsize-1; i++)
        (stimage->data)[i] = 0;
    */
    XPutImage(stdisplay,stwindow,stwindowGC,stimage,0,0,0,0,stimage->width,stimage->height);
}

int Majority(x,y)
int x,y;
{
    int p,q,n,max=0,major=0;
    int num[256];

    for (n=0;n<species;++n)
        num[n] = 0;
    for (p=-1;p<=1;++p)
    for (q=-1;q<=1;++q)
        ++num[state[x+p][y+q]];
    for (n=0;n<species;++n)
        if (num[n] > max) {
           max = num[n];
           major=n;
        }
    return(major);
}


int DrawMajorField()
{
    int x,y,mx,my;
    long offset=0;
    int p;

     for (y=1;y <= yfield; y++)  {
          for (x=1; x <= xfield;x++) {
               p = Majority(x,y);
               for (my=0; my < scale; my++) 
               for (mx=0; mx < scale; mx++) 
                   (image->data)[(offset+ (x-1)*scale+mx + my*xfield*scale)] = p;
          }
          offset +=xfield*scale*scale;
     }
    XPutImage(display,window,windowGC,image,0,0,0,0,image->width,image->height);
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
	
	/*int colormap_size=256;
     static XColor *png_colors=0;
     if (!png_colors) {
     // We allocate colors upon the first call only
     // so do not free png_colors!
     if ((png_colors = (XColor *)malloc(colormap_size*sizeof(XColor))) == NULL) {
     fprintf(stderr,"WriteField: No memory for setting up colormap.\n");
     }
     for (i=0; i < colormap_size; i++) {
     png_colors[i].pixel = i;
     png_colors[i].flags = DoRed | DoGreen | DoBlue;
     }
     ReadColorTable(png_colors);
     for (i=0;i<colormap_size;i++) {
     XColor col;
     col =png_colors[i];
     fprintf(stderr, "Color %d: %d, %d, %d\n",i,col.red,col.green,col.blue);
     }
     }*/

    int x,y,mx,my;
	for (y=1;y <= yfield; y++)  {
		for (my=0; my < scale; my++) {
			for (x=1; x <= xfield;x++) {
                
				for (mx=0; mx < scale; mx++) {
					
					XColor col;
					col=colors[state[x][y]];
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
    fclose(fp);
}





















