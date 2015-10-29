#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.57])

#---------------------------------------------------------
AC_INIT([misfits],[0.0.1],[opticsoft@head.cfa.harvard.edu],[misfits])
AC_CONFIG_SRCDIR([misfits/fits.cc])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_TESTDIR([tests])
AM_INIT_AUTOMAKE([ check-news  silent-rules color-tests parallel-tests subdir-objects ])

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

#---------------------------------------------------------
# Checks for header files.

#---------------------------------------------------------
# Checks for typedefs, structures, and compiler characteristics.


USE_BOOST=0
STDCXX_CPPFLAGS=
AC_CHECK_HEADERS([memory],
	[AC_CHECK_TYPES( [std__shared_ptr],
			 [STDCXX_CPPFLAGS="$STDCXX_CPPFLAGS -DHAVE_STD__SHARED_PTR"],
			 [USE_BOOST=1],
			 [[#include <memory>
			   typedef std::shared_ptr<int> std__shared_ptr;
			   ]],
	 )
	 AC_CHECK_TYPES( [std__unique_ptr],
			 [STDCXX_CPPFLAGS="$STDCXX_CPPFLAGS -DHAVE_STD__UNIQUE_PTR"],
			 [USE_BOOST=1],
			 [[#include <memory>
			   typedef std::unique_ptr<int> std__unique_ptr;
			   ]],
         )
	 AC_CHECK_TYPES( [std__weak_ptr],
			 [STDCXX_CPPFLAGS="$STDCXX_CPPFLAGS -DHAVE_STD__WEAK_PTR"],
			 [USE_BOOST=1],
			 [[#include <memory>
			   typedef std::weak_ptr<int> std__weak_ptr;
			   ]],
         )
	 AC_CHECK_TYPES( [std__enable_shared_from_this],
			 [STDCXX_CPPFLAGS="$STDCXX_CPPFLAGS -DHAVE_STD__ENABLE_SHARED_FROM_THIS"],
			 [USE_BOOST=1],
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
			   STDCXX_CPPFLAGS="$STDCXX_CPPFLAGS -DHAVE_STD__WEAK_FROM_THIS"
			  ],
			  [AC_MSG_RESULT([no])]
        )
	]
)

# this doesn't seem to work on OSX clang 7.0.0?
AX_CXX_HAVE_REF()

PKGCONFIG_REQUIRES="own_or_observe_ptr cfitsio boost_filesystem"

AS_IF( [test $USE_BOOST = 1],
       [ PKGCONFIG_REQUIRES="$PKGCONFIG_REQUIRES boost_base"
         # technically we don't know that we have Boost until after
         # PKG_CHECK_MODULES is called, but since we can't do anything
         # without Boost, there's no harm in putting this here
         STDCXX_CPPFLAGS="$STDCXX_CPPFLAGS -DHAVE_BOOST_BASE"
       ],
       [
         # for now, need boost for bind and ref; can't quite get the
         # code to work with clang 7.0.0's implementation
         PKGCONFIG_REQUIRES="$PKGCONFIG_REQUIRES boost_base"
         STDCXX_CPPFLAGS="$STDCXX_CPPFLAGS -DHAVE_BOOST_BASE"
       ]
)

# Use pkg-config to check for libraries.
#e.g. PKG_CHECK_MODULES([raygen],[Exception >= 1.0.2])
PKG_CHECK_MODULES([MISFITS],[$PKGCONFIG_REQUIRES])

MISFITS_CPPFLAGS="$STDCXX_CPPFLAGS $MISFITS_CFLAGS"
AC_SUBST([MISFITS_CPPFLAGS])

AC_SUBST([PKGCONFIG_CPPFLAGS],[$STDCXX_CPPFLAGS])
AM_SUBST_NOTMAKE([PKGCONFIG_CPPFLAGS])

AC_SUBST([PKGCONFIG_REQUIRES])
AM_SUBST_NOTMAKE([PKGCONFIG_REQUIRES])


#---------------------------------------------------------
# test dependencies

MST_GTEST


#---------------------------------------------------------
# Generate configurtion files
AC_CONFIG_FILES(
	[Makefile
	 tests/atlocal
	 misfits.pc
	],
	)

AC_OUTPUT
