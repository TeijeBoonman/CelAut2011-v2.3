 extern int
xfield,yfield,scale,species,rseed,border,asyn,thetime,layers,graphics,popdyn,spacetime;
 extern char ***state,***newstate;
 extern int *graph;
 double Uniform();

 void InitConstants()
 {
      xfield   = 300;	   /* horizontal size of the automaton */
      yfield   = 500;	   /* vertical size of the automaton */
      scale    =   1;      /* scale of drawing */
      species  =   2;	   /* number of states; range 1-256 */
      rseed    =   248;	   /* seed of the randomgenerator */
      border   =   2;	   /* 0 = empty borders; 1 = torus; 2 = echoing borders */
      asyn     =   0;	   /* 1 = asynchronous updating; 0 = synchronous */
      layers   =   2;	   /* number of layers */
      graphics =   1;      /* 0 = pograms runs silently */
      popdyn = 0;
      spacetime = 0;
}

InitGraphs()
{
}


 void InitSpecies()

 {
      int x,y,k;

   InitRandomLayer(0);
     for (int x=0;x<xfield+2;x++) {
         state[1][x][yfield]=1;
     }
     
     /* state[1][2*xfield/5][yfield/2]= 1;
      state[1][3*xfield/5][yfield/2]= 1; */
 }

 void NextState(x,y) int x,y;

 {
     const double p=0.2;
     newstate[0][x][y] = state[0][x][y];
     newstate[1][x][y] = state[1][x][y];
      switch (state[1][x][y]) {
	 case 0:
		if ((state[0][x][y]==1 && state[1][x][y]==0) && (CountMoore(1,1,x,y)>1) && Uniform()<p) {
            newstate[0][x][y] = 0;
		   newstate[1][x][y] = 1;
        }
		//else
		 //  newstate[1][x][y] = 0;
	      //  break;
	 case 1:
             // newstate[0][x][y]=0;
		//newstate[1][x][y] = 1;
	        break;
      }
 }

 void Report()

 {
     static int frame=0;
     static char filename[100];
     if ((thetime%100)==0) {
         for (int x=0;x<xfield+2;x++) {
         state[0][x][1]=1;
     }
     }
      Diffuse(0);
      if ((thetime%100)==0) DrawField();
      //DrawLayer(0);
      //DrawLayer(1);
     /* if (popdyn) DrawPopDyn(0,0,1,1,500,0,1);
      if ((spacetime)&&(!(thetime%5))) DrawSpaceTime(-1,50);*/
      //if (thetime==100) RecordState();
     if ((thetime%1000)==0) {
         snprintf(filename,99,"frame%05d.png",frame++);
         WriteField(filename);
     }
}

 main()
 {
     MultiLayer();
 }






