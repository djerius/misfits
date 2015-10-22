include %D%/autotest.mk
include %D%/use_gtest.mk

TESTSUITE_DEPS		+= %D%/testsuite.at

AUTOTEST_PATH           += :$(abs_builddir)/%D%

LDADD_%C% 		= $(LDADD) $(LDADD_GTEST)

CPPFLAGS_%C%		=			\
			-I %D%			\
			-I $(srcdir)/%D%	\
			$(AM_CPPFLAGS)		\
			$(CPPFLAGS_GTEST)


check_PROGRAMS		+=			\
			%D%/read_row		\
			%D%/reopen		\
			%D%/temptable		\
			%D%/writeable

TEST_FITS_FILENAME      = file.fits

LDADD_%C%_TESTS		=				\
			$(LDADD_%C%)			\
			$(top_builddir)/misfits/libmisfits.la

CPPFLAGS_%C%_TESTS	=						\
			-DTEST_FITS_FILENAME=$(TEST_FITS_FILENAME)	\
			$(CPPFLAGS_%C%)					\
			-I$(top_srcdir)					\
			-I$(top_builddir)

##############################

%C%_read_row_SOURCES	=			\
			%D%/read_row.cc		\
			%D%/util.hpp		\
			%D%/util.cc

%C%_read_row_LDADD	= $(LDADD_%C%_TESTS)
%C%_read_row_CPPFLAGS	= $(CPPFLAGS_%C%_TESTS)

##############################

%C%_writeable_SOURCES	=			\
			%D%/writeable.cc	\
			%D%/util.hpp		\
			%D%/util.cc

%C%_writeable_LDADD	= $(LDADD_%C%_TESTS)
%C%_writeable_CPPFLAGS	= $(CPPFLAGS_%C%_TESTS)


##############################

%C%_temptable_SOURCES	=			\
			%D%/temptable.cc	\
			%D%/util.hpp		\
			%D%/util.cc

%C%_temptable_LDADD	= $(LDADD_%C%_TESTS)
%C%_temptable_CPPFLAGS	= $(CPPFLAGS_%C%_TESTS)

##############################

%C%_reopen_SOURCES	=			\
			%D%/reopen.cc	\
			%D%/util.hpp		\
			%D%/util.cc

%C%_reopen_LDADD	= $(LDADD_%C%_TESTS)
%C%_reopen_CPPFLAGS	= $(CPPFLAGS_%C%_TESTS)
