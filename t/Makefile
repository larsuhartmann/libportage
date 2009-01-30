PREFIX = /usr/local
MANPREFIX = ${PREFIX}/share/man

CC = cc
LD = ${CC}
DX = doxygen

INCS = -I../lib
LIBS = -L/usr/lib -lc -L.

CFLAGS = -O0 ${INCS}
#CFLAGS = -g -Wall -O0 ${INCS} -DVERSION=\"${VERSION}\"
LDFLAGS = ${LIBS}
#LDFLAGS = -g ${LIBS}

check-syntax:
	${CC} -o nul -Wall ${CFLAGS} -S ${CHK_SOURCES}

devdoc:
	${DX} ../doxygen/dev-doc

apidoc:
	${DX} ../doxygen/api-doc
