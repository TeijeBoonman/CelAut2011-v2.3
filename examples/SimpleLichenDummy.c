#include <stdio.h>
#include <stdlib.h>
#include <math.h>
extern int xfield,yfield,scale,species,rseed,border,asyn,thetime,graphics,spacetime,popdyn;
 extern char **state,**newstate;
 double Uniform();

 InitConstants()
 {
      xfield    =  300;	 /* horizontal size of the automaton */
      yfield    =  300;	 /* vertical size of the automaton */
      scale    =   1;    /* scale of drawing */
      species  =   4;    /* number of states; range 1-256 */
      rseed    =   2;    /* seed of the randomgenerator */
      border   =   1;    /* 0 = empty borders; 1 = torus; 2 = echoing borders */
      asyn     =   0;    /* 1 = asynchronous updating; 0 = synchronous */
      graphics =   0;    /* 0 = program runs silently */
	 spacetime = 0;    /* length of space/time plot; 0 = no space time plot */
	 popdyn = 0;         /* 1 = population dynamics plot; 0 = no pop.dyn. plot */
 }

 InitSpecies()
 {
      int x,y;
      InitSpeciesRandom();
	  /* for(x=1;x<=xfield;++x){
		  for(y=1;y<=yfield;++y){
			  double r=Uniform();
			  if(r<0.5) state[x][y]=0;
			  if(r<0.75 && r>0.5) state[x][y]=1;
			  if(r>0.75) state[x][y]=2;
		  }
	  } */
	   /* state[xfield/2][yfield/2]=1;
	  state[1+xfield/2][1+yfield/2]=1;
	  state[xfield/2][1+yfield/2]=2;
	  state[1+xfield/2][yfield/2]=2;
	  state[3*xfield/5][yfield/2]=3;
	  state[1+3*xfield/5][yfield/2]=3;
	  state[3*xfield/5][1+yfield/2]=3;
	  state[1+3*xfield/5][1+yfield/2]=3;  */
}

 int 	AlgalPartner		=2, 			FungalPartner			=1,			ParasiteTarget		=1;
 double AlgalSymbioticGrowth=0.125, 			FungalSymbioticGrowth	=0.1,		ParasiticGrowth		=0.2;
 double AlgalSoloGrowth		=0.01, 			FungalSoloGrowth		=0.01, 		ParasiteSoloGrowth	=0.000;
 double	AlgalDeath			=0.008,			FungalDeath				=0.008,		ParasiteDeath		=0.005;
 double ParasiteAbundance= 0.01;
 int LateIntroduction=1, Square=0;
 int DiffusionFactorDown=10; 
 int framenumber=3;
 int PhotoHz=10000, StateHz=10000;
 double KsiSensitivity=0.05;
 
 NextState(x,y) int x,y;
 {
	 double r;
	 //Dead cell logic
if((thetime==100 && LateIntroduction==1) || (thetime==0 && Square==1)){
			 r=Uniform();
		if(r<ParasiteAbundance){
			newstate[x][y]=3;
		}
		else{
			newstate[x][y]=state[x][y];
		} 
		/* if((x>2*xfield/3 || x<xfield/3 || y>2*yfield/3 || y<yfield/3) && !(x==10+2*xfield/3 && y<2*yfield/3 && y>yfield/3)){
			newstate[x][y]=0;
		}
		else{
			if(x==10+2*xfield/3 && y<2*yfield/3 && y>yfield/3){
				newstate[x][y]=3;
			}
			else{
				newstate[x][y]=state[x][y];
			}
		} */
}
else{
	if(state[x][y]==0) {
		int BooleanGrowth=0;
		int copystate;
		copystate=RandMoore(x,y);
		switch(copystate){
			case 0:
			newstate[x][y]=0;
			break;
			//if the random neighbour is algal
			case 1:
			r=Uniform();
				if(CountMoore(AlgalPartner,x,y)>0){
					if(r<AlgalSymbioticGrowth) {
						newstate[x][y]=1;
						BooleanGrowth=1;
					}
				}
				else{
					if(r<AlgalSoloGrowth){
						newstate[x][y]=1;
						BooleanGrowth=1;
					}
				}
				break;
			//if fungal
			case 2:
			r=Uniform();
				if(CountMoore(FungalPartner,x,y)>0){
					if(r<FungalSymbioticGrowth) {
						newstate[x][y]=2;
						BooleanGrowth=1;
					}
				}
				else{
					if(r<FungalSoloGrowth){
						newstate[x][y]=2;
						BooleanGrowth=1;
					}
				}
				break;
				//if parasite
				case 3:
			r=Uniform();
				if(CountMoore(ParasiteTarget,x,y)>0){
					if(r<ParasiticGrowth) {
						newstate[x][y]=3;
						BooleanGrowth=1;
					}
				}
				else{
					if(r<ParasiteSoloGrowth){
						newstate[x][y]=3;
						BooleanGrowth=1;
					}
				}
				break;
		}
		if(BooleanGrowth==0) newstate[x][y]=0;
	}
	//Algal cell logic
	if(state[x][y]==1){
		
		r=Uniform();
		if(r<AlgalDeath){
			newstate[x][y]=0;
		}
		else{
			newstate[x][y]=state[x][y];
		}
	}
	//fungal cell logic
		if(state[x][y]==2){
		
		r=Uniform();
		if(r<FungalDeath){
			newstate[x][y]=0;
		}
		else{
			newstate[x][y]=state[x][y];
		}
	}
	//parasite cell logic
		if(state[x][y]==3){
		r=Uniform();
		if(r<ParasiteDeath){
			newstate[x][y]=0;
		}
		else{
			newstate[x][y]=state[x][y];
		}
	}
}
 }
 Report()
 {
	 if((thetime>1 || Square==0)&&thetime%DiffusionFactorDown==0) Diffuse();
//if(thetime%PhotoHz==0)DrawField();
if(thetime%PhotoHz==0)ReportImage();
	  //if(thetime>1&&thetime%StateHz==0) PrintState();
	if(thetime>1&&thetime%StateHz==0) SpatialCorrelation();
	 
	  //if (thetime%100==0) EchoTime();
	  //if (thetime%100==0) CountSpecies();
	   /*if(thetime%PhotoHz==0){
		ReportImage();
	  }*/ 
	//ReportCount();
 } 
 
 ReportImage()
 {
	 		  /* int Number;
		  Number=1+thetime/PhotoHz;
		  if(Number<framenumber){
			char naam[1024];
			if(Number<10)sprintf(naam, "naam-0000%d.png",Number);
			if(Number>=10 &&Number<100)sprintf(naam, "naam-000%d.png", Number);
			if(Number>=100 &&Number<1000)sprintf(naam, "naam-00%d.png", Number);
			if(Number>=1000&&Number<10000)sprintf(naam, "naam-0%d.png",  Number);
			if(Number>=10000)sprintf(naam, "naam-0%d.png",  Number); */
			char name[1024];
			snprintf(name, 1023, "%d-%d-%d-%f-%f-%f-%f-%f-%f-%f-%f-%f.png",rseed, thetime, DiffusionFactorDown, AlgalSymbioticGrowth, FungalSymbioticGrowth, ParasiticGrowth, AlgalSoloGrowth, FungalSoloGrowth, ParasiteSoloGrowth, AlgalDeath, FungalDeath, ParasiteDeath );
			WriteField(name);
		  /*}
		  else{
			  exit(0);
		  }*/
 }
 ReportCount()
 {
	  int PopulationSize[]= {0,0,0,0};
	  int x,y,i;
	  for(i=0; i<species;++i){
		  for(x=1;x<xfield;++x){
			  for(y=1;y<yfield;++y){
				  if(state[x][y]==i){
					  ++PopulationSize[i];
				  }
			  }
		  }
	  }

/* FILE *times;
			times = fopen("longoutputs.txt","a");
				if(times==NULL) printf("Null error\n");
				fprintf(times, "\n%d, %d, %d, %d, %d, %d, %f, %f, %f, %f, %f, %f, %f",thetime, PopulationSize[0], PopulationSize[1], PopulationSize[2], PopulationSize[3], DiffusionFactorDown, AlgalSymbioticGrowth, FungalSymbioticGrowth, ParasiticGrowth, AlgalSoloGrowth, ParasiteSoloGrowth, AlgalDeath, ParasiteDeath); //prints all relevant values. No header implemented yet.
				fclose(times);	 */
if(thetime==100000) exit(0);				
 }
 PrintState()
 {
	 FILE *fp;
     char name[50];
     int  x,y,statevalue, count=0;

     sprintf(name,"%d.txt",thetime);
     fp = fopen(name,"w");
	 for(y=1;y<=yfield;++y){
		 for(x=1;x<=xfield;++x){
				statevalue=state[x][y];
				if(x<xfield) fprintf(fp, "%d,",statevalue);
				if(x==xfield) fprintf(fp, "%d",statevalue);
		 }
		fprintf(fp, "\n");
		++count;
	 }
	 fclose(fp);
	 printf("%d", count);
	 if(thetime==100000) exit(0);		
 }
 SpatialCorrelation()
 {
	 double outputvector[1+yfield/2];
	 double outputvalue=0, Normal;
	 int d,x,y,count, guide;
	 for(d=0; d<=yfield/2;++d){
		 outputvector[d]=0;
		 outputvalue=0;
		 for(x=1;x<=xfield;++x){
			 for(y=1;y<=yfield;++y){
				 if(state[x][y]!=0){
					 for(count=1;count<=4;count++){
					 int bx=x;
					 int by=y;
						 if(count==1){
							 by=by-d;
							 if(by<1) by=by+yfield;
							 if(by>300) by=by-yfield;
							 for(guide=x-d; guide<=x+d; ++guide){
								 bx=guide;
								 if(bx<1)bx=bx+xfield;
								 if(bx>300)bx=bx-xfield;
								 if(state[x][y]==state[bx][by]) ++outputvalue;
								 ++Normal;
							 }
						 }
						 if(count==2){
							 by=by+d;
							 if(by<1) by=by+yfield;
							 if(by>300) by=by-yfield;
							 for(guide=x-d; guide<=x+d; ++guide){
								 bx=guide;
								 if(bx<1)bx=bx+xfield;
								 if(bx>300)bx=bx-xfield;
								 if(state[x][y]==state[bx][by]) ++outputvalue;
								 ++Normal;
							 }
						 }
						 if(count==3){
							 bx=bx-d;
							 if(bx<1) bx=bx+xfield;
							 if(bx>300) bx=bx-xfield;
							 for(guide=y-d+1; guide<=y+d-1; ++guide){
								 by=guide;
								 if(by<1)by=by+yfield;
								 if(by>300)by=by-yfield;
								 if(state[x][y]==state[bx][by]) ++outputvalue;
								 ++Normal;
							 }
						 }
						 if(count==4){
							 bx=bx+d;
							 if(bx<1) bx=bx+xfield;
							 if(bx>300) bx=bx-xfield;
							 for(guide=y-d+1; guide<=y+d-1; ++guide){
								 by=guide;
								 if(by<1)by=by+yfield;
								 if(by>300)by=by-yfield;
								 if(state[x][y]==state[bx][by]) ++outputvalue;
								 ++Normal;
							 }
						 }
					 }
				 }
			 }
		 }
		 //if(d>1) outputvalue=outputvalue/(((2*d+1)*4-4));
		 outputvalue=outputvalue/Normal;
		 Normal=0;
		 outputvector[d]=outputvalue;
		 printf("%d %f\n", d, outputvector[d]);
	 }
	 double BinArray[11];
	 int i,j;
	 for(i=0; i<=5; ++i){
		 BinArray[i]=0;
		 double TotalHeight=0;
		 for(j=1+25*i;j<=25*i+25;++j){
			 TotalHeight=TotalHeight+outputvector[j];
		 }
		 TotalHeight=TotalHeight/25;
		 BinArray[i]=fabs((outputvector[1+25*i]-outputvector[25*i+25])/TotalHeight);
	 }
	 int Found=0;
	 int BinNumber=-2;
	 for(i=1; i<=5; ++i){
		 if(BinArray[i]<KsiSensitivity && Found==0){
			 Found=1;
			 BinNumber=i-1;
		 }
	 }
	 printf("ksi found in big bin %d\n", BinNumber+1);
	 for(i=0; i<=5; ++i){
		  BinArray[i]=0;
		  double TotalHeight=0;
		 for(j=BinNumber*25+1+5*i;j<=5*i+5+BinNumber*25;++j){
			 TotalHeight=TotalHeight+outputvector[j];
		 }
		 TotalHeight=TotalHeight/5;
		 BinArray[i]=fabs((outputvector[BinNumber*25+1+5*i]-outputvector[5*i+5+BinNumber*25])/TotalHeight);
	 }
	 int Found2=0;
	 int SmallBinNumber=-2;
	 for(i=1; i<=10; ++i){
		 if(BinArray[i]<KsiSensitivity && Found2==0){
			 Found2=1;
			 SmallBinNumber=i-1;
		 }
	 }
	 printf("ksi found in small bin %d\n", SmallBinNumber+1);
	 int FinalRangeMin=1+25*BinNumber+5*SmallBinNumber;
	 int FinalRangeMax=4+FinalRangeMin;
	 printf("Final ksi range from %d to %d,", FinalRangeMin, FinalRangeMax);
	 for(d=0; d<=yfield/2; ++d){
		 	 FILE *fp;
     char name[500];
     sprintf(name,"%d-%d-%d-%f-%f-%f-%f-%f-%f-%f-%f-%f.txt",rseed, thetime, DiffusionFactorDown, AlgalSymbioticGrowth, FungalSymbioticGrowth, ParasiticGrowth, AlgalSoloGrowth, FungalSoloGrowth, ParasiteSoloGrowth, AlgalDeath, FungalDeath, ParasiteDeath );
     if(d==0)fp = fopen(name,"w");
	 if(d!=0)fp = fopen(name,"a");
	 if(d==0)fprintf(fp, "%d,%d,", FinalRangeMin, FinalRangeMax);
	 if(d!=yfield/2) fprintf(fp, "%f,", outputvector[d]);
	 if(d==yfield/2) fprintf(fp, "%f\n", outputvector[d]);
	 fclose(fp);	
	 }
	 
	 if(thetime==100000) exit(0);	
 }
 
 main()
 {
     OneLayer();
 }
