#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "control_ml.h"

char ***state;
char ***newstate;
int *graph;
char **title;
double Uniform();
int xfield=50,yfield=50,species=2,rseed=1,border=1,asyn=0,layers=1,thetime=0,graphics=1,popdyn=0,spacetime=0;
long prevnum=0;

struct coor {
              int x;
              int y;
};

static int toggle[256],first_num=1,first_growth=1,stop=-1;
static int nx[9] = { 0, 0, 0,-1, 1,-1,-1, 1, 1};
static int ny[9] = { 0,-1, 1, 0, 0,-1, 1,-1, 1};
extern int stline;

/* neighbor procedures */


int RandMoore(int k, int x, int y)
{
    int ran;

    ran=RandomNumber(8);
    return(state[k][x+nx[ran]][y+ny[ran]]);
}

int Moore(int k, int x, int y) 
{
    int sum = 0,i;

    for (i=1;i<=8;i++)
	sum += state[k][x+nx[i]][y+ny[i]];
    return(sum);
}

int CountMoore(int k, int n, int x, int y)
{
    int sum = 0,i;

    for (i=1;i<=8;i++)
	 if (state[k][x+nx[i]][y+ny[i]] == n) sum++;
    return(sum);
}

int RandVonn(int k, int x, int y)
{
    int ran;

    ran=RandomNumber(4);
    return(state[k][x+nx[ran]][y+ny[ran]]);
}

int Vonn(int k, int x, int y)
{
    int sum = 0,i;

    for (i=1;i<=4;i++)
	sum += state[k][x+nx[i]][y+ny[i]];
    return(sum);
}

int CountVonn(int k, int n, int x, int y)
{
    int sum = 0,i;

    for (i=1;i<=4;i++)
	 if (state[k][x+nx[i]][y+ny[i]] == n) sum++;
    return(sum);
}

void CopyLayer(int k, int l)
{
     int x,y;

     for (x=0;x<=xfield+1;++x)
     for (y=0;y<=yfield+1;++y)
	 state[l][x][y] = state[k][x][y];
}

void InitRand()
{
     SetSeed(rseed);
}

/* border procedures */

void AsynTorus(int x, int y)
 {
      int k;

      for (k=0;k<layers;++k) {
         state[k][x][0] = state[k][x][yfield];
         state[k][x][yfield+1] = state[k][x][1];
         state[k][0][y] = state[k][xfield][y];
         state[k][xfield+1][y] = state[k][1][y];
         state[k][0][0] = state[k][xfield][yfield];
         state[k][0][yfield+1] = state[k][xfield][1];
         state[k][xfield+1][0] = state[k][1][yfield];
         state[k][xfield+1][yfield+1] = state[k][1][1];
      }
 }

void AsynEcho(int x, int y)
 {
      int k;

      for (k=0;k<layers;++k) {
          state[k][x][0] = state[k][x][1];
          state[k][x][yfield+1] = state[k][x][yfield];
          state[k][0][y] = state[k][1][y];
          state[k][xfield+1][y] = state[k][xfield][y];
          state[k][0][0] = state[k][1][1];
          state[k][0][yfield+1] = state[k][1][yfield];
          state[k][xfield+1][0] = state[k][xfield][1];
          state[k][xfield+1][yfield+1] = state[k][xfield][yfield];
      }
 }

 void SynTorus()
 {
    int i,k;

    for (k=0;k<layers;++k) {
        for (i = 1; i <= xfield; ++i) {
            state[k][i][0] = state[k][i][yfield];
            state[k][i][yfield+1] = state[k][i][1];
        }
        for (i = 1; i <= yfield; ++i) {
           state[k][xfield+1][i] = state[k][1][i];
           state[k][0][i] = state[k][xfield][i];
        }
        state[k][0][0] = state[k][xfield][yfield];
        state[k][0][yfield+1] = state[k][xfield][1];
        state[k][xfield+1][0] = state[k][1][yfield];
        state[k][xfield+1][yfield+1] = state[k][1][1];
    }
 }

