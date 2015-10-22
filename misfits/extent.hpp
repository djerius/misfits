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

#ifndef misFITS_EXTENT_H
#define misFITS_EXTENT_H

#include <misfits/fits.hpp>
#include <misfits/table.hpp>

namespace misFITS {

    typedef std::vector<LONGLONG> ExtentT;

    class Extent {

    public:
	Extent( LONGLONG e0 = 1,
		LONGLONG e1 = 1,
		LONGLONG e2 = 1,
		LONGLONG e3 = 1,
		LONGLONG e4 = 1,
		LONGLONG e5 = 1,
		LONGLONG e6 = 1,
		LONGLONG e7 = 1,
		LONGLONG e8 = 1,
		LONGLONG e9 = 1 );

	Extent& add( LONGLONG e );
	Extent& clear();
	Extent& resize( int size);

	LONGLONG nelem() const;
	int size() const;
	const ExtentT& operator () () const { return extent; }

    private:
	ExtentT extent;
    };

    bool operator == ( const Extent& a0, const Extent& b0 );

}

#endif // ! misFITS_EXTENT_H
