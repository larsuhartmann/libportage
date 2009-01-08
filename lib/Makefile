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
LIBS = -L/usr/lib -lc -L.

CFLAGS = -O0 ${INCS} -DVERSION=\"${VERSION}\"
#CFLAGS = -g -Wall -O0 ${INCS} -DVERSION=\"${VERSION}\"

#LDFLAGS = ${LIBS}
LDFLAGS = -g ${LIBS}

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
	@echo LD $@
	@${CC} -c ${CFLAGS} -fPIC -o liblputil.od liblputil.c
	@${LD} ${SOFLAGS} -o $@ liblputil.od
	@${RM} liblputil.od

liblpxpak.so: liblpxpak.h
	@echo LD $@
	@${CC} -c ${CFLAGS} -fPIC -o liblpxpak.od liblpxpak.c
	@${LD} ${SOFLAGS} -o $@ liblpxpak.od
	@${RM} liblpxpak.od

liblpatom.so: liblpatom.h
	@echo LD $@
	@${CC} -c ${CFLAGS} -fPIC -o liblpatom.od liblpatom.c
	@${LD} ${SOFLAGS} -o $@ liblpatom.od
	@${RM} liblpatom.od

.PHONY: clean
clean:
	@echo cleaning
	@${RM} -f *.a *.so *.o

# target for flymake-mode
check-syntax:
	${CC} -o nul -Wall ${CFLAGS} -c -S ${CHK_SOURCES}
