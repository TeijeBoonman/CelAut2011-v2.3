#include <stdlib.h>
 extern int
xfield,yfield,scale,species,rseed,border,asyn,thetime,layers,graphics,popdyn,spacetime;
 extern char ***state,***newstate;
 extern int *graph;
 double Uniform();



 void InitConstants()
 {
      xfield   = 500;	   /* horizontal size of the automaton */
      yfield   = 500;	   /* vertical size of the automaton */
      scale    =   1;      /* scale of drawing */
      species  =   2;	   /* number of states; range 1-256 */
      rseed    =   914858;	   /* seed of the randomgenerator */
      border   =   1;	   /* 0 = empty borders; 1 = torus; 2 = echoing borders */
      asyn     =   1;	   /* 1 = asynchronous updating; 0 = synchronous */
      layers   =   3;	   /* number of layers */
      graphics =   1;      /* 0 = pograms runs silently */
      popdyn   =   0;
      spacetime =  0;
}

InitGraphs()
{
}


 void InitSpecies()

 {
      int x,y,k;
     int r=1300;
     //ReadLayerPNG("patella.png");
	InitRandomLayer(1);
	InitRandomLayer(2);
     int xc[2],yc[2];
     xc[0]=xfield/2;
     xc[1]=xfield/2 + xfield/4 + r;
     yc[0]=yfield+r*0.86;
     yc[1]=yfield/2;
     
     for (int q=0;q<1;q++) {
      //   int xc=RandomNumber(xfield);
        // int yc=RandomNumber(yfield);
	for (x = 0; x < xfield; x++){
	for ( y = 0; y < yfield; y++){
        if (((x-xc[q])*(x-xc[q])+(y-yc[q])*(y-yc[q]))<r*r) {
		state[0][x][y] = 1;
        }
	}
	}
     }
 }

 void NextState(x,y) int x,y;

 {
        newstate[0][x][y] = state[0][x][y];
	newstate[1][x][y] = state[1][x][y];
     newstate[2][x][y]=state[2][x][y];
     //double p1=0.02,p2=0.02;
     double p1=0.03,p2=0.02;
     if (state[0][x][y]==1) {
        // p1=0.04;
         //p2=0.04;
         p1=0.03; p2=0.1; //dichte - homogene distributie
	//p1=0.02, p2=0.02; // "wormen"
      
     }
	//if (((double)rand()/(double)RAND_MAX)<p1)
     if (Uniform()<p1)
	{newstate[1][x][y]=0;
        }
  
	//if (((double)rand()/(double)RAND_MAX)<p2) // 0.05
     if (Uniform()<p2)
	{newstate[2][x][y]=0;
        }

	if (CountMoore(1,1,x,y)-CountMoore(2,1,x,y)>3)
		{
			newstate[1][x][y]=1;
		}
	if (CountMoore(1,1,x,y)-CountMoore(2,1,x,y)>3)
		{
			newstate[2][x][y]=1;
		}


 }


 void Report()

 {
	//if(thetime%2==1){Diffuse(0);Diffuse(1);Diffuse(1);}
	Diffuse(1);
	
     
     for (int i=0;i<10;i++) {
         Diffuse(2);
     }
     /* Diffuse(1);
	Diffuse(1);Diffuse(1);*/
	
     if ((thetime%50)==0) {
      DrawField2();
         char filename[100];
         snprintf(filename,99,"field-0%06d.png",thetime);
         WriteFieldLayerOffset(filename,1);
         snprintf(filename,99,"field-layer1-0%06d.png",thetime);
         WriteLayer(filename,1);
         snprintf(filename,99,"field-layer2-0%06d.png",thetime);
         WriteLayer(filename,2);
         
         
     }
      //DrawLayer(0);
      //DrawLayer(1);
      //if (popdyn) DrawPopDyn(0,0,1,1,500,0,1);
      //if ((spacetime)&&(!(thetime%5))) DrawSpaceTime(-1,50);
      //if (thetime%10==1) getchar();
 	
}

 main()
 {
     MultiLayer();
 }






