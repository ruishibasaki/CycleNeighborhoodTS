##############################################################################
################################ makefile ####################################
##############################################################################
#									     #
#   makefile of Solver cplex							     #
#									     #
#   Input:  $(CC)     = compiler command				     #
#           $(SW)     = compiler options                                     #
#           $(SOLVERDIR) = the directory where the TestFi source is	     #
#									     #
#   Output: $(SOLVEROBJ) = the final object(s) / library			     #
#           $(SOLVERH)   = the .h files to include for TestFi	  	     #
#           $(SOLVERINC) = the -I$(TestFi directory)			     #
#									     #
#                                VERSION 2.00				     #
#                	        20 - 02 - 2013				     #
#									     #
##############################################################################

# ---------------------------------------------------------------------
# Cplex and Concert paths
# ---------------------------------------------------------------------

SYSTEM = x86-64_linux
LIBFORMAT = static_pic


#CPLEX DIRS
CPLEXDIR = /usr/local/cplex/CPLEX_Studio/cplex
CONCERTDIR = /usr/local/cplex/CPLEX_Studio/concert
CPLEXBINDIR = $(CPLEXDIR)/bin/$(SYSTEM)
CPLEXLIBDIR   = $(CPLEXDIR)/lib/$(SYSTEM)/$(LIBFORMAT)
CONCERTLIBDIR = $(CONCERTDIR)/lib/$(SYSTEM)/$(LIBFORMAT)

CPLEXINC = -DIL_STD -I$(CPLEXDIR)/include   -I$(CONCERTDIR)/include

CPLEXFLAGS = $(CPLEXINC) -L$(CPLEXLIBDIR) -lilocplex -lcplex -L$(CONCERTLIBDIR) -lconcert -lm -m64 -lpthread

# clean - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

clean::
	rm -f $(SOLVERDIR)*.o $(SOLVERDIR)*~
	


# macroes to be exported- - - - - - - - - - - - - - - - - - - - - - - - - - -

SOLVERLIB = -L$(CPLEXLIBDIR) -lilocplex -lcplex -L$(CONCERTLIBDIR) -lconcert -lm -m64 -lpthread
SOLVEROBJ = $(SOLVERDIR)Solver.o
SOLVERH   = $(SOLVERDIR)Solver.hpp
SOLVERINC = -I$(SOLVERDIR) $(CPLEXINC) 

# dependencies: every .o from its .C + every recursively included .h- - - - -

$(SOLVERDIR)Solver.o: $(SOLVERDIR)Solver.cpp $(SOLVERH) $(OUTILH)
	$(CC) -c $*.cpp -o $@ $(SW)  $(OUTILINC) $(CPLEXFLAGS)

########################## End of makefile ###################################
