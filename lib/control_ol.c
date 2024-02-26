#include <stdio.h>
#include <stdlib.h>

char **state;
char **newstate;
double Uniform();
int xfield=50,yfield=50,species=2,rseed=1,border=1,asyn=0,thetime=0,graphics=1,popdyn=0,spacetime=0;
long prevnum=0;

static int toggle=1,first_num=1,first_growth=1;
static int nx[9] = { 0, 0, 0,-1, 1,-1,-1, 1, 1};
static int ny[9] = { 0,-1, 1, 0, 0,-1, 1,-1, 1};

/* neighbor procedures */


int RandMoore(x,y) int x,y;
{
    int ran;

    ran=RandomNumber(8);
    return(state[x+nx[ran]][y+ny[ran]]);
}

int Moore(x,y) int x,y;
{
    int sum = 0,i;

    for (i=1;i<=8;i++)
	sum += state[x+nx[i]][y+ny[i]];
    return(sum);
}

int CountMoore(n,x,y) int n,x,y;
{
    int sum = 0,i;

    for (i=1;i<=8;i++)
	 if (state[x+nx[i]][y+ny[i]] == n) sum++;
    return(sum);
}

int RandVonn(x,y) int x,y;
{
    int ran;

    ran=RandomNumber(4);
    return(state[x+nx[ran]][y+ny[ran]]);
}

int Vonn(x,y) int x,y;
{
    int sum = 0,i;

    for (i=1;i<=4;i++)
	sum += state[x+nx[i]][y+ny[i]];
    return(sum);
}

int CountVonn(n,x,y) int n,x,y;
{
    int sum = 0,i;

    for (i=1;i<=4;i++)
	 if (state[x+nx[i]][y+ny[i]] == n) sum++;
    return(sum);
}

void InitRand()
{
     SetSeed(rseed);
}

/* border procedures */

 void AsynTorus(x,y) int x,y;
 {
      state[x][0] = state[x][yfield];
      state[x][yfield+1] = state[x][1];
      state[0][y] = state[xfield][y];
      state[xfield+1][y] = state[1][y];
      state[0][0] = state[xfield][yfield];
      state[0][yfield+1] = state[xfield][1];
      state[xfield+1][0] = state[1][yfield];
      state[xfield+1][yfield+1] = state[1][1];
 }

 void AsynEcho(x,y) int x,y;
 {
      state[x][0] = state[x][1];
      state[x][yfield+1] = state[x][yfield];
      state[0][y] = state[1][y];
      state[xfield+1][y] = state[xfield][y];
      state[0][0] = state[1][1];
      state[0][yfield+1] = state[1][yfield];
      state[xfield+1][0] = state[xfield][1];
      state[xfield+1][yfield+1] = state[xfield][yfield];
 }

 void SynTorus()
 {
    int i;

    for (i = 1; i <= xfield; ++i) {
        state[i][0] = state[i][yfield];
        state[i][yfield+1] = state[i][1];
    }
    for (i = 1; i <= yfield; ++i) {
       state[xfield+1][i] = state[1][i];
       state[0][i] = state[xfield][i];
    }
    state[0][0] = state[xfield][yfield];
    state[0][yfield+1] = state[xfield][1];
    state[xfield+1][0] = state[1][yfield];
    state[xfield+1][yfield+1] = state[1][1];
 }

 void EchoBorder()
 {
    int i;

    for (i = 1; i <= xfield; ++i) {
        state[i][0] = state[i][1];
        state[i][yfield+1] = state[i][yfield];
    }
    for (i = 1; i <= yfield; ++i) {
        state[0][i] = state[1][i];
        state[xfield+1][i] = state[xfield][i];
    }
    state[0][0] = state[1][1];
    state[0][yfield+1] = state[1][yfield];
    state[xfield+1][0] = state[xfield][1];
    state[xfield+1][yfield+1] = state[xfield][yfield];
 }

 /* diffusion */

 void TurnLeft(x,y) int x,y;
 {
      char dummy;

      dummy = state[x][y];
      state[x][y] = state[x][y+1];
      state[x][y+1] = state[x+1][y+1];
      state[x+1][y+1] = state[x+1][y];
      state[x+1][y] = dummy;
 }

 void TurnRight(x,y) int x,y;
 {
      char dummy;

      dummy = state[x][y];
      state[x][y] = state[x+1][y];
      state[x+1][y] = state[x+1][y+1];
      state[x+1][y+1] = state[x][y+1];
      state[x][y+1] = dummy;
 }

 void DoTurn(x,y) int x,y;
 {
      if (Uniform() < 0.5)
	 TurnLeft(x,y);
      else
	 TurnRight(x,y);
 }

 void SetBoundaries()
 {
      int i;

      for (i=0;i<=yfield+1;++i) {
	  state[0][i] = 0;
	  state[xfield+1][i] = 0;
      }
      for (i=0;i<=xfield+1;++i) {
	  state[i][0] = 0;
	  state[i][yfield+1]=0;
      }
 }

 void Diffuse()
 {
      int i,x,y,start=0,endx=xfield-1,endy=yfield-1;

      if (toggle) start=1;
      else if (border==2) start=2;
      if (border==0) {++endx;++endy;}

      for (x=start;x<=endx;x+=2)
      for (y=start;y<=endy;y+=2)
          DoTurn(x,y);

      if (border==0) SetBoundaries(); 
      else if (border==1) {
         if (!toggle) {
 	    state[xfield][yfield]=state[0][0];
	    for (i=1;i<yfield;++i) state[xfield][i] = state[0][i];
	    for (i=1;i<xfield;++i) state[i][yfield] = state[i][0];
         }
         SynTorus();      
      }
      ++toggle; toggle%=2;
 }


