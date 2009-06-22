docdir = $(datadir)/doc/@PACKAGE@

# only add asciidoc stuff if asciidoc is enabled.
if USE_ASCIIDOC
doc_DATA = xpak.5.text
man_MANS = xpak.5
MOSTLYCLEANFILES = xpak.5.text xpak.5 xpak.5.xml
EXTRA_DIST = xpak.5.txt

xpak.5.text: xpak.5.txt
	a2x -d manpage -f text xpak.5.txt

xpak.5: xpak.5.txt
	a2x -d manpage -f manpage xpak.5.txt
endif
