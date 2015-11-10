include %D%/autotest.mk
include %D%/use_gtest.mk


TESTSUITE_DEPS		+= %D%/testsuite.at

AUTOTEST_PATH           += :$(abs_builddir)/%D%

TEST_FITS_FILENAME      = file.fits

LDADD_%C% 		= $(MISFITS_LIBS) $(LDADD) $(LDADD_GTEST) %D%/libtest_utils.la

CPPFLAGS_%C%		=						\
			-I %D%						\
			-I $(srcdir)/%D%				\
			$(AM_CPPFLAGS)					\
			-DTEST_FITS_FILENAME=$(TEST_FITS_FILENAME)	\
			$(CPPFLAGS)

CXXFLAGS_%C%		=			\
			$(GTEST_CXXFLAGS)


##############################

noinst_LTLIBRARIES	+= %D%/libtest_utils.la

%C%_libtest_utils_la_CPPFLAGS =			\
			$(AM_CPPFLAGS)		\
			$(CPPFLAGS_%C%)

%C%_libtest_utils_la_SOURCES  =			\
			%D%/util.hpp		\
			%D%/util.cc


##############################

LDADD_%C%_TESTS		=					\
			$(LDADD_%C%)				\
			$(top_builddir)/misfits/libmisfits.la

CPPFLAGS_%C%_TESTS	=						\
			$(CPPFLAGS_%C%)					\
			-I$(top_srcdir)					\
			-I$(top_builddir)

CXXFLAGS_%C%_TESTS	= $(CXXFLAGS_%C%)


##############################

check_PROGRAMS		+= %D%/gen_fits

%C%_gen_fits_SOURCES	=			\
			%D%/gen_fits.cc		\
			%D%/util.hpp

%C%_gen_fits_LDADD	= $(LDADD_%C%_TESTS)
%C%_gen_fits_CPPFLAGS	= $(CPPFLAGS_%C%_TESTS)
%C%_gen_fits_CXXFLAGS	= $(CXXFLAGS_%C%_TESTS)

##############################

check_PROGRAMS		+= %D%/read_row

%C%_read_row_SOURCES	=			\
			%D%/read_row.cc		\
			%D%/util.hpp

%C%_read_row_LDADD	= $(LDADD_%C%_TESTS)
%C%_read_row_CPPFLAGS	= $(CPPFLAGS_%C%_TESTS)
%C%_read_row_CXXFLAGS	= $(CXXFLAGS_%C%_TESTS)

##############################

check_PROGRAMS		+= %D%/writeable

%C%_writeable_SOURCES	=			\
			%D%/writeable.cc		\
			%D%/util.hpp

%C%_writeable_LDADD	= $(LDADD_%C%_TESTS)
%C%_writeable_CPPFLAGS	= $(CPPFLAGS_%C%_TESTS)
%C%_writeable_CXXFLAGS	= $(CXXFLAGS_%C%_TESTS)


##############################

check_PROGRAMS		+= %D%/table

%C%_table_SOURCES	=			\
			%D%/table.cc		\
			%D%/util.hpp

%C%_table_LDADD		= $(LDADD_%C%_TESTS)
%C%_table_CPPFLAGS	= $(CPPFLAGS_%C%_TESTS)
%C%_table_CXXFLAGS	= $(CXXFLAGS_%C%_TESTS)

##############################

check_PROGRAMS		+= %D%/reopen

%C%_reopen_SOURCES	=			\
			%D%/reopen.cc		\
			%D%/util.hpp

%C%_reopen_LDADD	= $(LDADD_%C%_TESTS)
%C%_reopen_CPPFLAGS	= $(CPPFLAGS_%C%_TESTS)
%C%_reopen_CXXFLAGS	= $(CXXFLAGS_%C%_TESTS)
