#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.57])

#---------------------------------------------------------
AC_INIT([misfits],[0.0.15],[opticsoft@head.cfa.harvard.edu],[misfits])
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

AX_CXX_COMPILE_STDCXX_11([noext],[optional])

MST_POD_GEN_DOCS
m4_include(prog_help.ac)

#---------------------------------------------------------
# Checks for typedefs, structures, and compiler characteristics.

# require boost.Core
BOOST_REQUIRE(1.56)
BOOST_FILESYSTEM
# shouldn't this be done for me?
BOOST_FILESYSTEM_LIBS="$BOOST_FILESYSTEM_LIBS $BOOST_SYSTEM_LIBS"
STDCXX_CPPFLAGS=

AC_CHECK_TYPES(
    [uint8_t],
    [UINT8_T=-DHAVE_STD__UINT8_T],
    [
     BOOST_FIND_HEADER(
	 [boost/cstdint.hpp],
	 [AC_MSG_ERROR([Cannot find support for uint8_t (either from compiler or Boost); cannot continue])],
	 [UINT8_T=-DHAVE_BOOST__UINT8_T
	  USE_BOOST_STDCXX=1
	 ]
     )
    ],
    [[#include <cstdint>]],
)
STDCXX_CPPFLAGS="$STDCXX_CPPFLAGS $UINT8_T"


AC_CHECK_TYPES(
    [int16_t],
    [INT16_T=-DHAVE_STD__INT16_T],
    [
     BOOST_FIND_HEADER(
	 [boost/cstdint.hpp],
	 [AC_MSG_ERROR([Cannot find support for int16_t (either from compiler or Boost); cannot continue])],
	 [INT16_T=-DHAVE_BOOST__INT16_T
	  USE_BOOST_STDCXX=1
	 ]
     )
    ],
    [[#include <cstdint>]],
)
STDCXX_CPPFLAGS="$STDCXX_CPPFLAGS $INT16_T"

AC_CHECK_TYPES(
    [uint16_t],
    [UINT16_T=-DHAVE_STD__UINT16_T],
    [
     BOOST_FIND_HEADER(
	 [boost/cstdint.hpp],
	 [AC_MSG_ERROR([Cannot find support for uint16_t (either from compiler or Boost); cannot continue])],
	 [UINT16_T=-DHAVE_BOOST__UINT16_T
	  USE_BOOST_STDCXX=1
	 ]
     )
    ],
    [[#include <cstdint>]],
)
STDCXX_CPPFLAGS="$STDCXX_CPPFLAGS $UINT16_T"


AC_CHECK_TYPES(
    [int32_t],
    [INT32_T=-DHAVE_STD__INT32_T],
    [
     BOOST_FIND_HEADER(
	 [boost/cstdint.hpp],
	 [AC_MSG_ERROR([Cannot find support for int32_t (either from compiler or Boost); cannot continue])],
	 [INT32_T=-DHAVE_BOOST__INT32_T
	  USE_BOOST_STDCXX=1
	 ]
     )
    ],
    [[#include <cstdint>]],
)
STDCXX_CPPFLAGS="$STDCXX_CPPFLAGS $INT32_T"

AC_CHECK_TYPES(
    [uint32_t],
    [UINT32_T=-DHAVE_STD__UINT32_T],
    [
     BOOST_FIND_HEADER(
	 [boost/cstdint.hpp],
	 [AC_MSG_ERROR([Cannot find support for uint32_t (either from compiler or Boost); cannot continue])],
	 [UINT32_T=-DHAVE_BOOST__UINT32_T
	  USE_BOOST_STDCXX=1
	 ]
     )
    ],
    [[#include <cstdint>]],
)
STDCXX_CPPFLAGS="$STDCXX_CPPFLAGS $UINT32_T"


AC_CHECK_TYPES(
    [int64_t],
    [INT64_T=-DHAVE_STD__INT64_T],
    [
     BOOST_FIND_HEADER(
	 [boost/cstdint.hpp],
	 [AC_MSG_ERROR([Cannot find support for int64_t (either from compiler or Boost); cannot continue])],
	 [INT64_T=-DHAVE_BOOST__INT64_T
	  USE_BOOST_STDCXX=1
	 ]
     )
    ],
    [[#include <cstdint>]],
)
STDCXX_CPPFLAGS="$STDCXX_CPPFLAGS $INT64_T"


AC_CHECK_TYPES(
   [std__shared_ptr],
   [SHARED_PTR=-DHAVE_STD__SHARED_PTR],
   [
     BOOST_FIND_HEADER(
	 [boost/shared_ptr.hpp],
	 [AC_MSG_ERROR([Cannot find support for shared_ptr (either from compiler or Boost); cannot continue])],
	 [SHARED_PTR=-DHAVE_BOOST__SHARED_PTR
	  USE_BOOST_STDCXX=1
         ]
     )
   ],
   [[#include <memory>
     typedef std::shared_ptr<int> std__shared_ptr;
   ]],
 )
STDCXX_CPPFLAGS="$STDCXX_CPPFLAGS $SHARED_PTR"

AC_CHECK_TYPES(
    [std__weak_ptr],
    [WEAK_PTR=-DHAVE_STD__WEAK_PTR],
    [
      BOOST_FIND_HEADER(
	  [boost/weak_ptr.hpp],
	  [AC_MSG_ERROR([Cannot find support for weak_ptr (either from compiler or Boost); cannot continue])],
	  [WEAK_PTR=-DHAVE_BOOST__WEAK_PTR
	   USE_BOOST_STDCXX=1
	  ]
      )
    ],
    [[#include <memory>
      typedef std::weak_ptr<int> std__weak_ptr;
      ]],
)
STDCXX_CPPFLAGS="$STDCXX_CPPFLAGS $WEAK_PTR"

AC_CHECK_TYPES(
    [std__unique_ptr],
    [UNIQUE_PTR=-DHAVE_STD__UNIQUE_PTR],
    [
      BOOST_FIND_HEADER(
	  [boost/move/unique_ptr.hpp],
	  [AC_MSG_ERROR([Cannot find support for unique_ptr (either from compiler or Boost); cannot continue])],
	  [UNIQUE_PTR=-DHAVE_BOOST__UNIQUE_PTR
	   USE_BOOST_STDCXX=1
	  ]
      )
    ],
    [[#include <memory>
      typedef std::unique_ptr<int> std__unique_ptr;
      ]],
)
STDCXX_CPPFLAGS="$STDCXX_CPPFLAGS $UNIQUE_PTR"

AC_CHECK_TYPES(
    [std__enable_shared_from_this],
    [ENABLE_SHARED_FROM_THIS=-DHAVE_STD__ENABLE_SHARED_FROM_THIS],
    [
      BOOST_FIND_HEADER(
	  [boost/enable_shared_from_this.hpp],
	  [AC_MSG_ERROR([Cannot find support for enable_shared_from_this (either from compiler or Boost); cannot continue])],
	  [ENABLE_SHARED_FROM_THIS=-DHAVE_BOOST__ENABLE_SHARED_FROM_THIS
	   USE_BOOST_STDCXX=1
	  ]
      )
    ],
    [[#include <memory>
      typedef std::enable_shared_from_this<int> std__enable_shared_from_this;
    ]]
)
STDCXX_CPPFLAGS="$STDCXX_CPPFLAGS $ENABLE_SHARED_FROM_THIS"


# available in Boost 1.58, but we can work around it if it's not available
AC_MSG_NOTICE([checking for weak_from_this])
AC_MSG_CHECKING([for std::weak_from_this])
AC_COMPILE_IFELSE(
    [
      dnl might be in the standard ...
      AC_LANG_PROGRAM(
        [[#include <memory>
	    class foo : public std::enable_shared_from_this<foo> {
	       std::weak_ptr<foo> get() { return weak_from_this() ; }
	    };]],
	[[foo boo; return 0;]]
      )
    ],
    [AC_MSG_RESULT([yes])
     WEAK_FROM_THIS=-DHAVE_STD__WEAK_FROM_THIS
    ],
    [AC_MSG_RESULT([no])
     dnl look for boost version only if we're not using the system enable_shared_from_this
     AS_IF(
         [test "$ENABLE_SHARED_FROM_THIS" != -DHAVE_STD__ENABLE_SHARED_FROM_THIS ],
	 [
	   AC_MSG_NOTICE([checking for boost::weak_from_this])
	   BOOST_FIND_HEADER(
	       [boost/enable_shared_from_this.hpp],
	       [AC_MSG_RESULT([weak_from_this not found])],
	       [
		 CPPFLAGS_SAVE=$CPPFLAGS
		 CPPFLAGS="$CPPFLAGS $BOOST_CPPFLAGS"
		 AC_COMPILE_IFELSE(
		     [
		       AC_LANG_PROGRAM(
			   [[#include <boost/enable_shared_from_this.hpp>
			    class foo : public boost::enable_shared_from_this<foo> {
			       boost::weak_ptr<foo> get() { return weak_from_this() ; }
			    };]],
			  [[foo boo; return 0; ]]
		       )
		     ],
		     [AC_MSG_RESULT([found boost::weak_from_this])
		      WEAK_FROM_THIS=-DHAVE_BOOST__WEAK_FROM_THIS
		      USE_BOOST_STDCXX=1
		     ],
		     [AC_MSG_RESULT([didn't find boost::weak_from_this])]
		 )
		 CPPFLAGS=$CPPFLAGS_SAVE
	       ]
	    )
         ]
     )
  ]
)

STDCXX_CPPFLAGS="$STDCXX_CPPFLAGS $WEAK_FROM_THIS"



test "x$USE_BOOST_STDCXX" = 1 \
     && STDCXX_CPPFLAGS="$BOOST_CPPFLAGS $STDCXX_CPPFLAGS"

AX_AM_OVERRIDE_FINALIZE

AC_SUBST([PKGCONFIG_REQUIRES],["own_or_observe_ptr cfitsio"])
AM_SUBST_NOTMAKE([PKGCONFIG_REQUIRES])

PKG_CHECK_MODULES([MISFITS],[own_or_observe_ptr cfitsio >= 3.39])

AC_SUBST([PACKAGE_CPPFLAGS],["$STDCXX_CPPFLAGS $BOOST_CPPFLAGS $MISFITS_CFLAGS"])
AC_SUBST([PACKAGE_LDFLAGS],["$BOOST_FILESYSTEM_LDFLAGS $MISFITS_LDFLAGS"])
AC_SUBST([PACKAGE_LIBS],["$BOOST_FILESYSTEM_LIBS $MISFITS_LIBS"])


AC_SUBST([PKGCONFIG_CPPFLAGS],[$STDCXX_CPPFLAGS])
AM_SUBST_NOTMAKE([PKGCONFIG_CPPFLAGS])

AC_SUBST([PKGCONFIG_LDFLAGS],[$BOOST_FILESYSTEM_LDFLAGS])
AM_SUBST_NOTMAKE([PKGCONFIG_LDFLAGS])

AC_SUBST([PKGCONFIG_LIBS],[$BOOST_FILESYSTEM_LIBS])
AM_SUBST_NOTMAKE([PKGCONFIG_LIBS])

AC_CHECK_SIZEOF([long])
CPPFLAGS_SAVE=$CPPFLAGS
CPPFLAGS=$PACKAGE_CPPFLAGS
AC_CHECK_SIZEOF([LONGLONG],[],[#include <fitsio.h>])

AS_IF([test "$ac_cv_sizeof_LONGLONG" = 0],
      [AC_MSG_ERROR([Unable to determine size of CFITSIO LONGLONG type])])

CPPFLAGS=$CPPFLAGS_SAVE

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
