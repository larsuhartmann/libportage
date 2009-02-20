METASOURCES = AUTO

check_PROGRAMS = 01_lpatom_parse 02_lpxpak_parse
TESTS = 01_lpatom_parse 02_lpxpak_parse

01_lpatom_parse_SOURCES = 01_lpatom_parse.c 01_lpatom_parse.txt
01_lpatom_parse_LDFLAGS = $(all_libraries)
01_lpatom_parse_LDADD = $(top_srcdir)/src/libportage.la

02_lpxpak_parse_SOURCES = 02_lpxpak_parse.c 02_lpxpak_parse.tbz2
02_lpxpak_parse_LDFLAGS = $(all_libraries)
02_lpxpak_parse_LDADD = $(top_srcdir)/src/libportage.la

AM_CPPFLAGS = -I$(top_srcdir)/include -g

# dev-targets
check-syntax:
	$(COMPILE) -o nul -Wall -c -S ${CHK_SOURCES}
