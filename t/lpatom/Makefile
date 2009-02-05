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

atom_parse.test: atom_parse.test.c atom_positive.txt
	${CC} -o atom_parse.test ${CFLAGS} ../../lib/liblputil.c ../../lib/liblpatom.c atom_parse.test.c

check-syntax:
	${CC} -o nul -Wall ${CFLAGS} -S ${CHK_SOURCES}

