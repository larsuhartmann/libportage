CC = cc
LS = ${CC}

check-syntax:
	${CC} -o nul -Wall -S ${CHK_SOURCES}

