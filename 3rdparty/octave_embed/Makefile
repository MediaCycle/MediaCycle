
MKOCTFILE=mkoctfile

LD=$(MKOCTFILE)
LDFLAGS=-v --link-stand-alone
CXX=$(MKOCTFILE)
CXX_FLAGS=-v
RM=rm -f

all: test_embed

test_embed: test.o embed.o
	$(LD) $(LDFLAGS) test.o embed.o -o test_embed

test.o: test.cpp embed.h
	$(CXX) $(CXX_FLAGS) -c test.cpp

embed.o: embed.cc embed.h
	$(CXX) $(CXX_FLAGS) -c embed.cc

clean:
	$(RM) embed.o test.o test_embed *~ core octave-core
