##############################################
#             Table of Contents              #
# 1: Basic Flags in the compile and linking  #
# 2: where to put and to find files?         #
# 3: Compiling and Linking                   #
##############################################


# Part 1: Basic Flags in the compile and linking
##############################################
#             Table of Contents              #
# 1: Basic Flags in the compile and linking  #
# 2: where to put and to find files?         #
# 3: Compiling and Linking                   #
##############################################

# Part 1: Basic Flags in the compile and linking
CC = mpiicc

#MPI_INCLUDE := $(shell mpicc -showme:compile)
#MPI_LINK := $(shell mpicc -showme:link)

#INCLUDE = -Iinclude $(MPI_INCLUDE)
#LDLIBS  =  $(MPI_LINK) -lm -L./lib/sha256_intel_avx/ -lsha256_avx 

INCLUDE = -Iinclude
LDLIBS  =  -lm -L./lib/sha256_intel_avx/ -lsha256_avx 

LDFLAGS = -march=native -fno-strict-aliasing -O3 -flto -qopenmp -pthread
CFLAGS =  -march=native -fno-strict-aliasing -O3 -flto -qopenmp -Wall
#CFLAGS += -DVERBOSE_LEVEL=2 



# Part 2: where to put and to find files?
# store *.o files in obj/
OBJDIR = obj

# all source files can be found in these folders
SRC = src
SRC_UTIL = $(SRC)/util
#SRC_SHA256 = $(SRC)/sha256

## extract all *.c filenames from the directories
FILENAMES  =  $(wildcard $(SRC)/*.c)
FILENAMES +=  $(wildcard $(SRC_UTIL)/*.c)

# Substitution References: replaces all *.c with *.o
# note that it will keep the directory structure
OBJECTS := $(FILENAMES:$(SRC)/%.c=$(OBJDIR)/%.o)



# Part 3: Compiling and Linking

lib:
	cd lib/sha256_intel_avx && make clean && make all

# BUILD OBJECT FILES IN OBJECTDIR directory
$(OBJDIR)/%.o: $(SRC)/%.c
	mkdir -p '$(@D)'
	$(CC) -c $< $(INCLUDE) $(CFLAGS)  -o $@




TARGETS = phase_ii

# REMOVE TARGETS FROM $(OBJECTS)
TARGET_OBJECTS = $(addprefix $(OBJDIR)/,  $(addsuffix .o, $(TARGETS)) )
COMMON_OBJECTS = $(filter-out $(TARGET_OBJECTS), $(OBJECTS) )

$(info common objects = $(COMMON_OBJECTS))

all: $(TARGETS) lib
	mkdir -p obj
	mkdir -p data
	mkdir -p data/digests
	mkdir -p data/messages
	mkdir -p data/digests
	mkdir -p data/messages
	mkdir -p data/stats
	mkdir -p data/verify




# we wish to build X:
# 1- remove all $(TARGETS) members from dependencies
# 2- add X.o as a dependency

phase_ii: $(OBJDIR)/phase_ii.o $(COMMON_OBJECTS)
	$(CC)  $^ $(LDFLAGS) $(LDLIBS) -o $@ 



.PHONY: clean
clean:
	rm -f $(OBJECTS)
	rm -f $(TARGETS)
purge:
	rm -f $(OBJECTS)
	rm -f $(TARGETS)
	rm -rf data/digests
	rm -rf data/messages
	rm -rf data/stats
	rm -rf data/verify
