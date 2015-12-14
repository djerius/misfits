// --8<--8<--8<--8<--
//
// Copyright (C) 2015 Smithsonian Astrophysical Observatory
//
// This file is part of misfits
//
// misfits is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or (at
// your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// -->8-->8-->8-->8--

// -*-c++-*-

#ifndef misFITS_misCONFIG_H
#define misFITS_misCONFIG_H

#include <memory>

#ifdef HAVE_STD__SHARED_PTR

    namespace misFITS {
	using std::shared_ptr;
	using std::make_shared;
    }

#elif HAVE_BOOST__SHARED_PTR

    #include <boost/shared_ptr.hpp>
    #include <boost/make_shared.hpp>

    namespace misFITS {
	using boost::shared_ptr;
	using boost::make_shared;
    }

#endif

#ifdef HAVE_STD__UNIQUE_PTR

    namespace misFITS {
	using std::unique_ptr;
	using std::make_shared;
    }

#elif HAVE_BOOST__UNIQUE_PTR

    #include <boost/move/unique_ptr.hpp>

    namespace misFITS {
	using boost::movelib::unique_ptr;
    }

#endif


#ifdef HAVE_STD__WEAK_PTR

    namespace misFITS {
	using std::weak_ptr;
	using std::bad_weak_ptr;
    }

#elif HAVE_BOOST__WEAK_PTR

    #include <boost/weak_ptr.hpp>

    namespace misFITS {
	using boost::weak_ptr;
	using boost::bad_weak_ptr;
    }

#endif


#ifdef HAVE_STD__ENABLE_SHARED_FROM_THIS

    namespace misFITS {
	using std::enable_shared_from_this;
    }

#elif HAVE_BOOST__ENABLE_SHARED_FROM_THIS

    #include <boost/enable_shared_from_this.hpp>

    namespace misFITS {
	using boost::enable_shared_from_this;
    }

#endif

#ifdef HAVE_STD__UINT8_T

#include <cstdint>

    namespace misFITS {

	using std::uint8_t;

    }

#elif HAVE_BOOST__UINT8_T

    #include <boost/cstdint.hpp>

    namespace misFITS {

	using boost::uint8_t;

    }

#endif


#endif  // ! misFITS_misCONFIG_H


