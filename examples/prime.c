 extern int xfield,yfield,scale,species,rseed,border,asyn,thetime,graphics,spacetime;
 extern char **state,**newstate;
 double Uniform();

 InitConstants()
 {
      xfield   =  100;	 /* x size of the automaton */
      yfield   =  100;	 /* y size of the automaton */
      scale    =    4;    /* size of drawing */
      species  =    5;    /* number of states; range 1-256 */
      rseed    =    1;    /* seed of the randomgenerator */
      border   =    1;    /* 0 = empty borders; 1 = torus; 2 = echoing borders */
      asyn     =    0;    /* 1 = asynchronous updating; 0 = synchronous */
      graphics =    1;    /* 0 = program runs silently */
 }


 InitSpecies()
 {
      ReadPatternFile();
 }

 NextState(x,y) int x,y;
 {
      newstate[x][y] = Moore(x,y) % species;
 }

 Report()
 {  
      if (thetime%species==0) {
         EchoTime();
         DrawField();
      }
}

 main()
 {
     OneLayer();
 }