void TorusLayer(int k)
 {
      int i;

      for (i = 1; i <= xfield; ++i) {
	  state[k][i][0] = state[k][i][yfield];
	  state[k][i][yfield+1] = state[k][i][1];
      }
      for (i = 1; i <= yfield; ++i) {
      	  state[k][0][i] = state[k][xfield][i];
	  state[k][xfield+1][i] = state[k][1][i];
      }
      state[k][0][0] = state[k][xfield][yfield];
      state[k][0][yfield+1] = state[k][xfield][1];
      state[k][xfield+1][0] = state[k][1][yfield];
      state[k][xfield+1][yfield+1] = state[k][1][1];
 }

 void EchoBorder()
 {
    int i,k;

    for (k=0;k<layers;++k) {
        for (i = 1; i <= xfield; ++i) {
            state[k][i][0] = state[k][i][1];
            state[k][i][yfield+1] = state[k][i][yfield];
        }
        for (i = 1; i <= yfield; ++i) {
            state[k][0][i] = state[k][1][i];
            state[k][xfield+1][i] = state[k][xfield][i];
        }
        state[k][0][0] = state[k][1][1];
        state[k][0][yfield+1] = state[k][1][yfield];
        state[k][xfield+1][0] = state[k][xfield][1];
        state[k][xfield+1][yfield+1] = state[k][xfield][yfield];
    }
 }

 /* diffusion */

void TurnLeft(int k, int x, int y)
 {
      char dummy;

      dummy = state[k][x][y];
      state[k][x][y] = state[k][x][y+1];
      state[k][x][y+1] = state[k][x+1][y+1];
      state[k][x+1][y+1] = state[k][x+1][y];
      state[k][x+1][y] = dummy;
 }

void TurnRight(int k, int x, int y)
 {
      char dummy;

      dummy = state[k][x][y];
      state[k][x][y] = state[k][x+1][y];
      state[k][x+1][y] = state[k][x+1][y+1];
      state[k][x+1][y+1] = state[k][x][y+1];
      state[k][x][y+1] = dummy;
 }

 void DoTurn(int k, int x, int y)
 {
      if (stop == -1) {
	 if (Uniform() < 0.5)
	    TurnRight(k,x,y);
	 else
	    TurnLeft(k,x,y);
      }
      else if (state[stop][x][y] != 1 && state[stop][x+1][y] != 1 &&
	     state[stop][x][y+1] != 1 && state[stop][x+1][y+1] != 1) {
	 if (Uniform() < 0.5)
	    TurnRight(k,x,y);
	 else
	    TurnLeft(k,x,y);
      }
 }

void DoTurnTogether(int k1, int k2, int x, int y)
{
    if (stop == -1) {
        if (Uniform() < 0.5) {
            TurnRight(k1,x,y);
            TurnRight(k2,x,y);
        }
        else {
            TurnLeft(k1,x,y);
            TurnLeft(k2,x,y);
        }
    }
    else if (state[stop][x][y] != 1 && state[stop][x+1][y] != 1 &&
             state[stop][x][y+1] != 1 && state[stop][x+1][y+1] != 1) {
        if (Uniform() < 0.5) {
            TurnRight(k1,x,y);
            TurnRight(k2,x,y);
        }
        else {
            TurnLeft(k1,x,y);
            TurnLeft(k2,x,y);
        }
    }
}

 void SetBoundaries(int k)
 {
      int i;

      for (i=0;i<=yfield+1;++i) {
	  state[k][0][i] = 0;
	  state[k][xfield+1][i] = 0;
      }
      for (i=0;i<=xfield+1;++i) {
	  state[k][i][0] = 0;
	  state[k][i][yfield+1]=0;
      }
 }

 void Diffuse(int k)
 {
      int i,x,y,start=0,endx=xfield-1,endy=yfield-1;

      if (toggle[k]) start=1;
      else if (border==2) start=2;
      if (border==0) {++endx;++endy;}

      for (x=start;x<=endx;x+=2)
      for (y=start;y<=endy;y+=2)
          DoTurn(k,x,y);

      if (border==0) SetBoundaries(k); 
      else if (border==1) {
         if (!toggle[k]) {
 	    state[k][xfield][yfield]=state[k][0][0];
	    for (i=1;i<yfield;++i) state[k][xfield][i] = state[k][0][i];
	    for (i=1;i<xfield;++i) state[k][i][yfield] = state[k][i][0];
         }
         TorusLayer(k);      
      }
      ++toggle[k]; toggle[k]%=2;
 }

