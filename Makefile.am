include create_am_macros.mk

ACLOCAL_AMFLAGS		= -I m4
AM_DEFAULT_SOURCE_EXT	= .cc

EXTRA_DIST              += $(PACKAGE_NAME).pc.in
CLEANFILES              += $(PACKAGE_NAME).pc

pkgconfigdatadir        = $(libdir)/pkgconfig
pkgconfigdata_DATA      = $(PACKAGE_NAME).pc

# Any files make built
CLEANFILES		+=

# Any files configure built
DISTCLEANFILES		+=

# Any files maintainer built(with autoconf, automake, etc)
MAINTAINERCLEANFILES	+= \
			Makefile.in	\
			aclocal.m4	\
			compile		\
			config.guess	\
			config.h.in	\
			config.sub	\
			configure	\
			depcomp		\
			install-sh	\
			ltmain.sh	\
			missing		\
			mkinstalldirs


AM_CPPFLAGS		+= $(PACKAGE_CPPFLAGS) -I$(top_srcdir)
AM_LDFLAGS		= $(PACKAGE_LDFLAGS)
LDADD                   =  $(PACKAGE_LIBS)

# include %D%/docs/Makefile.am
include %D%/misfits/Makefile.am
include %D%/tests/Makefile.am

