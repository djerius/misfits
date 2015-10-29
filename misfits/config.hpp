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

#ifdef HAVE_BOOST_BASE

#ifndef HAVE_STD__SHARED_PTR

#include <boost/shared_ptr.hpp>

namespace std {
    using boost::shared_ptr;
}

#define HAVE_STD__SHARED_PTR

#endif	// ! HAVE_STD__SHARED_PTR


#ifndef HAVE_STD__UNIQUE_PTR

#include <boost/move/unique_ptr.hpp>

namespace std {
    using boost::movelib::unique_ptr;
}

#endif	// ! HAVE_STD__UNIQUE_PTR


#ifndef HAVE_STD__WEAK_PTR

#include <boost/weak_ptr.hpp>

namespace std {
    using boost::weak_ptr;
}

#endif // ! HAVE_STD__WEAK_PTR


#ifndef HAVE_STD__ENABLE_SHARED_FROM_THIS

#include <boost/enable_shared_from_this.hpp>

namespace std {
    using boost::enable_shared_from_this;
}

#define HAVE_STD__ENABLE_SHARED_FROM_THIS
// boost has this
#define HAVE_STD__WEAK_FROM_THIS

#endif // ! HAVE_STD__ENABLE_SHARED_FROM_THIS

#endif // HAVE_BOOST_BASE


#endif  // ! misFITS_misCONFIG_H


