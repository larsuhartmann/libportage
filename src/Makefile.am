lib_LTLIBRARIES = libportage.la

libportage_la_SOURCES = liblpatom.c liblputil.c liblpxpak.c

AM_CPPFLAGS = -I$(top_srcdir)/include
METASOURCES = AUTO

# dev-targets
check-syntax:
	$(COMPILE) -o nul -Wall -c -S ${CHK_SOURCES}
