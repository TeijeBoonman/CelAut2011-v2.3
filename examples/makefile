X11INCLUDE = /usr/X11R6/include
X11LIB = /usr/X11R6/lib

.c:
	cc -O2 -o $@ $< -I$(X11INCLUDE) -L$(X11LIB) $(CELAUTLIB) -lX11 -lm -lpng

clean:
	rm -if `find * -prune -type f -perm -700 -print`
