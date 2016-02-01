OBJECTS     = fluids.o
CFILES      = $(OBJECTS:.o=.c)
EXECFILE    = smoke
FFTW        = /Users/freerkvandijk/git/scientific_visualization/fftw-2.1.5
INCLUDEDIRS = -I /Users/freerkvandijk/git/scientific_visualization/fftw-2.1.5/include
LIBDIRS     = $(FFTW)/lib
LIBS        = -framework GLUT -framework OpenGL -lrfftw -lfftw
CFLAGS      = -O2 -Wall -pipe
LINKFLAGS   = 


.SILENT:

all: $(EXECFILE)

$(EXECFILE): $(OBJECTS)
		cc $(LINKFLAGS) $(OBJECTS) -o $(EXECFILE) -L$(LIBDIRS) $(LIBS)

.c.o: $$@.c $$@.h
		cc $(CFLAGS) $(INCLUDEDIRS) -c  $<

clean:
		-rm -rf $(OBJECTS) $(EXECFILE)

depend:
		gcc -MM $(CFILES) > make.dep

-include make.dep
