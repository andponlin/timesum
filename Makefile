# =============================================================================
# Copyright 2020, Andrew Lindesay <apl@lindesay.co.nz>.
# All rights reserved. Distributed under the terms of the MIT License.
# =============================================================================

VERSIONMAJOR=1
VERSIONMIDDLE=0
VERSIONMINOR=0
VERSION=$(VERSIONMAJOR).$(VERSIONMIDDLE).$(VERSIONMINOR)

XXD=xxd
CC=gcc
RM=rm -f
ECHO=@echo

CFLAGSOTHER=-Wall -c -I . -DTIMESUM_VERSION=\"$(VERSION)\"

ifdef DEBUG
	CFLAGS=-g -gdwarf-2 -ggdb -DDEBUG
	LDFLAGS=-g -gdwarf-2 -ggdb
else
	CFLAGS=-O
	LDFLAGS=-dead_strip
endif

all: timesum test

test: timesum
	./test.sh

timesum: timesum.o
	$(CC) timesum.o -o timesum $(LDFLAGS)

usage.h: usage.txt
	$(XXD) -i usage.txt usage.h

timesum.o: timesum.c usage.h
	$(CC) $(CFLAGS) $(CFLAGSOTHER) -o timesum.o timesum.c

clean:
	$(RM) timesum.o
	$(RM) timesum
	$(RM) usage.h

