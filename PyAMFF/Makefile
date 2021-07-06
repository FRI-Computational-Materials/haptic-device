LIBS += libAMFF.a

OBJECTS = PyAMFF.o 

FC=gfortran
# Note that the optimization flag is overruled  with 0
# Seems to be necessary for the interface between fortran and gcc
#%.o: %.f90
#	$(FC) $(FFLAGS) -c $< -o $@
all: libPyAMFF.a
LIB := libPyAMFF.a

$(LIB) : $(OBJECTS)
	ar cru $@ $^
	ranlib $@


clean :
	rm -f $(OBJECTS) $(LIB)
	rm -f libAMFF.a

