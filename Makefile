#Make file for BinExtractor

CC=gcc
CFLAGS=-c -g -Wall -Wno-unused-result -Wno-strict-aliasing -O2

LD=gcc
LDFLAGS=

SOURCE=src/APHeader.c src/BinExtractor.c src/GPT.c
DIR=build
OBJECTS=$(DIR)/APHeader.o $(DIR)/BinExtractor.o $(DIR)/GPT.o

EXECUTABLE=BinExtractor

all: $(DIR) $(OBJECTS) $(EXECUTABLE)

$(DIR):
	test -d $(DIR) || mkdir $(DIR)

build/BinExtractor.o: src/GPT.h src/BinExtractor.h src/APHeader.h src/BinExtractor.c
	$(CC) $(CFLAGS) src/BinExtractor.c -o build/BinExtractor.o

build/GPT.o: src/GPT.h src/BinExtractor.h src/GPT.c
	$(CC) $(CFLAGS) src/GPT.c -o build/GPT.o

build/APHeader.o: src/APHeader.h src/BinExtractor.h src/APHeader.c
	$(CC) $(CFLAGS) src/APHeader.c -o build/APHeader.o

$(EXECUTABLE): $(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) -o build/$(EXECUTABLE)
