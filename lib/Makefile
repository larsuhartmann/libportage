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

CFLAGS = -O0 ${INCS} -DVERSION=\"${VERSION}\" -pipe
#CFLAGS = -g -Wall -O0 ${INCS} -DVERSION=\"${VERSION}\" -pipe

LDFLAGS = -Wl,-z,defs ${LIBS} -lc
#LDFLAGS = -g -Wl,-z,defs ${LIBS} -lc

SOFLAGS = -fPIC -shared

all: libportage

libportage: liblputil.a liblputil.so liblpxpak.a liblpxpak.so liblpatom.a liblpatom.so

.c.o:
	@echo CC $<
	@${CC} -c ${CFLAGS} $<

liblputil.a: liblputil.o
	@echo AR $@
	@${AR} $@ liblputil.o
	@${RANLIB} $@

liblpxpak.a: liblpxpak.o liblputil.o
	@echo AR $@
	@${AR} $@ liblputil.o liblpxpak.o
	@${RANLIB} $@

liblpatom.a: liblpatom.o liblputil.o
	@echo AR $@
	@${AR} $@ liblputil.o liblpatom.o
	@${RANLIB} $@

liblputil.so: liblputil.h
	@echo CC $@
	@echo LD $@
	@${CC} ${CFLAGS} ${SOFLAGS} ${LDFLAGS} -o $@ liblputil.c

liblpxpak.so: liblpxpak.h liblputil.so
	@echo CC $@
	@echo LD $@
	@${CC} ${CFLAGS} ${SOFLAGS} ${LDFLAGS} -llputil -o $@ liblpxpak.c

liblpatom.so: liblpatom.h liblputil.so
	@echo CC $@
	@echo LD $@
	@${CC} ${CFLAGS} ${SOFLAGS} ${LDFLAGS} -llputil -o $@ liblpatom.c

.PHONY: clean
clean:
	@echo cleaning
	@${RM} -f *.a *.so *.o

# target for flymake-mode
check-syntax:
	${CC} -o nul -Wall ${CFLAGS} -c -S ${CHK_SOURCES}
