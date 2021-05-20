# The name of your C compiler:
CC=gcc

# You may need to adjust these cc options:
CFLAGS= -O2 -I. -Iargparse -Ijpeg-6b -ISDL2-2.0.14\x86_64-w64-mingw32\include -IContainers\src\include -std=c99

# Link-time cc options:
LDFLAGS= -L argparse -largparse -LSDL2-2.0.14/x86_64-w64-mingw32\lib -LContainers -lmingw32 -largparse -lm -lSDL2 -Ljpeg-6b -ljpeg -lcontainers

# To link any special libraries, add the necessary -l commands here.
LDLIBS= 

# miscellaneous OS-dependent stuff
LN=$(CC)
RM=rm -f
CP=cp
MV=mv
AR=ar rc
AR2=ranlib

# End of configurable options.


all: teojpg copy_release

teojpg: 	jpeg.o pixel.o thomson.o ordereddither.o floyddither.o mediancut.o octree.o \
				wu.o colourconverter.o powf.o log.o jpeg-6b/libjpeg.a teojpg.c
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

jpeg.o: jpeg.c
	$(CC) $(CFLAGS) -c $< -o $@

pixel.o: pixel.c
	$(CC) $(CFLAGS) -c $< -o $@

thomson.o: thomson.c
	$(CC) $(CFLAGS) -c $< -o $@

ordereddither.o: ordereddither.c
	$(CC) $(CFLAGS) -c $< -o $@

floyddither.o: floyddither.c
	$(CC) $(CFLAGS) -c $< -o $@

mediancut.o: mediancut.c
	$(CC) $(CFLAGS) -c $< -o $@

octree.o: octree.c
	$(CC) $(CFLAGS) -c $< -o $@

wu.o: wu.c
	$(CC) $(CFLAGS) -c $< -o $@

powf.o: powf.c
	$(CC) $(CFLAGS) -c $< -o $@

log.o: log.c
	$(CC) $(CFLAGS) -c -o $@ $<

colourconverter.o: colourconverter.c
	$(CC) $(CFLAGS) -c -o $@ $<
	
copy_release:
	$(MV) teojpg.exe release
	$(RM) *.o *.a
	$(CP) SDL2-2.0.14\x86_64-w64-mingw32\bin\SDL2.dll release
	$(CP) Containers\containers.dll release
	
	
clean:
	$(RM) *.o *.a teojpg.exe
	
indent:
	uncrustify --replace -c C:/ProgramData/chocolatey/lib/uncrustify/tools/cfg/linux.cfg *.c  *.h
	$(RM) *unc-backup*

indent_clean:
	$(RM) *unc-backup*
