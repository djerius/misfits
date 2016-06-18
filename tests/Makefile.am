include %D%/autotest.mk
include %D%/use_gtest.mk


TESTSUITE_DEPS		+= %D%/testsuite.at

AUTOTEST_PATH           += :$(abs_builddir)/%D%

TEST_FITS_FILENAME      = file.fits

LDADD_%C% 		=			\
			$(MISFITS_LIBS)		\
			$(LDADD)		\
			$(LDADD_GTEST)

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
			$(CPPFLAGS_%C%)

%C%_libtest_utils_la_CXXFLAGS =			\
			$(CXXFLAGS_%C%)

%C%_libtest_utils_la_SOURCES  =			\
			%D%/columns.hpp		\
			%D%/columns.cc		\
			%D%/fiducial.hpp	\
			%D%/fiducial.cc		\
			%D%/fiducial_data.hpp	\
			%D%/fiducial_data.cc	\
			%D%/util.hpp		\
			%D%/util.cc		\
			%D%/row_utils.hpp	\
			%D%/row_utils.cc

##############################

# these must be explicitly linked into test programs so that they
# register tests properly.  simply adding them to libtest_utils won't
# work, as they use startup code to register things, and aren't called
# by name

%C%_row_test_SOURCES =				\
			%D%/row_test.hpp	\
			%D%/row_test.cc



##############################

LDADD_%C%_TESTS		=					\
			%D%/libtest_utils.la			\
			$(top_builddir)/misfits/libmisfits.la	\
			$(LDADD_%C%)

CPPFLAGS_%C%_TESTS	=			\
			$(CPPFLAGS_%C%)		\
			-I$(top_srcdir)		\
			-I$(top_builddir)

CXXFLAGS_%C%_TESTS	= $(CXXFLAGS_%C%)


##############################

check_PROGRAMS		+= %D%/file

%C%_file_SOURCES	=			\
			%D%/file.cc

%C%_file_LDADD		= $(LDADD_%C%_TESTS)
%C%_file_CPPFLAGS	= $(CPPFLAGS_%C%_TESTS)
%C%_file_CXXFLAGS	= $(CXXFLAGS_%C%_TESTS)

##############################

check_PROGRAMS		+= %D%/gen_fits

%C%_gen_fits_SOURCES	=			\
			%D%/gen_fits.cc

%C%_gen_fits_LDADD	= $(LDADD_%C%_TESTS)
%C%_gen_fits_CPPFLAGS	= $(CPPFLAGS_%C%_TESTS)
%C%_gen_fits_CXXFLAGS	= $(CXXFLAGS_%C%_TESTS)

##############################

check_PROGRAMS		+= %D%/read_row

%C%_read_row_SOURCES	=			\
			%D%/read_row.cc		\
			$(%C%_row_test_SOURCES)

%C%_read_row_LDADD	= $(LDADD_%C%_TESTS)
%C%_read_row_CPPFLAGS	= $(CPPFLAGS_%C%_TESTS)
%C%_read_row_CXXFLAGS	= $(CXXFLAGS_%C%_TESTS)

##############################

check_PROGRAMS		+= %D%/write_row

%C%_write_row_SOURCES	=			\
			%D%/write_row.cc	\
			$(%C%_row_test_SOURCES)

%C%_write_row_LDADD	= $(LDADD_%C%_TESTS)
%C%_write_row_CPPFLAGS	= $(CPPFLAGS_%C%_TESTS)
%C%_write_row_CXXFLAGS	= $(CXXFLAGS_%C%_TESTS)

##############################

check_PROGRAMS		+= %D%/writeable

%C%_writeable_SOURCES	=			\
			%D%/writeable.cc

%C%_writeable_LDADD	= $(LDADD_%C%_TESTS)
%C%_writeable_CPPFLAGS	= $(CPPFLAGS_%C%_TESTS)
%C%_writeable_CXXFLAGS	= $(CXXFLAGS_%C%_TESTS)


##############################

check_PROGRAMS		+= %D%/table

%C%_table_SOURCES	=			\
			%D%/table.cc

%C%_table_LDADD		= $(LDADD_%C%_TESTS)
%C%_table_CPPFLAGS	= $(CPPFLAGS_%C%_TESTS)
%C%_table_CXXFLAGS	= $(CXXFLAGS_%C%_TESTS)

##############################

check_PROGRAMS		+= %D%/reopen

%C%_reopen_SOURCES	=			\
			%D%/reopen.cc

%C%_reopen_LDADD	= $(LDADD_%C%_TESTS)
%C%_reopen_CPPFLAGS	= $(CPPFLAGS_%C%_TESTS)
%C%_reopen_CXXFLAGS	= $(CXXFLAGS_%C%_TESTS)

##############################

check_PROGRAMS		+= %D%/row

%C%_row_SOURCES	=			\
			%D%/row.cc

%C%_row_LDADD		= $(LDADD_%C%_TESTS)
%C%_row_CPPFLAGS	= $(CPPFLAGS_%C%_TESTS)
%C%_row_CXXFLAGS	= $(CXXFLAGS_%C%_TESTS)

##############################

check_PROGRAMS		+= %D%/keywords

%C%_keywords_SOURCES	=			\
			%D%/keywords.cc

%C%_keywords_LDADD	= $(LDADD_%C%_TESTS)
%C%_keywords_CPPFLAGS	= $(CPPFLAGS_%C%_TESTS)
%C%_keywords_CXXFLAGS	= $(CXXFLAGS_%C%_TESTS)

##############################

check_PROGRAMS		+= %D%/cfitsio

%C%_cfitsio_SOURCES	=			\
			%D%/cfitsio.cc

%C%_cfitsio_LDADD	= $(LDADD_%C%_TESTS)
%C%_cfitsio_CPPFLAGS	= $(CPPFLAGS_%C%_TESTS)
%C%_cfitsio_CXXFLAGS	= $(CXXFLAGS_%C%_TESTS)

##############################

check_PROGRAMS		+= %D%/column_info

%C%_column_info_SOURCES	=			\
			%D%/column_info.cc

%C%_column_info_LDADD	= $(LDADD_%C%_TESTS)
%C%_column_info_CPPFLAGS	= $(CPPFLAGS_%C%_TESTS)
%C%_column_info_CXXFLAGS	= $(CXXFLAGS_%C%_TESTS)

##############################

check_PROGRAMS		+= %D%/extent

%C%_extent_SOURCES	=			\
			%D%/extent.cc

%C%_extent_LDADD	= $(LDADD_%C%_TESTS)
%C%_extent_CPPFLAGS	= $(CPPFLAGS_%C%_TESTS)
%C%_extent_CXXFLAGS	= $(CXXFLAGS_%C%_TESTS)

##############################

check_PROGRAMS		+= %D%/test_bitset

%C%_test_bitset_SOURCES	=			\
			%D%/test_bitset.cc

%C%_test_bitset_LDADD		= $(LDADD_%C%_TESTS)
%C%_test_bitset_CPPFLAGS	= $(CPPFLAGS_%C%_TESTS)
%C%_test_bitset_CXXFLAGS	= $(CXXFLAGS_%C%_TESTS)

##############################

check_PROGRAMS		+= %D%/flush

%C%_flush_SOURCES	=			\
			%D%/flush.cc

%C%_flush_LDADD		= $(LDADD_%C%_TESTS)
%C%_flush_CPPFLAGS	= $(CPPFLAGS_%C%_TESTS)
%C%_flush_CXXFLAGS	= $(CXXFLAGS_%C%_TESTS)
