OBJECTS     = fluids.o
CFILES      = $(OBJECTS:.o=.cpp)
EXECFILE    = smoke
FFTW        = fftw-2.1.5
INCLUDEDIRS = -I ${FFTW}/include/
LIBDIRS     = $(FFTW)/lib
LIBS        = -framework GLUT -framework OpenGL -lrfftw -lfftw
CFLAGS      = -O2 -Wall -pipe -Wno-deprecated-declarations
LINKFLAGS   = 


.SILENT:

all: $(EXECFILE)

$(EXECFILE): $(OBJECTS)
		g++ $(LINKFLAGS) $(OBJECTS) -o $(EXECFILE) -L$(LIBDIRS) $(LIBS)

.cpp.o: $$@.cpp $$@.h
		g++ $(CFLAGS) $(INCLUDEDIRS) -c  $<

clean:
		-rm -rf $(OBJECTS) $(EXECFILE)

depend:
		g++ -MM $(CFILES) > make.dep

-include make.dep
