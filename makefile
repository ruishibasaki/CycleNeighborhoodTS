# compiler
CC = g++

# module name
NAME = tabu

# basic directory
DIR = ./

# debug switches
#SW = -g -Wall -fPIC -fexceptions -DNDEBUG -DIL_STD
# production switches
SW = -O3

# default target- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

default: $(NAME)


# clean - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

clean::
	rm -f $(DIR)*.o $(DIR)*~ $(NAME)

# define & include the necessary modules- - - - - - - - - - - - - - - - - - -
# if a module is not used in the current configuration, just comment out the
# corresponding include line
# each module outputs some macros to be used here:
# *OBJ is the final object(s) / library
# *LIB is the external libreries + -L<libdirs>
# *H   is the list of all include files
# *INC is the -I<include directories>

#structures
OUTILH = ./Outils/estruturas.hpp
OUTILINC = -I./Outils/

#solver
SOLVERDIR = ./Solver/
include $(SOLVERDIR)makefile

#tabu search
TABUDIR = ./TabuSearch/
include $(TABUDIR)makefile

#nighbor generator
NHGENDIR = ./NeighborGen/
include $(NHGENDIR)makefile

# main module (linking phase) - - - - - - - - - - - - - - - - - - - - - - - -

# object files

MOBJ =	$(SOLVEROBJ)\
		$(TABUOBJ)\
		$(NHGENOBJ)\

# libreries
MLIB = $(SOLVERLIB)\
	

# include dirs
MINC =	$(SOLVERINC)\
		$(TABUINC)\
		$(NHGENINC)\
		$(OUTILINC)\

# includes
MH =	$(SOLVERH)\
		$(NHGENH)\
		$(TABUH)\
		$(OUTILH)\
		
	
$(DIR)main.o: $(DIR)main.cpp $(MH)
	$(CC) -c $*.cpp -o $@ $(MINC) $(SW)

$(NAME): $(MOBJ) $(DIR)main.o
	$(CC) -o $(NAME) $(DIR)main.o $(MOBJ) $(MLIB) $(SW)



