CC = gcc
CFLAGS = -pthread -Wconversion -Wall
DEBUG = -DDEBUG

default: readin.o conversions.o
	$(CC) $(CFLAGS) readin.o conversions.o -o readin 

conversions.o: conversions.c conversions.h
	$(CC) $(CFLAGS) conversions.c conversions.h -c
	
readin.o: readin.c readin.h
	$(CC) $(CFLAGS) readin.c readin.h -c

debug: readin_d.o conversions_d.o
	$(CC) $(CFLAGS) $(DEBUG) readin.o conversions.o -o readin_debug

conversions_d.o: conversions.c conversions.h
	$(CC) $(CFLAGS) $(DEBUG) conversions.c conversions.h -c
	
readin_d.o: readin.c readin.h
	$(CC) $(CFLAGS) $(DEBUG) readin.c readin.h -c
	
clean:
	rm *.o *.h.gch readin readin_debug
