### Makefile --- 

## Author: Gaspar Fernández <blakeyed@totaki.com>
## Version: $Id: Makefile,v 0.0 2015/03/21 13:12:00 gaspy Exp $
## Keywords: 
## X-URL: 

CC=g++
CFLAGS=-g -std=c++11
LIBS=-lpthread -lssl -lcrypto

SRCFILES = anapacheapp.cpp		\
	lib/gcommon/gcommon.cpp		\
	lib/gcommon/cfileutils.cpp	\
	lib/gcommon/gstr.cpp

SOURCES=anapache.cpp $(SRCFILES)
OBJECTS=$(SOURCES:.cpp=.o)
INCLUDES=

EXECUTABLE=main

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $(INCLUDES) $(OBJECTS) $(LIBS) -o $@

.cpp.o:
	$(CC) -c $(CFLAGS) $(DESKTOP_NOTIFICATION) $(INCLUDES) $< -o $@

clean:
	rm -rf $(OBJECTS) 
	rm -rf $(EXECUTABLE)
### Makefile ends here
