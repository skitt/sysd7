CC = gcc68
CFLAGS = -O2 -Wall -I/home/sysd7/source/include -L/home/sysd7/source/lib -DSDPMotorola
AR = ar68

.SUFFIXES: .c .o .a

# What we're going to build (OBJS is optional, make can figure that
# out from the LIBS and EXES targets
LIBS = libsupport.a
EXES =

# Standard object file target

.c.o:
	$(CC) $(CFLAGS) -c -o $*.o $*.c

# Top targets
all: $(OBJS) $(LIBS) $(EXES)

# Library target (change to suit)
LIBSUPPORTOBJS = puts.o putn.o
libsupport.a: $(LIBSUPPORTOBJS)
	$(AR) r $*.a $(LIBSUPPORTOBJS)

# Executable target
MAKETESTOBJS = test.o
maketest: $(MAKETESTOBJS)
	$(CC) $(CFLAGS) -o $*.e $(MAKETESTOBJS) -lsupport

# Install target (to copy the libraries)
install:
	cp $(LIBS) /home/sysd7/source/lib
