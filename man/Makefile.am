## Process this file with automake to produce Makefile.in

man_MANS = spew.1
BUILT_SOURCES = spew.1
CLEANFILES = spew.1
EXTRA_DIST = spew.1.in


spew.1:		spew.1.in
	sed -e 's/SPEW_VERSION/$(VERSION)/' $^ >$@


install-data-local:
	(cd $(mandir)/man1 && $(LN_S) -f spew.1 snarf.1)
	(cd $(mandir)/man1 && $(LN_S) -f spew.1 regorge.1)

help2man:
	help2man --section=1 --no-info --name="measures I/O performance and/or generates I/O load" --include="spew.1.include" --output=spew.1.in ../src/spew
