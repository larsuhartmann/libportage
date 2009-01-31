DX = doxygen
DXPATH = doxygen

devdoc:
	${DX} ${DXPATH}/dev-doc

apidoc:
	${DX} ${DXPATH}/api-doc
