##############################################################################
################################ makefile ####################################
##############################################################################
#									     #
#   makefile of NeighborGen							     #
#									     #
#   Input:  $(CC)     = compiler command				     #
#           $(SW)     = compiler options                                     #
#           $(NHGENDIR) = the directory where the TestFi source is	     #
#									     #
#   Output: $(NHGENOBJ) = the final object(s) / library			     #
#           $(NHGENH)   = the .h files to include for TestFi	  	     #
#           $(NHGENINC) = the -I$(TestFi directory)			     #
#									     #
#                                VERSION 2.00				     #
#                	        20 - 02 - 2013				     #
#									     #
##############################################################################

# clean - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

clean::
	rm -f $(NHGENDIR)*.o $(NHGENDIR)*~
	


# macroes to be exported- - - - - - - - - - - - - - - - - - - - - - - - - - -

NHGENLIB = 
NHGENOBJ = $(NHGENDIR)NeighborGen.o
NHGENH   = $(NHGENDIR)NeighborGen.hpp
NHGENINC = -I$(NHGENDIR) 

# dependencies: every .o from its .C + every recursively included .h- - - - -

$(NHGENDIR)NeighborGen.o: $(NHGENDIR)NeighborGen.cpp $(NHGENH) $(OUTILH)
	$(CC) -c $*.cpp -o $@  $(OUTILINC) $(SW)

########################## End of makefile ###################################
