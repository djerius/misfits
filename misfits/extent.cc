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

#include <numeric>
#include <algorithm>
#include <functional>
#include <sstream>

#include <misfits/extent.hpp>


namespace misFITS {

    Extent::Extent
    (
     LONGLONG e0,
     LONGLONG e1,
     LONGLONG e2,
     LONGLONG e3,
     LONGLONG e4,
     LONGLONG e5,
     LONGLONG e6,
     LONGLONG e7,
     LONGLONG e8,
     LONGLONG e9 ) {

	push_back( e0 );
	if ( e1 > 1 ) push_back( e1 );
	if ( e2 > 1 ) push_back( e2 );
	if ( e3 > 1 ) push_back( e3 );
	if ( e4 > 1 ) push_back( e4 );
	if ( e5 > 1 ) push_back( e5 );
	if ( e6 > 1 ) push_back( e6 );
	if ( e7 > 1 ) push_back( e7 );
	if ( e8 > 1 ) push_back( e8 );
	if ( e9 > 1 ) push_back( e9 );
    }

    LONGLONG
    Extent::nelem() const {
	return std::accumulate( begin(), end(),
				1,
				std::multiplies<LONGLONG>()
				);
    }

    Extent
    Extent::squeeze() const {

	Extent sqz( *this );
	Extent::iterator end = remove( sqz.begin(), sqz.end(), 1 );
	sqz.erase( end, sqz.end() );
	return sqz;
    }


    bool operator == ( const Extent& a,
		       const Extent& b ) {

	return static_cast<ExtentType>( a.squeeze() ) == static_cast<ExtentType>( b.squeeze() );
    }

    bool operator != ( const Extent& a0, const Extent& b0 ) {
	return ! ( a0 == b0 ) ;
    }

    std::string
    to_string( const Extent& extent ) {

	std::ostringstream tdim;

	tdim << '(';
	ExtentType::size_type idx;
	ExtentType::size_type naxes = extent.naxes();

	for ( idx = 0 ; idx < naxes - 1 ; idx++ ) {

	    tdim << extent[idx] << ',';
	}

	tdim << extent[idx] << ')';
	return tdim.str();
    }


}
