##############################################################################
################################ makefile ####################################
##############################################################################
#									     #
#   makefile of TabuSearch							     #
#									     #
#   Input:  $(CC)     = compiler command				     #
#           $(SW)     = compiler options                                     #
#           $(TABUDIR) = the directory where the TestFi source is	     #
#									     #
#   Output: $(TABUOBJ) = the final object(s) / library			     #
#           $(TABUH)   = the .h files to include for TestFi	  	     #
#           $(TABUINC) = the -I$(TestFi directory)			     #
#									     #
#                                VERSION 2.00				     #
#                	        20 - 02 - 2013				     #
#									     #
##############################################################################

# clean - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

clean::
	rm -f $(TABUDIR)*.o $(TABUDIR)*~
	


# macroes to be exported- - - - - - - - - - - - - - - - - - - - - - - - - - -

TABULIB = 
TABUOBJ = $(TABUDIR)TabuSearch.o
TABUH   = $(TABUDIR)TabuSearch.hpp
TABUINC = -I$(TABUDIR) 

# dependencies: every .o from its .C + every recursively included .h- - - - -

$(TABUDIR)TabuSearch.o: $(TABUDIR)TabuSearch.cpp $(TABUH) $(OUTILH) $(NHGENH) $(SOLVERH)
	$(CC) -c $*.cpp -o $@  $(OUTILINC) $(NHGENINC) $(SOLVERINC) $(SW) 

########################## End of makefile ###################################
