# Makefile  for line models

LIBDIR = /usr/lib
INCDIR = /usr/include
STLDIR = /usr/include

#CFLAGS = -g -I$(INCDIR) -I$(STLDIR) -L$(LIBDIR) -Wno-deprecated -I/Users/das/source/jpeg-6b
#
CFLAGS = -O3 -I$(INCDIR) -I$(STLDIR) -L$(LIBDIR) -Wno-deprecated -I/Users/das/source/jpeg-6b
LDFLAGS = -L/Users/das/source/jpeg-6b
# LIBS   = -lm -llibjpeg -lpthread
LIBS   = -lm -ljpeg -lpthread
OBJS  = control.o s_hull.o
HEADS = s_hull.h

CC = gcc


# Building rules for Makefile

shull: $(OBJS) $(HEADS)
	g++ $(CFLAGS) $(LDFLAGS) -o $@ $(OBJS)  $(LIBS)

%.o: %.C
	g++ $(CFLAGS) -c  $< -o $@

clean:
	-rm *.o shull

control.o:	control.C $(HEADS)

s_hull.o:	s_hull.C $(HEADS)




