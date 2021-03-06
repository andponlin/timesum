# =============================================================================
# Copyright 2020-2021, Andrew Lindesay <apl@lindesay.co.nz>.
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
CAT=@cat
GO=go

CFLAGSOTHER=-Wall -c -I . -DTIMESUM_VERSION=\"$(VERSION)\"

ifdef DEBUG
	CFLAGS=-g -gdwarf-2 -ggdb -DDEBUG
	LDFLAGS=-g -gdwarf-2 -ggdb
else
	CFLAGS=-O
	LDFLAGS=-dead_strip
endif

all: timesum test timesumgo testgo

test: timesum
	./test.sh timesum

testgo: timesumgo
	$(GO) test -v timesum_test.go timesum.go usage.go
	./test.sh timesumgo

timesum: timesum.o
	$(CC) timesum.o -o timesum $(LDFLAGS)

timesumgo: timesum.go usage.go
	$(GO) build -o timesumgo timesum.go usage.go

usage.go: usage.txt
	$(ECHO) "// ** this file is auto-generated **" > $@
	$(ECHO) "package main" > $@
	$(ECHO) "const version = \"$(VERSION)\"" >> $@
	$(ECHO) -n "const help = \`" >> $@
	$(CAT) usage.txt >> $@
	$(ECHO) "\`" >> $@

usage.h: usage.txt
	$(XXD) -i usage.txt usage.h

timesum.o: timesum.c usage.h
	$(CC) $(CFLAGS) $(CFLAGSOTHER) -o timesum.o timesum.c

clean:
	$(RM) timesum.o
	$(RM) timesum
	$(RM) timesumgo
	$(RM) usage.h
	$(RM) usage.go

