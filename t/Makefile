PREFIX = /usr/local
MANPREFIX = ${PREFIX}/share/man

CC = cc
LD = ${CC}

INCS = -I../include
LIBS = -L/usr/lib -lc -L.

CFLAGS = -O0 ${INCS}
#CFLAGS = -g -Wall -O0 ${INCS} -DVERSION=\"${VERSION}\"
LDFLAGS = ${LIBS}
#LDFLAGS = -g ${LIBS}

check-syntax:
	${CC} -o nul -Wall ${CFLAGS} -S ${CHK_SOURCES}

atom_test:
	${CC} -o atom_test ${CFLAGS} ../lib/liblputil.c ../lib/liblpatom.c atom_test.c

xpak_test:
	${CC} -o xpak_test ${CFLAGS} ../lib/liblputil.c ../lib/liblpxpak.c xpak_test.c

test: atom_test xpak_test
	./atom_test
	./xpak_test
