VERSION = 0.1

PREFIX = /usr/local
MANPREFIX = ${PREFIX}/share/man

AR = ar cr
CC = cc
LD = ld
RANLIB = ranlib
RM = rm

INCS = -I../include
LIBS = -L.

CFLAGS = -O0 ${INCS} -DVERSION=\"${VERSION}\" -fPIC -pipe
#CFLAGS = -g -Wall -O0 ${INCS} -DVERSION=\"${VERSION}\" -pipe

LDFLAGS = -z,defs ${LIBS}
#LDFLAGS = -g -z,defs ${LIBS}

SOFLAGS = -shared

all: libportage

libportage: liblputil.a liblputil.so liblpxpak.a liblpxpak.so liblpatom.a\
            liblpatom.so

.c.o:
	@echo CC $<
	@${CC} -c ${CFLAGS} $<

liblputil.a: liblputil.o
	@echo AR $@
	@${AR} $@ $<
	@${RANLIB} $@

liblpxpak.a: liblpxpak.o
	@echo AR $@
	@${AR} $@ $<
	@${RANLIB} $@

liblpatom.a: liblpatom.o
	@echo AR $@
	@${AR} $@ $<
	@${RANLIB} $@

liblputil.so: liblputil.o
	@echo LD $@
	@${LD} ${SOFLAGS} ${LDFLAGS} -o $@ $< -lc

liblpxpak.so: liblpxpak.o liblputil.so
	@echo LD $@
	@${LD} ${SOFLAGS} ${LDFLAGS} -llputil -o $@ $< -lc

liblpatom.so: liblpatom.o liblputil.so
	@echo LD $@
	@${LD} ${SOFLAGS} ${LDFLAGS} -llputil -o $@ $< -lc

clean:
	@echo cleaning
	@${RM} -f *.a *.so *.o

# target for flymake-mode
check-syntax:
	${CC} -o nul -Wall ${CFLAGS} -c -S ${CHK_SOURCES}
