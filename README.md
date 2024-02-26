# CelAut 2011 - Cellular Automata Library

This is a simple cellular automata library written in C.

It derived from an old code that was in use at the Theoretical Biology group Utrecht University in the 1990s, and has been often modified since. 


# Cellular automata simulations with CelAut

*CelAut2011* handles both single layer, and multilayer cellular automata
(CA). *eden.c*, *vote.c* en *prime.c* are examples of single layer CA,
whereas *dla.c* and *turing.c* are examples of multilayer CAs. To run a
CA program, type into a terminal:

-   `celaut -o [name]` for *single-layer* CA (e.g., `celaut -o eden`;
    with -o for ’one layer’)

-   `celaut -m [name]` for *multilayer* CA (e.g., `celaut -o turing`;
    with -m for ’multilayer’)

CelAut will respond in the terminal as follows:  
`Enter control:`  
`h` of `?` will give you a list with possible control codes.

To start the simulation of the CA, type: `c`.

Double-click on the CA-window to stop the simulations and enter a new
control code into the terminal Window.

Move to the ‘examples’ folder of your ’celaut’ installation, probably:

`cd ~/Download/celaut/examples`

and type, `celaut -o [MYCA]` to compile and run your CA. For example:

`celaut -o 2dca` (-o voor ”single layer”)

The software responds:

`Enter control:`

Now type `q` (enter) or CTRL-C to exit the program.

Once you have returned to your own machine, run the simulation by again
typing (make sure you are in `CelAut2011/examples`):

`celaut -o 2dca`

The software responds:

`Enter control:`

Type: `h` of `?` for a list of control codes.

Start the simulation of the CA by typing `c`.

To stop the simulation and choose an alternative control code,
double-click on the graphics window.

## Defining your own CA-rules

Most of time you will be defining your own CA-rules. To do so, it’s
easiest to copy one of the examples (e.g., `eden.c`) to a new file, and
edit that. The CA-definition code will call a number of standard
functions of *CelAut2011*.

## Format of CA-definition

`InitConstants();`

> This is were you’ll define a number constants (e.g., parameters)

`InitGraphs();` (only for multilayer CAs)

> This is were you can define some settings for the graphics windows,
> but it’s rarely used in practice.

`InitSpecies();`

> Definition of the initial condition of the CA.

`NextState(x,y) int x,y; `

> Definition of the transition rules for a single cell of the CA. Assign
> the new state to `newstate[x][y]` (single layer) or
> `newstate[k][x][y]` (multilayer), making sure that you always assign a
> new value to `newstate`. A safe way to this is to always include, at
> the beginning of `Nextstate`:
>
> `newstate[x][y]=state[x][y]`
>
> or, for multilayer CA:
>
> `for (int l=0;l<layers;l++) { newstate[l][x][x]=state[l][x][y]; }`
>
> This practice may slow down the code, so if you know what you are
> doing, you can skip this.

`Report(); `

> This function is executed once for each time step. This is were you
> will put the statements to display the CA field or to write
> information to a file. Also, this is the place you will put operators
> that should only be executed once per timestep. The operator
> `Diffuse()` is a good example of this.

`main(); `

> `Onelayer();` or `Multilayer();`
>
> Leave this unchanged.

## Pattern files

If you want to define an initial condition for your CA, you can define a
`pattern file`. It is a text-file with the following format:

`x-pos y-pos state`

To read a pattern file from within `‘celaut’` choose option `’p’` from
the menu.

## Defining your own CA

To define a cellular automata model with your own rules, follow these
steps. First, copy the definition (C-source) of an existing CA to a new
file. For example:

`cp 2dca.c myca.c`