void DiffuseTogether(int k1, int k2)
{
    int i,x,y,start=0,endx=xfield-1,endy=yfield-1;
    
    if (toggle[k1]) start=1;
    else if (border==2) start=2;
    if (border==0) {++endx;++endy;}
    
    for (x=start;x<=endx;x+=2)
        for (y=start;y<=endy;y+=2)
            DoTurnTogether(k1,k2,x,y);
    
    if (border==0) { SetBoundaries(k1); SetBoundaries(k2); }
    else if (border==1) {
        if (!toggle[k1]) {
            state[k1][xfield][yfield]=state[k1][0][0];
            for (i=1;i<yfield;++i) state[k1][xfield][i] = state[k1][0][i];
            for (i=1;i<xfield;++i) state[k1][i][yfield] = state[k1][i][0];
            
        }
        if (!toggle[k2]) {
            state[k2][xfield][yfield]=state[k2][0][0];
            for (i=1;i<yfield;++i) state[k2][xfield][i] = state[k2][0][i];
            for (i=1;i<xfield;++i) state[k2][i][yfield] = state[k2][i][0];
        }
       TorusLayer(k1); TorusLayer(k2);
    }
    ++toggle[k1]; toggle[k1]%=2;
    ++toggle[k2]; toggle[k2]%=2;
}


 void DiffuseStop(int k,int l)
 {
     stop = l;
     Diffuse(k);
     stop = -1;
 }


void ReShuffle(int k)
{
     int x,y;
     int i,j;
     char dummy;


     for (x = 1; x <= xfield; ++x)
     for (y = 1; y <= yfield; ++y) {
         i = x + RandomNumber(xfield-x);
         if (i == x)
            j = y + RandomNumber(yfield-y);
         else
            j = RandomNumber(yfield);
         dummy = state[k][i][j];
         state[k][i][j] = state[k][x][y];
         state[k][x][y] = dummy;
     }
     if (border==1) TorusLayer(k);
     if (border==2) EchoBorder();
}

