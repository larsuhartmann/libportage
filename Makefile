CC = icc
LS = ${CC}
DX = doxygen

check-syntax:
	${CC} -o nul -Wall -S ${CHK_SOURCES}

devdoc:
	${DX} doxygen/dev-doc

apidoc:
	${DX} doxygen/api-doc