You can check if it work by running the CA (`celaut -o myca`. It should
(of course) produce the same output as your original model. You will now
probably want to edit the CA-rules, which are defined in function
`NextState(x,y)`. During the computer lab session we will discuss the
details.

The rest of the definition is set up as follows:

`InitConstants();`

> Definition of field size, random seeds, boundary conditions, *etc.*
> See comments in source code.

`InitSpecies();`

> This function defines the initial condition of the CA.

`NextState(x,y) int x,y; `

> This function defines the CA-rules. Assign the new state of the CA to
> `newstate[x][y]`. Please make sure to assign a new value to
> `newstate[x][y]` in all cases, for examples by writing
> `newstate[x][y]=state[x][y]` at the start of this function (or better,
> in an `’else’` clause. Otherwise the value of the new state will be
> undefined.

`Report(); `

> This function is carried out once every timestep (i.e. after all
> CA-rules have been applied. It is generally used to write the output,
> or display graphics to the screen.

`main(); `

> `Onelayer();` of `Multilayer();`

## Overview of functions to be used in CelAut

Initialisation functions:

> `InitSpeciesRandom();`
>
> > Assign a random state to each cell.
>
> `ReadPatternFile();`
>
> > read in a pattern file (e.g., ’loper’)

Random functions:

> `int RandomNumber(n);`
>
> > returns an integer random number in \[1,*n*\]
>
> `double Uniform();`
>
> > returns a random, real number in \[0, 1⟩

Neighborhood functions:

> <div class="tabbing">
>
>   
> `Moore(x,y);` Moore(k,x,y);
>
> </div>
>
> > returns the sum of the states of the 8, first and second order
> > neighbors of cell `(x,y)`.
>
> <div class="tabbing">
>
>   
> **Single layer** **Multilayer**  
> `CountMoore(n,x,y);` `CountMoore(k,n,x,y);`
>
> </div>
>
> > returns the number of neighbors in state ’n’ (of the 8 neighbors).
>
> <div class="tabbing">
>
>   
> `RandMoore(x,y);` `RandMoore(k,x,y);`
>
> </div>
>
> > returns the state of a random neighbor (out of eight neighbors)
>
> <div class="tabbing">
>
>   
> `Vonn(x,y);` `Vonn(k,x,y);`
>
> </div>
>
> > returns the sum of the states of the 4, first order neighbors of
> > cell `(x,y)`.

Plotting functions:

<div class="tabbing">

  
`DrawField();` `DrawLayer(k);`  
plots the state to graphics window plots the *k*<sup>*t**h*</sup> layer

</div>

<div class="tabbing">

  
`WriteField([filename.png]);` `WriteLayer([filename.png],k);`  
Plots the state to file \[filename\] plots the state of the
*k*<sup>*t**h*</sup> layer to file \[filename\]  

</div>

> In multiple layers, use `InitGraphs();` to tell the system what layer
> must be drawn in what window, as follows:
>
> `graph[w]=k; ` tells the system that layer `k` will be drawn in window
> `w`.

`DrawPopDyn(n1,n2,xaxis,yaxis,color); `  
`DrawPopDyn(k1,n1,k2,n2,xaxis,yaxis,color); `  

> This function graphs the numbers of species while the simulation is
> running.
>
> `n1` is the species on the x-axis, with `n1` on layer `k1`. Use `n1=0`
> to put time on the x-axis. `n2` is the species on the y-axis, k2 is
> the layer on which `n2` is defined.
>
> The x-axis runs from 0 to `xaxis`; the y-axis runs from 0 to y-axis.
> Use `xaxis=0` or `yaxis=0` to set the axis dimensions automatically.
> `colour` gives the color of the plot.
>
> To use this function, in `InitConstants();` add: `popdyn=1; `, and add
> to the list `extern int`: `popdyn`.

<div class="tabbing">

  
`DrawSpaceTime(y);` `DrawSpaceTime(k,y); `

</div>

> Produces a “space time window” (or kymograph) of the CA at position
> ’y’.
>
> Assing the desired length of the space time window is to the constant
> `spacetime` in `InitConstants();`. Also add `spacetime` to the list
> `extern int`.

## Functions writing to your terminal:

> `EchoTime();`
>
> > Writes the current time
>
> `CountSpecies();`
>
> > Writes the current numbers of species

## Functions writing to a file:

> `RecordNumber();`
>
> > Writes the numbers of cells that are in each state to file
> > `num.dat`.
>
> <div class="tabbing">
>
>   
> `RecordGrowth(n);` `RecordGrowth(k,n);`
>
> </div>
>
> > Writes the number and growth per individual of state ’n’ to the file
> > ’growth.n’
>
> `RecordState();`
>
> > Writes the current states to file ’`state.time`’

## Other functions:

> <div class="tabbing">
>
>   
> `Diffuse();` `Diffuse(k);`
>
> </div>

> Perform a Margolus diffusion step

<div class="tabbing">

  
`DiffuseStop(k,l);`

</div>

> Performs a Margolus diffusion step; no diffusion at `(x,y)` if there
> is a `1` in layer ’l’ at `(x,y)`.

<div class="tabbing">

  
`ReShuffle();` `ReShuffle(k);`

</div>

> Mixes all states in the CA.
