VERSION = 0.1

PREFIX = /usr/local
MANPREFIX = ${PREFIX}/share/man

AR = ar cr
CC = cc
LD = ld
DX = doxygen
RANLIB = ranlib
RM = rm

INCS = -I.
LIBS = -L.

CFLAGS = -O0 ${INCS} -DVERSION=\"${VERSION}\" -fPIC -pipe
#CFLAGS = -g -Wall -O0 ${INCS} -DVERSION=\"${VERSION}\" -pipe

LDFLAGS = -z,defs ${LIBS}
#LDFLAGS = -g -z,defs ${LIBS}

SOFLAGS = -shared

all: libportage

libportage: liblputil.a liblputil.so liblpxpak.a liblpxpak.so liblpatom.a liblpatom.so

.c.o:
	@echo CC $<
	@${CC} -c ${CFLAGS} $<

liblputil.a: liblputil.o
	@echo AR $@
	@${AR} $@ $^
	@${RANLIB} $@

liblpxpak.a: liblpxpak.o
	@echo AR $@
	@${AR} $@ $^
	@${RANLIB} $@

liblpatom.a: liblpatom.o
	@echo AR $@
	@${AR} $@ $^
	@${RANLIB} $@

liblputil.so: liblputil.o
	@echo LD $@
	@${LD} ${SOFLAGS} ${LDFLAGS} -o $@ $^ -lc

liblpxpak.so: liblputil.so liblpxpak.o
	@echo LD $@
	@${LD} ${SOFLAGS} ${LDFLAGS} -o $@ -llputil liblpxpak.o -lc

liblpatom.so: liblputil.so liblpatom.o
	@echo LD $@
	@${LD} ${SOFLAGS} ${LDFLAGS} -llputil -o $@ liblpatom.o -lc

.PHONY: clean
clean:
	@echo cleaning
	@${RM} -f *.a *.so *.o

# target for flymake-mode
check-syntax:
	${CC} -o nul -Wall ${CFLAGS} -c -S ${CHK_SOURCES}
