all: preadwritetest.exe preadwritetest

preadwritetest: preadwritetest.c
	gcc preadwritetest.c -g -o preadwritetest

preadwritetest.exe: preadwrite.c preadwrite.h preadwritetestwindows.c
	i686-w64-mingw32-gcc preadwrite.c preadwritetestwindows.c -g -o preadwritetest.exe

clean: 
	rm -f preadwritetest.exe preadwritetest
