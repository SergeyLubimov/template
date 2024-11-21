MODULE_big = template

OBJS =  \
	$(WIN32RES) \
	template.o 

EXTENSION = template
REGRESS = template

DATA = template--1.0.sql

ifdef USE_PGXS
PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
else
subdir = contrib/template
top_builddir = ../..
include $(top_builddir)/src/Makefile.global
include $(top_srcdir)/contrib/contrib-global.mk
endif