/* initialisatie procedures */


 void InitRandomLayer(int k)
 {
      int  x,y,n,i;
      float perc[256];

      perc[species-1] = 100.;
      printf("\nlayer %d\n",k);
      for (n = 0; n < species-1; n++) {
	  printf("perc state %d: ",n);
	  scanf("%f",&perc[n]);
          perc[species-1] -= perc[n];
	  if (perc[species-1] < 0. || perc[n] > 100. || perc[n]<0) {
	     printf("wrong ! try again ..\n");
	     perc[species-1] = 100.;
	     n=-1;
	  }
	  if (perc[species-1]== 0.) {
	     for (i = n+1; i< species-1;i++) {
		 perc[i]= 0.;
		 printf("perc state %d: %4.1f",i,perc[i]);
	     }
	     n=species-1;
	  }
      }
      printf("perc state %d: %4.1f\n",species-1,perc[species-1]);
      for (n=0;n<species;++n)
	  perc[n] *= (xfield/100.)*yfield;
      n = 0;
      for (x=1; x<=xfield && n<species;++x)
      for (y=1; y<=yfield && n<species;++y) {
	  while (perc[n] <= 0.)
		++n;
	  --perc[n];
	  state[k][x][y] = n;
     }
     ReShuffle(k);
 }


 void InitSpeciesRandom()
 {
      int k;
 
     for (k=0;k<layers;++k) {
          InitRandomLayer(k);
     }    
     thetime=0;
 }

 void ReadPatternFile()
 {
      FILE *fopen(), *fp;
      char name[50];
      int  k,x,y,spec;

      for (k=0;k<layers;k++)\
      for (x=0;x<=xfield+1;++x)
      for (y=0;y<=yfield+1;++y){
	  state[k][x][y] = 0;
      }
      printf("\npatternfile : ");
      scanf("%s",name);
      fp = fopen(name,"r");
      if (fp == NULL){
	 printf("\nfile not found");
      } else {
	 while (fscanf(fp,"%k %d %d %d\n",&k,&x,&y,&spec) > 0) {
	       if (k>0)
		  state[k][x][y] = spec;
	 }
	 fclose(fp);
	 thetime = 0;
	 first_num=1;
	 first_growth=1;
	 if (border==1) SynTorus();
	 if (border==2) EchoBorder();
	 DrawField();
      }
      thetime=0;
 }

 /* control procedures */

 void EchoTime()
 {

      printf("\nTime = %d\n",thetime);
 }

 void ReadState()
 {

     FILE *fopen(), *fp;
     char name[50];
     int  x,y,k,vars[7],stop=0;

     printf("\nread file : ");
     scanf("%s",name);
     fp = fopen(name,"r");
     if (fp == NULL){
	printf("file not found\n");
     } else {
        fread(vars,sizeof(int),7,fp);
        if (vars[6]!=123) {stop=1;printf("file in wrong format\n");}
        else {
           if (vars[0]!=layers) {stop=1;printf("change layers to:  %d\n",vars[0]);}
           if (vars[1]!=xfield) {stop=1;printf("change xfield to:  %d\n",vars[1]);}
           if (vars[2]!=yfield) {stop=1;printf("change yfield to:  %d\n",vars[2]);}
           if (vars[3]>species) {stop=1;printf("change species to: %d\n",vars[3]);}
           if (vars[3]<species)  
              printf("warning: changed number of species\nold number: %d\n",vars[3]);
           if (vars[4]!=border)  
              printf("warning: changed border rule\nold rule: %d\n",vars[4]);
        }
        if (!stop) {
           thetime = vars[5];
	   for (k=0;k<layers;++k)
	   for (x=0;x<=xfield+1;x++)
	      fread(state[k][x],sizeof(char),yfield+2,fp);
	   fclose(fp);
	   first_num=1;
	   first_growth=1;
	   if (border==1) SynTorus();
	   if (border==2) EchoBorder();
	   DrawField();
           printf("file '%s' read\n",name);
        } else printf("<<aborted>>\n");
     }
}

void CountSpecies()
{

     int x,y,n,k;
     long number[256];
     double perc;

     for (k=0;k<layers;++k) {
         for (n = 0; n < 256; ++n)
             number[n] = 0;
         for (x = 1; x <= xfield; ++x)
         for (y = 1; y <= yfield; ++y)
             ++number[state[k][x][y]];
         printf("\nlayer %d   number perc\n",k);
         for (n = 0; n < species; ++n) {
             perc = number[n] / ((xfield*0.01)*yfield);
             printf("state %d: %6ld %7.2f\n",n,number[n],perc);
          }
     }
}

void SaveState()
{

     FILE *fopen(), *fp;
     int  x,y,k,vars[7];
     short val;
     char name[50],dummy[2];
  
     printf("\nsave file : ");
     scanf("%s",name);
     val = 'y';
     if ((fp = fopen(name,"r")) != NULL) {
        fclose(fp);
        printf("file '%s' already exist\ndo you want to overwrite ? (y/n) : ",name);
        scanf("%s",dummy);
        val = dummy[0];
     }
     if (val == 'y') {
        fp = fopen(name,"w");
        vars[0]=layers; vars[1]=xfield; vars[2]=yfield;
        vars[3]=species; vars[4]=border; vars[5]=thetime;vars[6]=123;
        fwrite(vars,sizeof(int),7,fp);        
        for (k=0;k<layers;++k)
        for (x=0;x<=xfield+1;++x)                         
           fwrite(state[k][x],sizeof(char),(yfield+2),fp);        
        fclose(fp);    
        printf("file '%s' saved\n",name);
     } else  printf("<<aborted>>");
}

