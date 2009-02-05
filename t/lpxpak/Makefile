PREFIX = /usr/local
MANPREFIX = ${PREFIX}/share/man

CC = cc
LD = ${CC}

INCS = -I../../include
LIBS = -L/usr/lib -lc -L.

CFLAGS = -O0 ${INCS} -g
#CFLAGS = -g -Wall -O0 ${INCS} -DVERSION=\"${VERSION}\"
LDFLAGS = ${LIBS}
#LDFLAGS = -g ${LIBS}

xpak_parse.test: xpak_parse.test.c xpak_parse.test.tbz2
	${CC} -o xpak_parse.test ${CFLAGS} ../../lib/liblputil.c ../../lib/liblpxpak.c xpak_parse.test.c

check-syntax:
	${CC} -o nul -Wall ${CFLAGS} -S ${CHK_SOURCES}

