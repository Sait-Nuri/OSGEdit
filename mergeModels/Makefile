CC=g++
CFLAGS=-c -Wall
LDFLAGS=-losg -losgGA -losgDB -losgViewer -losgText -losgUtil -lOpenThreads
SOURCES=main.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=mergeModels
LIBPATH=/usr/local/lib64

all: 
	$(CC) $(SOURCES) $(LDFLAGS) -o $(EXECUTABLE)

clean:
	rm *.o -R 
