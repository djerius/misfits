LDADD_GTEST =  %D%/gtest/libgtest.la -lpthread
CPPFLAGS_GTEST = $(AM_CPPFLAGS) $(CPPFLAGS) -I %D% -I$(top_srcdir)/tests
include %D%/gtest/Makefile.am
