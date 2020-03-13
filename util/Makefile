.SUFFIXES:	.c

BINDIR=../../bin/util

CC = gcc
OBJFILES = adler32.o compress.o crc32.o deflate.o gzio.o infblock.o infcodes.o \
inffast.o inflate.o inftrees.o infutil.o trees.o uncompr.o zutil.o zhrink.c

default: $(OBJFILES)
	$(CC) -o $(BINDIR)/zhrink $(OBJFILES)

clean:
	rm -f *.o

adler32.o: adler32.c zlib.h zconf.h
compress.o: compress.c zlib.h zconf.h
crc32.o: crc32.c zlib.h zconf.h
deflate.o: deflate.c deflate.h zutil.h zlib.h zconf.h
gzio.o: gzio.c zutil.h zlib.h zconf.h
infblock.o: infblock.c zutil.h zlib.h zconf.h infblock.h inftrees.h \
 infcodes.h infutil.h
infcodes.o: infcodes.c zutil.h zlib.h zconf.h inftrees.h infblock.h \
 infcodes.h infutil.h inffast.h
inffast.o: inffast.c zutil.h zlib.h zconf.h inftrees.h infblock.h \
 infcodes.h infutil.h inffast.h
inflate.o: inflate.c zutil.h zlib.h zconf.h infblock.h
inftrees.o: inftrees.c zutil.h zlib.h zconf.h inftrees.h inffixed.h
infutil.o: infutil.c zutil.h zlib.h zconf.h infblock.h inftrees.h \
 infcodes.h infutil.h
trees.o: trees.c deflate.h zutil.h zlib.h zconf.h trees.h
uncompr.o: uncompr.c zlib.h zconf.h
zutil.o: zutil.c zutil.h zlib.h zconf.h
zhrink.o: zhrink.c zlib.h

.c.o:
	$(CC) -c $<
