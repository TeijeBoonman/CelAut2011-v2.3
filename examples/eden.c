#include <stdio.h>
extern int xfield,yfield,scale,species,rseed,border,asyn,thetime,graphics,spacetime,popdyn;
 extern char **state,**newstate;
 double Uniform();

 InitConstants()
 {
      xfield    =  100;	 /* horizontal size of the automaton */
      yfield    =  100;	 /* vertical size of the automaton */
      scale    =   3;    /* scale of drawing */
      species  =   2;    /* number of states; range 1-256 */
      rseed    =   1;    /* seed of the randomgenerator */
      border   =   1;    /* 0 = empty borders; 1 = torus; 2 = echoing borders */
      asyn     =   0;    /* 1 = asynchronous updating; 0 = synchronous */
      graphics =   2;    /* 0 = program runs silently */
	 spacetime = 0;    /* length of space/time plot; 0 = no space time plot */
	 popdyn = 0;         /* 1 = population dynamics plot; 0 = no pop.dyn. plot */
 }

 InitSpecies()
 {
      int x,y;

	 state[xfield/2][yfield/2]=1;
      //InitSpeciesRandom();
 }

 NextState(x,y) int x,y;
 {
     int north = state[x][y-1];
	 int south = state[x][y+1];
	 int east = state[x+1][y];
	 int west = state[x-1][y];
	 double p_up=.01, p_lr=.01;
	
	 double p = 1.-(1.-north*p_up)*(1.-south*p_up)*(1.-east*p_lr)*(1.-west*p_lr);
	 double r=Uniform();
	 if (r<p) {
		 newstate[x][y] = 1;
	 }
	 else
		 newstate[x][y]=state[x][y];
	 
 }

 Report()
 {
	 static int frame=0;
      char filename[100];
	 DrawField(); 
 snprintf(filename,99,"frame%05d.png",frame++);
	WriteField(filename);
	// RecordGrowth(1);
	/* DrawSpaceTime(xfield/2);
	 DrawPopDyn(0,1,1000,xfield*yfield/10,1);*/
      if (thetime%10==0) EchoTime();

	 } 

 main()
 {
     OneLayer();
 }
