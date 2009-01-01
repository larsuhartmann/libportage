CC = cc
LS = ${CC}
DX = doxygen

check-syntax:
	${CC} -o nul -Wall -S ${CHK_SOURCES}

doxydoc:
	${DX} -d Doxyfile

