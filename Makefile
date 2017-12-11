# $Id: Makefile,v 1.5 2002/10/25 06:48:35 kingofgib Exp $
CC              =  mpicc -DStandAlone
CCOPTFLAGS      = -O2
CCDEBUGFLAGS    = -g
CCWARNFLAGS     = 


CFLAGS		= $(CCOPTFLAGS) $(CCWARNFLAGS)

OBJ 	=  app.o kmeans.o kmeans_mpi.o database_struct.o database_handler.o array.o write_database_bin.o database_bin_handler_mpi.o
PROGS	=  app write_database_bin

.SUFFIXES:	.o

all:
	@-$(MAKE) CC="mpicc -fopenmp -pipe -DStandAlone" \
	CCWARNFLAGS="-Wall -pedantic -W -Wshadow \
	-Wnested-externs -Wwrite-strings -Wpointer-arith \
	-Wmissing-declarations -Wredundant-decls -Winline \
	-Wstrict-prototypes" $(PROGS);
	make clean_obj

app: app.o kmeans.o kmeans_mpi.o database_handler.o database_bin_handler_mpi.o database_struct.o array.o
	$(CC) $(CFLAGS) $@.o kmeans.o kmeans_mpi.o database_handler.o database_bin_handler_mpi.o database_struct.o array.o -o $@ -g

write_database_bin: write_database_bin.o database_handler.o database_struct.o array.o
	$(CC) $(CFLAGS) $@.o database_handler.o database_struct.o array.o -o $@ -g

kmeans:	kmeans.o database_handler.o database_struct.o array.o
	$(CC) $(CFLAGS) $@.o database_handler.o database_struct.o array.o -o $@ -g

kmeans_mpi: kmeans_mpi.o database_bin_handler_mpi.o database_struct.o array.o
	$(CC) $(CFLAGS) $@.o database_bin_handler_mpi.o database_struct.o array.o -o $@ -g

database_handler: database_handler.o database_struct.o array.o
	$(CC) $(CFLAGS) $@.o database_struct.o array.o -o $@ -g

database_bin_handler_mpi: database_bin_handler_mpi.o database_struct.o array.o
	$(CC) $(CFLAGS) $@.o database_struct.o array.o -o $@ -g

database_struct: database_struct.o array.o
	$(CC) $(CFLAGS) $@.o array.o -o $@ -g

%.o:	%.c
	$(CC) $(CFLAGS) -c $*.c -g

.PHONY: clean

clean_obj:
	@-rm -f core *.e *.o *.obj *.ilk *.ncb *.pdb *.pch; \
	rm -f *~ $(OBJ)

clean:
	@-echo "Cleaning directory "
	@-rm -f core *.e *.o *.obj *.ilk *.ncb *.pdb *.pch; \
	rm -f *~ $(PROGS) $(OBJ)
