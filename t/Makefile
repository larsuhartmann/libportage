PREFIX = /usr/local
MANPREFIX = ${PREFIX}/share/man

CC = cc
LD = ${CC}

INCS = -I../include
LIBS = -L/usr/lib -lc -L.

CFLAGS = -O0 ${INCS} -g
#CFLAGS = -g -Wall -O0 ${INCS} -DVERSION=\"${VERSION}\"
LDFLAGS = ${LIBS}
#LDFLAGS = -g ${LIBS}

atom_tests := lpatom
xpak_tests := lpxpak

tests := $(atom_tests) $(xpak_tests)

$(tests):
	$(MAKE) --directory=$@

test: $(tests) test.sh
	@./test.sh
