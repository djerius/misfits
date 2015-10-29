include %D%/autotest.mk
include %D%/use_gtest.mk

TESTSUITE_DEPS		+= %D%/testsuite.at

AUTOTEST_PATH           += :$(abs_builddir)/%D%

LDADD_%C% 		= $(MISFITS_LIBS) $(LDADD) $(LDADD_GTEST)

CPPFLAGS_%C%		=			\
			-I %D%			\
			-I $(srcdir)/%D%	\
			$(AM_CPPFLAGS)		\
			$(CPPFLAGS)

CXXFLAGS_%C%		=			\
			$(GTEST_CXXFLAGS)


TEST_FITS_FILENAME      = file.fits

LDADD_%C%_TESTS		=					\
			$(LDADD_%C%)				\
			$(top_builddir)/misfits/libmisfits.la

CPPFLAGS_%C%_TESTS	=						\
			-DTEST_FITS_FILENAME=$(TEST_FITS_FILENAME)	\
			$(CPPFLAGS_%C%)					\
			-I$(top_srcdir)					\
			-I$(top_builddir)

CXXFLAGS_%C%_TESTS	= $(CXXFLAGS_%C%)

##############################

check_PROGRAMS		+= %D%/read_row

%C%_read_row_SOURCES	=			\
			%D%/read_row.cc		\
			%D%/util.hpp		\
			%D%/util.cc

%C%_read_row_LDADD	= $(LDADD_%C%_TESTS)
%C%_read_row_CPPFLAGS	= $(CPPFLAGS_%C%_TESTS)
%C%_read_row_CXXFLAGS	= $(CXXFLAGS_%C%_TESTS)

##############################

check_PROGRAMS		+= %D%/writeable

%C%_writeable_SOURCES	=			\
			%D%/writeable.cc	\
			%D%/util.hpp		\
			%D%/util.cc

%C%_writeable_LDADD	= $(LDADD_%C%_TESTS)
%C%_writeable_CPPFLAGS	= $(CPPFLAGS_%C%_TESTS)
%C%_writeable_CXXFLAGS	= $(CXXFLAGS_%C%_TESTS)


##############################

check_PROGRAMS		+= %D%/table

%C%_table_SOURCES	=			\
			%D%/table.cc		\
			%D%/util.hpp		\
			%D%/util.cc

%C%_table_LDADD		= $(LDADD_%C%_TESTS)
%C%_table_CPPFLAGS	= $(CPPFLAGS_%C%_TESTS)
%C%_table_CXXFLAGS	= $(CXXFLAGS_%C%_TESTS)

##############################

check_PROGRAMS		+= %D%/temptable

%C%_temptable_SOURCES	=			\
			%D%/temptable.cc	\
			%D%/util.hpp		\
			%D%/util.cc

%C%_temptable_LDADD	= $(LDADD_%C%_TESTS)
%C%_temptable_CPPFLAGS	= $(CPPFLAGS_%C%_TESTS)
%C%_temptable_CXXFLAGS	= $(CXXFLAGS_%C%_TESTS)

##############################

check_PROGRAMS		+= %D%/reopen

%C%_reopen_SOURCES	=			\
			%D%/reopen.cc		\
			%D%/util.hpp		\
			%D%/util.cc

%C%_reopen_LDADD	= $(LDADD_%C%_TESTS)
%C%_reopen_CPPFLAGS	= $(CPPFLAGS_%C%_TESTS)
%C%_reopen_CXXFLAGS	= $(CXXFLAGS_%C%_TESTS)
