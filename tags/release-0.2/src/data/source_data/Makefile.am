INCLUDES = $(all_includes)
METASOURCES = AUTO
source_datadir = $(pkgdatadir)/source_data
source_data_DATA = BookTemplate.xml MenuTemplate.xml GenerateBooks.py templating.py

EXTRA_DIST = $(source_data_DATA)
SUBDIRS = books_source

install-data-hook:
	chmod +x $(srcdir)/GenerateBooks.py
	$(srcdir)/GenerateBooks.py

