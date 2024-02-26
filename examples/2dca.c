 extern int xfield,yfield,scale,species,rseed,border,asyn,thetime,graphics,spacetime,popdyn;
 extern char **state,**newstate;
 double Uniform();

 InitConstants()
 {
      xfield    =  100;	 /* horizontal size of the automaton */
      yfield    =  100;	 /* vertical size of the automaton */
      scale    =   2;    /* scale of drawing */
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

      InitSpeciesRandom();
 }

 NextState(x,y) int x,y;
 {
      int sum;

      sum = Moore(x,y);

      if (sum == 3)
	 newstate[x][y] = 1;
      else if (sum == 2)
	 newstate[x][y] = 1;
      else
	 newstate[x][y] = 0;
 }

 Report()
 {
      DrawField(); 
	/* DrawSpaceTime(xfield/2);
	 DrawPopDyn(0,1,1000,xfield*yfield/10,1);*/
      if (thetime%10==0) EchoTime();

	 } 

 main()
 {
     OneLayer();
 }
