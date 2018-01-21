## Custom commands
## Recursive wildcard
rwildcard=$(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *,%,$2),$d))

## Compiler configuration
CC          =  mpicc -fopenmp
CFLAGS		= -fPIC -O0 -ggdb -Wall -fopenmp
LIBS		= -l:libkmeans.a -lm

## Project configuration
INCLUDE			= ./include
SOURCE			= src
SRCEXT			= c
OUTPUT			= build
OBJEXT			= o

# Filesystem folders
FOLDERS			= $(subst ${SOURCE},${OUTPUT}, $(sort $(dir $(wildcard src/*/))))
FOLDERS_T		= $(subst ${OUTPUT},${OUTPUT}/tests/kmeans, ${FOLDERS})

## Project files
SRCFILES		:= $(call rwildcard, , ${SOURCE}/*.${SRCEXT})
OUTFILES		=  $(subst .${SRCEXT},.${OBJEXT}, ${}$(subst ${SOURCE}, ${OUTPUT}, ${SRCFILES}))

## Create the kmeans shared library to be used by external
## applications.
all: output_dirs libkmeans.a kmeans_mpi_app kmeans_seq_app bin_database_writer database_reader_test

## Rule for building the library from source
libkmeans.a: ${OUTFILES}
	ar rcs bin/$@ $^

kmeans_mpi_app: src/kmeans_mpi_app.c
	${CC} $^ -o bin/$@ -I ${INCLUDE} ${CFLAGS} -L./bin ${LIBS}

kmeans_seq_app: src/kmeans_seq_app.c
	${CC} $^ -o bin/$@ -I ${INCLUDE} ${CFLAGS} -L./bin ${LIBS}

bin_database_writer: src/database/bin_database_writer.c
	${CC} $^ -o bin/$@ -I ${INCLUDE} ${CFLAGS} -L./bin ${LIBS}

database_reader_test: src/database/database_reader_test.c
	${CC} $^ -o bin/$@ -I ${INCLUDE} ${CFLAGS} -L./bin ${LIBS}

## Rule for compiling each .c file.
${OUTPUT}/%.o: ${SOURCE}/%.c
	@echo Compiling $^ into $@
	${CC} -c $^ -o $@ -I ${INCLUDE} ${CFLAGS}


## Create all directories
output_dirs:
	@echo Creating output directories
	@mkdir -p ${OUTPUT}
	@mkdir -p ${FOLDERS}
	@mkdir -p bin

## Clear Project
clean:
	@echo Cleaning project
	@rm -rf build
	@rm -rf bin