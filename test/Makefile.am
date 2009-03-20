METASOURCES = AUTO

check_PROGRAMS = 01_lputil_splitstr 02_lpatom_parse 03_lpxpak
TESTS = 01_lputil_splitstr 02_lpatom_parse 03_lpxpak

01_lputil_splitstr_SOURCES = 01_lputil_splitstr.c 01_lputil_splitstr.txt
01_lputil_splitstr_LDFLAGS = $(all_libraries)
01_lputil_splitstr_LDADD = ../src/libportage.la

02_lpatom_parse_SOURCES = 02_lpatom_parse.c 02_lpatom_parse.txt
02_lpatom_parse_LDFLAGS = $(all_libraries)
02_lpatom_parse_LDADD = ../src/libportage.la

03_lpxpak_SOURCES = 03_lpxpak.c 03_lpxpak.tbz2
03_lpxpak_LDFLAGS = $(all_libraries)
03_lpxpak_LDADD = ../src/libportage.la

AM_CPPFLAGS = -I$(top_srcdir)/include

# dev-targets
check-syntax:
	$(COMPILE) -o nul -Wall -c -S ${CHK_SOURCES}