void ReShuffle()
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
         dummy = state[i][j];
         state[i][j] = state[x][y];
         state[x][y] = dummy;
     }
     if (border==1) SynTorus();
     if (border==2) EchoBorder();
}

/* initialisatie procedures */


 void InitSpeciesRandom()
 {
      int  x,y,n,i;
      float *perc;

      perc = (float *)calloc(species,sizeof(float));
      perc[species-1] = 100.;
      printf("\n");
      for (n = 0; n < species-1; n++) {
	  printf("perc state %d: ",n);
	  scanf("%f",&perc[n]);
          perc[species-1] -= perc[n];
	  if (perc[species-1] < 0. || perc[n] > 100. || perc[n]<0) {
	     printf("\nwrong ! try again ..");
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
	  state[x][y] = n;
     }
     ReShuffle();
     thetime = 0;
 }

 void ReadPatternFile()
 {
      FILE *fopen(), *fp;
      char name[50];
      int  x,y,spec;

      for (x=0;x<=xfield+1;++x)
      for (y=0;y<=yfield+1;++y){
	  state[x][y] = 0;
      }
      printf("\npatternfile : ");
      scanf("%s",name);
      fp = fopen(name,"r");
      if (fp == NULL){
	 printf("\nfile not found");
      } else {
	 while (fscanf(fp,"%d %d %d\n",&x,&y,&spec) > 0) {
	       if (x>0)
		  state[x][y] = spec;
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
     int  x,y,vars[7],stop=0;

     printf("\nread file : ");
     scanf("%s",name);
     fp = fopen(name,"r");
     if (fp == NULL){
	printf("file not found\n");
     } else {
        fread(vars,sizeof(int),7,fp);
        if (vars[6]!=123) {stop=1;printf("file in wrong format\n");}
        else {
           if (vars[0]!=1) {stop=1;printf("change layers to:  %d\n",vars[0]);}
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
	   for (x=0;x<=xfield+1;x++)
	      fread(state[x],sizeof(char),yfield+2,fp);
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

     int x,y,n;
     long number[1000];
     double perc;

     for (n = 0; n < 1000; ++n)
         number[n] = 0;
     for (x = 1; x <= xfield; ++x)
     for (y = 1; y <= yfield; ++y)
         ++number[state[x][y]];
     printf("\n           number perc\n");
     for (n = 0; n < species; ++n) {
         perc = number[n] / ((xfield*0.01)*yfield);
         printf("state %d: %6ld %7.2f\n",n,number[n],perc);
      }
}

void SaveState()
{

     FILE *fopen(), *fp;
     int  x,y,vars[7];
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
        vars[0]=1; vars[1]=xfield; vars[2]=yfield;
        vars[3]=species; vars[4]=border; vars[5]=thetime;vars[6]=123;
        fwrite(vars,sizeof(int),7,fp);        
        for (x=0;x<=xfield+1;++x)                         
           fwrite(state[x],sizeof(char),(yfield+2),fp);        
        fclose(fp);    
        printf("file '%s' saved\n",name);
     } else  printf("<<aborted>>");
}

void RecordState()
{

     FILE *fopen(), *fp;
     char name[50],dummy[2];
     int  x,y,vars[7];

     sprintf(name,"state.%d",thetime);
     fp = fopen(name,"w");
     vars[0]=1; vars[1]=xfield; vars[2]=yfield;
     vars[3]=species; vars[4]=border; vars[5]=thetime;vars[6]=123;
     fwrite(vars,sizeof(int),7,fp);        
     for (x = 0; x <= xfield+1; ++x)
	 fwrite(state[x],sizeof(char),yfield+2,fp);
     fclose(fp);

}

void RecordNumber()
{

      FILE *fopen(), *fp;
      char name[50];
      int x,y;
      long num[256];

     sprintf(name,"num.dat");
     if (first_num) {
	first_num = 0;
	fp = fopen(name,"w");
     }
     else fp = fopen(name,"a");
     for (x = 0;x <= species; ++x)
     num[x] = 0;
     for (x=1;x <= xfield; ++x)
     for (y=1;y <= yfield; ++y)
            ++num[state[x][y]];
     fprintf(fp,"\n%d",thetime);
     for (x = 0;x < species; ++x)
         fprintf(fp,"\t%d",num[x]);
     fclose(fp);
 }

 void RecordGrowth(spec) int spec;
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
	  ++num[state[x][y]];
      sprintf(name,"growth.%d",spec);
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
      int x,y;

      for (x=0;x<=xfield+1;++x)
      for (y=0;y<=yfield+1;++y)
	  state[x][y] = 0;
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
     printf(" D  drawmajorfield\n");
     printf(" t  thetime\n");
     printf(" s  save file\n");
     printf(" r  read file\n");
     printf(" p  read pattern file\n");
     printf(" n  count numbers\n");
     printf(" i  init again\n");
     printf(" c  continue\n");
     printf(" S  step\n");
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
		case 'D':  DrawMajorField(); break;
		case 't':  EchoTime(); break;
		case 's':  SaveState(); break;
		case 'r':  ReadState();  break;
		case 'n':  CountSpecies(); break;
		case 'i':  InitAgain();break;
		case 'p':  ReadPatternFile(); break;
                case '?':
                case 'h':  ShowControlPanel(); break;
		default :  printf("\nUnknown option '%c'",control); break;
	  }
    }
    return (control);
 }

/* main */

 OneLayer()
 {
     int x,y,i,j;
     char control='g';
     long k;

     /* Initialisatie */

     InitConstants();
     state = (char **)calloc(xfield+2,sizeof(char *));
     for (x=0; x <= xfield+1; x++)
	 state[x] = (char *)calloc(yfield+2,sizeof(char));
     newstate = (char **)calloc(xfield+2,sizeof(char *));
     for (x=0; x <= xfield+1; x++)
	 newstate[x] = (char *)calloc(yfield+2,sizeof(char));
     thetime=0;
     if (graphics) OpenGraphics(xfield,yfield);
     if (popdyn) {
		 fprintf(stderr, "Trying to open popdyn\n");
		 
		 OpenPopDyn();
		 fprintf(stderr, "Opened popdyn\n");
		 ClearPopDyn();
		 fprintf(stderr, "Cleared popdyn\n");
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

	      for (k = 0; k < xfield*yfield; ++k) {
		  x = RandomNumber(xfield);
		  y = RandomNumber(yfield);
		  NextState(x,y);
		  state[x][y] = newstate[x][y];
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

	      for (y = 1; y <= yfield; ++y)
              for (x = 1; x <= xfield; ++x)
		  state[x][y] = newstate[x][y];

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