void RecordState()
{

     FILE *fopen(), *fp;
     char name[50];
     int  x,y,k,vars[7];

     sprintf(name,"state.%d",thetime);
     fp = fopen(name,"w");
     vars[0]=layers; vars[1]=xfield; vars[2]=yfield;
     vars[3]=species; vars[4]=border; vars[5]=thetime;vars[6]=123;
     fwrite(vars,sizeof(int),7,fp);        
     for (k=0;k<layers;++k)
     for (x = 0; x <= xfield+1; ++x)
	 fwrite(state[k][x],sizeof(char),yfield+2,fp);
     fclose(fp);

}

void RecordNumber()
{

      FILE *fopen(), *fp;
      char name[50];
      int x,y,k;
      long num[256];

     sprintf(name,"num.dat");
     if (first_num) {
	first_num = 0;
	fp = fopen(name,"w");
     }
     else fp = fopen(name,"a");
     fprintf(fp,"\n%d",thetime);
     for (k=0;k<layers;++k) {
         for (x = 0;x <= species; ++x)
         num[x] = 0;
         for (x=1;x <= xfield; ++x)
         for (y=1;y <= yfield; ++y)
                ++num[state[k][x][y]];
         for (x = 0;x < species; ++x)
             fprintf(fp,"\t%d",num[x]);
     }
     fclose(fp);
 }

 void RecordGrowth(int k,int spec)
 {
      FILE *fopen(), *fp;
      char name[50];
      int x,y;
      long num[256];
      double growth;


      for (x = 0;x <= species; ++x)
	  num[x] = 0;
      for (x=1;x <= xfield; ++x)
      for (y=1;y <= yfield; ++y)
	  ++num[state[k][x][y]];
      sprintf(name,"growth%d.%d",k,spec);
      if (!first_growth) {
	 fp = fopen(name,"a");
	 fprintf(fp,"%d",prevnum);
	 if (prevnum > 0) {
	    growth = ((double)(num[spec]-prevnum))/((double)prevnum);
	    fprintf(fp,"\t%f\n",growth);
	 }
	 fclose(fp);
      }
      else {
	 first_growth = 0;
	 fp = fopen(name,"w");
	 fclose(fp);
      }
      prevnum = num[spec];
}


 void InitAgain()
 {
      int x,y,k;

      for (k=0;k<layers;++k)
      for (x=0;x<=xfield+1;++x)
      for (y=0;y<=yfield+1;++y)
	  state[k][x][y] = 0;
      InitSpecies();
      thetime = 0;
      first_num = 1;
      first_growth = 1;
      if (border == 1) SynTorus();
      if (border == 2) EchoBorder();
      DrawField();
 }

void ShowControlPanel()
{
     printf(" d  drawfield\n");
     printf(" l  drawlayer\n");
     printf(" t  thetime\n");
     printf(" s  save file\n");
     printf(" r  read file\n");
     printf(" p  read pattern file\n");
     printf(" n  count numbers\n");
     printf(" i  init again\n");
     printf(" c  continue\n");
     printf(" S  step\n");
     printf(" C  clear population dynamics plot\n");
     printf(" P  clear spacetime plot\n");
     printf(" q  quit\n");
 }

 ControlPanel()
 {
    char control,dummy[2];

    control='g';
    while (control != 'q' && control != 'c' && control != 'S') {
	  printf("\nEnter control: ");
          scanf("%s",dummy);
          control = dummy[0];
	  switch (control) {
		case 'c':
                case 'S':
		case 'q':  break;
		case 'd':  DrawField(); break;
		case 'l':  DrawLayerField(); break;
		case 't':  EchoTime(); break;
		case 's':  SaveState(); break;
		case 'r':  ReadState();  break;
		case 'n':  CountSpecies(); break;
		case 'i':  InitAgain();break;
		case 'p':  ReadPatternFile(); break;
                case '?':
                case 'h':  ShowControlPanel(); break;
		case 'C':  ClearPopDyn(); break;
		case 'P':  ClearSpaceTime(); stline=0; break;
                default :  printf("\nUnknown option '%c'",control); break;
	  }
    }
    return (control);
 }

