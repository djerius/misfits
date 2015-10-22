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

#include <misfits/table.hpp>

#include <numeric>
#include <algorithm>
#include <functional>

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


	if ( e9 > 1 ) extent.insert( extent.begin(), e9 );
	if ( e8 > 1 ) extent.insert( extent.begin(), e8 );
	if ( e7 > 1 ) extent.insert( extent.begin(), e7 );
	if ( e6 > 1 ) extent.insert( extent.begin(), e6 );
	if ( e5 > 1 ) extent.insert( extent.begin(), e5 );
	if ( e4 > 1 ) extent.insert( extent.begin(), e4 );
	if ( e3 > 1 ) extent.insert( extent.begin(), e3 );
	if ( e2 > 1 ) extent.insert( extent.begin(), e2 );
	if ( e1 > 1 ) extent.insert( extent.begin(), e1 );

	extent.insert( extent.begin(), e0 );

    }

    Extent&
    Extent::resize(int size ) {
	extent.resize( size );
	return *this;
    }

    Extent&
    Extent::clear( ) {
	extent.clear();
	return *this;
    }

    Extent&
    Extent::add( LONGLONG e ) {
	extent.push_back( e );
	return *this ;
    }

    LONGLONG
    Extent::nelem() const {
	return std::accumulate( extent.begin(), extent.end(),
				1,
				std::multiplies<LONGLONG>()
				);
    }


    int
    Extent::naxes() const {
	return extent.size();
    }


    bool operator == ( const Extent& a0,
		       const Extent& b0 ) {

	ExtentT a( a0() );
	ExtentT b( b0() );

	ExtentT::iterator a_end = remove( a.begin(), a.end(), 1 );
	ExtentT::iterator b_end = remove( b.begin(), b.end(), 1 );

	a.resize( a_end - a.begin() );
	b.resize( b_end - b.begin() );

	return a == b;
    }


}
