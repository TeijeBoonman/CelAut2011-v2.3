#include <math.h>
#include <stdio.h>
#include "misc.h"
#define MBIG 1000000000
#define MSEED 161803398
#define MZ 0
#define FAC (1.0/MBIG)
struct coor {
              int x;
              int y;
};

int idum = -1;
long idum2 = -1;

double Uniform /*ran3*/()
{
  static int inext,inextp;
  static long ma[56];
  static int iff=0;
  long mj,mk;
  int i,ii,k;

  if (idum < 0 || iff == 0) {
    iff=1;
    mj=MSEED-(idum < 0 ? -idum : idum);
    mj %= MBIG;
    ma[55]=mj;
    mk=1;
    for (i=1;i<=54;i++) {
      ii=(21*i) % 55;
      ma[ii]=mk;
      mk=mj-mk;
      if (mk < MZ) mk += MBIG;
      mj=ma[ii];
    }
    for (k=1;k<=4;k++)
      for (i=1;i<=55;i++) {
        ma[i] -= ma[1+(i+30) % 55];
        if (ma[i] < MZ) ma[i] += MBIG;
      }
    inext=0;
    inextp=31;
    idum=1;
  }
  if (++inext == 56) inext=1;
  if (++inextp == 56) inextp=1;
  mj=ma[inext]-ma[inextp];
  if (mj < MZ) mj += MBIG;
  ma[inext]=mj;
  return mj*FAC;
}

#undef MBIG
#undef MSEED
#undef MZ
#undef FAC

int RandomNumber(int number)
{
    int temp;

    if (number <= 0)
       return(0);
    else {
       temp = number*Uniform() + 1;
       return(temp);
    }
}

double gasdev()
{
  static int iset=0;
  static double gset;
  double fac,r,v1,v2;
  double Uniform();

  if  (iset == 0) {
    do {
      v1=2.0*Uniform()-1.0;
      v2=2.0*Uniform()-1.0;
      r=v1*v1+v2*v2;
    } while (r >= 1.0);
    fac=sqrt(-2.0*log(r)/r);
    gset=v1*fac;
    iset=1;
    return v2*fac;
  } else {
    iset=0;
    return gset;
  }
}

#define IB1 1
#define IB2 2
#define IB5 16
#define IB18 131072

unsigned long iseed=55;

int boolean() /*irbit1*/
{
  unsigned long newbit;
  newbit = (iseed & IB18) >> 17
    ^ (iseed & IB5) >> 4
    ^ (iseed & IB2) >> 1
    ^ (iseed & IB1);
  iseed=(iseed << 1) | newbit;
  return (int) newbit;
}

#undef IB1
#undef IB2
#undef IB5
#undef IB18

double Normal(double mean,double stdev)
{
  return gasdev()*stdev + mean;
}

int SetSeed(int seed)
{
  int i;
  idum = -seed;
  idum2 = -seed;
  iseed = seed;
  for (i=0; i<100; i++)
    Uniform();
  for (i=0; i<100; i++)
    boolean();
  return seed;
}