/* main */

 MultiLayer()
 {
     int x,y,i,j,k;
     char control='g';
     long p;

     /* Initialisatie */
     InitConstants(); 
	 InitPngMemory();
     title = (char **)calloc(graphics+1,sizeof(char *));
     for (k=0;k<graphics;++k) 
          title[k]=(char *)calloc(50,sizeof(char));
     if (graphics) strcpy(title[0],"CA X11 field"); 
     for (k=1;k<graphics;++k) 
         sprintf(title[k],"CA X11, laag %d",k-1);  
     graph = (int *)calloc(graphics+1,sizeof(int)); 
     for (k=0;k<graphics;++k) 
         graph[k]=k-1;
    // if (graphics) InitGraphs();
     state = (char ***)calloc(layers,sizeof(char *));
     for (k=0;k<layers;++k) {
	 state[k] = (char **)calloc(xfield+2,sizeof(char *));
         for (x=0; x <= xfield+1; x++)
	     state[k][x] = (char *)calloc(yfield+2,sizeof(char));
     }
     newstate = (char ***)calloc(xfield+2,sizeof(char *));
     for (k=0;k<layers;++k) {
	 newstate[k] = (char **)calloc(xfield+2,sizeof(char *));
         for (x=0; x <= xfield+1; x++)
	     newstate[k][x] = (char *)calloc(yfield+2,sizeof(char));
     }
     thetime=0;
     if (graphics) OpenGraphics(xfield,yfield);
     if (popdyn) { 
		 OpenPopDyn();
		 fprintf(stderr, "Pop dyn opened\n");
		 ClearPopDyn();
    		 fprintf(stderr, "Pop dyn done\n");
		 
	 }
     if (spacetime) {
            OpenSpaceTime(spacetime);
	    ClearSpaceTime();
     }
     InitRand();
     InitSpecies();
     if (border == 1) SynTorus();
     if (border == 2) EchoBorder();
     if (graphics) {
        DrawField();
        control = ControlPanel();
     }
    
        
     /* Run-loop */

     while (control != 'q') {

	   Report();

	   if (graphics) {
              if (Mouse() || control == 'S') {
	         control = ControlPanel();
	      }
            }

	   if (control == 'q') break;

	   if (asyn) {

	   /* Bereken NextState asynchroon*/

	      for (p = 0; p < xfield*yfield; ++p) {
		  x = RandomNumber(xfield);
		  y = RandomNumber(yfield);
		  NextState(x,y);
                  for (k=0;k<layers;k++)
		      state[k][x][y] = newstate[k][x][y];
		  if (x == 1 || x == xfield || y == 1 || y == yfield) {
		     if (border == 1) AsynTorus(x,y);
		     if (border == 2) AsynEcho(x,y);
		  }
	      }
	   } else if (!asyn) {

	   /* Bereken Nextstate synchroon */

	      /* Bereken NextState */

	      for (x = 1; x <= xfield; ++x)
	      for (y = 1; y <= yfield; ++y)
		  NextState(x,y);

	      /* State := NextState */

	      for (k=0;k<layers;k++)
              for (y = 1; y <= yfield; ++y)
              for (x = 1; x <= xfield; ++x)
		  state[k][x][y] = newstate[k][x][y];

	      /* Verbind Randen */

	      if (border==1) SynTorus();
	      if (border==2) EchoBorder();

	   }
	   ++thetime;
     }
     CloseGraphics();
     if (popdyn) ClosePopDyn();
     if (spacetime) CloseSpaceTime();
     return(0);
 }





