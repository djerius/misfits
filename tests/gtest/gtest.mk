noinst_LTLIBRARIES      += %D%/libgtest.la

%C%_libgtest_la_SOURCES	 =			\
			 %D%/gtest-all.cc	\
                         %D%/gtest_main.cc

%C%_libgtest_la_CPPFLAGS =				\
			 -I $(top_srcdir)/%D%/..	\
			 -I $(top_builddir)/%D%/..

%C%_libgtest_la_CXXFLAGS = $(GTEST_CXXFLAGS)

noinst_HEADERS          += %D%/gtest.h
