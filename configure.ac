#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.57])

#---------------------------------------------------------
AC_INIT([misfits],[0.0.1],[opticsoft@head.cfa.harvard.edu],[misfits])
AC_CONFIG_SRCDIR([misfits/fits.cc])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_TESTDIR([tests])
AC_CONFIG_HEADERS([config.h misfits/config.hpp])
AM_INIT_AUTOMAKE([ check-news subdir-objects ])

AC_LANG([C++])

AX_AM_OVERRIDE_VAR(CXXFLAGS)

#---------------------------------------------------------
# Checks for programs.
AC_PROG_CXX
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LIBTOOL

AX_AM_OVERRIDE_FINALIZE

MST_POD_GEN_DOCS
m4_include(prog_help.ac)

#---------------------------------------------------------
# Checks for libraries.

# Use pkg-config to check for libraries.
#e.g. PKG_CHECK_MODULES([raygen],[Exception >= 1.0.2])
PKG_CHECK_MODULES([MISFITS],[
				cfitsio
				boost_base
				boost_filesystem
			     ])

#---------------------------------------------------------
# Checks for header files.

#---------------------------------------------------------
# Checks for typedefs, structures, and compiler characteristics.

AC_CHECK_HEADERS([memory],
	[AC_CHECK_TYPES( [std__shared_ptr], [],[],
			 [[#include <memory>
			   typedef std::shared_ptr<int> std__shared_ptr;
			   ]],
	 )
	 AC_CHECK_TYPES( [std__unique_ptr], [],[],
			 [[#include <memory>
			   typedef std::unique_ptr<int> std__unique_ptr;
			   ]],
         )
	 AC_CHECK_TYPES( [std__weak_ptr], [],[],
			 [[#include <memory>
			   typedef std::weak_ptr<int> std__weak_ptr;
			   ]],
         )
	 AC_CHECK_TYPES( [std__enable_shared_from_this], [],[],
			 [[#include <memory>
			   typedef std::enable_shared_from_this<int> std__enable_shared_from_this;
			 ]]
	 )
	 AC_MSG_CHECKING([for weak_from_this])
	 AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
			   [[#include <memory>
			     class foo : public std::enable_shared_from_this<foo> {
				std::weak_ptr<foo> get() { return weak_from_this() ; }
			     };]],
			   [[ int main( void ) { foo boo; return 0; } ]]
			  )],
			  [AC_MSG_RESULT([yes])
			   AC_DEFINE([HAVE_STD__WEAK_FROM_THIS],1,[has std::weak_from_this])
			  ],
			  [AC_MSG_RESULT([no])]
        )
	]
)

AX_CXX_HAVE_REF()

#---------------------------------------------------------
# test dependencies

MST_TESTDEP_SETUP

MST_GTEST

MST_TESTDEP_STATUS

#---------------------------------------------------------
# Checks for library functions.


#---------------------------------------------------------
# Generate configurtion files
AC_CONFIG_FILES(
	[Makefile
	 tests/atlocal
	 misfits.pc
	],
	)

AC_OUTPUT
